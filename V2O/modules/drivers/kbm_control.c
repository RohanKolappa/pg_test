#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#include "fpga.h"
#include "fpga_ioctl.h"
#include "legacyv2d.h"
#include "kbm_control.h"
#include "log_control.h"

static ssize_t kb_read(struct file *filp, char *buff,
                       size_t count, loff_t *offp);
static ssize_t kb_write(struct file *filp, const char *buff,
                        size_t count, loff_t *offp);
static loff_t kb_llseek(struct file *filp,  loff_t off, int whence);
static unsigned int kb_poll(struct file *filp, poll_table *wait);

static ssize_t ms_read(struct file *filp, char *buff,
                       size_t count, loff_t *offp);
static ssize_t ms_write(struct file *filp, const char *buff,
                        size_t count, loff_t *offp);
static loff_t ms_llseek(struct file *filp,  loff_t off, int whence);
static unsigned int ms_poll(struct file *filp, poll_table *wait);

static void kbm_interrupt_handler(unsigned long data);
static void v4_keyboard_interrupt_handler(unsigned long data);
static void v4_mouse_interrupt_handler(unsigned long data);

static int fpga_kbm_procfile_read(char* page, char** start, off_t off,
                                  int count, int* eof, void* data) ;

#define KEYBOARD_OFFSET         0x4000
#define MOUSE_OFFSET            0x8000

static inline int spaceavail(struct kbm_device *dev)
{
    if (dev->kbm_rp == dev->kbm_wp)
        return KBM_DATA_SIZE;
    return ((dev->kbm_rp + (KBM_DATA_SIZE + 1) - dev->kbm_wp) % (KBM_DATA_SIZE + 1)) - 1;
}

static inline void putabyte(struct kbm_device *dev, char data)
{
    if (dev->kbm_wp < &dev->kbm_buffer[KBM_DATA_SIZE]) {
        *dev->kbm_wp = data;
        dev->kbm_wp++;
    }
    else {
        dev->kbm_buffer[KBM_DATA_SIZE] = data;
        dev->kbm_wp = &dev->kbm_buffer[0];
    }
    dev->kbm_bytecount = KBM_DATA_SIZE - spaceavail(dev);
}

static inline char getabyte(struct kbm_device *dev)
{
    char kbmdata;

    if (dev->kbm_rp < &dev->kbm_buffer[KBM_DATA_SIZE]) {
        kbmdata = *dev->kbm_rp;
        dev->kbm_rp++;
    }
    else {
        kbmdata = dev->kbm_buffer[KBM_DATA_SIZE];
        dev->kbm_rp = &dev->kbm_buffer[0];
    }
    dev->kbm_bytecount = KBM_DATA_SIZE - spaceavail(dev);
    return kbmdata;
}

/************************
 * Module load function *
 ************************/
int kbm_control_init(struct v2d_board *board) {
    struct kbm_device *pKeyboard=NULL;
    struct kbm_device *pMouse=NULL;
    struct proc_dir_entry *pdr;

    struct file_operations *fops=NULL;
    struct devlist *v2dcdev;

    pKeyboard = kzalloc(sizeof(struct kbm_device), GFP_KERNEL);
    pMouse = kzalloc(sizeof(struct kbm_device), GFP_KERNEL);

    board->v_kbdev = (void *)pKeyboard;
    board->v_msdev = (void *)pMouse;

    /**************************
     * Set up File Operations *
     **************************/
    v2dcdev = board->v_devlist + KEYBOARD_DEV_MINOR;
    fops = &v2dcdev->d_fops;
    fops->read = kb_read;
    fops->write = kb_write;
    fops->llseek = kb_llseek;
    fops->poll = kb_poll;

    v2dcdev = board->v_devlist + MOUSE_DEV_MINOR;
    fops = &v2dcdev->d_fops;
    fops->read = ms_read;
    fops->write = ms_write;
    fops->llseek = ms_llseek;
    fops->poll = ms_poll;

    pKeyboard->kbm_rp = pKeyboard->kbm_wp = &pKeyboard->kbm_buffer[0];
    pKeyboard->kbm_type = KB_TYPE_PC;
    pMouse->kbm_rp = pMouse->kbm_wp = &pMouse->kbm_buffer[0];
    pMouse->kbm_type = MS_TYPE_IMPS2;

    init_waitqueue_head(&(pKeyboard->kbm_inq));
    init_waitqueue_head(&(pKeyboard->kbm_outq));
    init_waitqueue_head(&(pMouse->kbm_inq));
    init_waitqueue_head(&(pMouse->kbm_outq));

    spin_lock_init(&pKeyboard->kbm_lock);
    spin_lock_init(&pMouse->kbm_lock);

    pKeyboard->kbm_bad_interrupts = 0;
    pKeyboard->kbm_errors = 0;

    pMouse->kbm_mouse_pktsize = MOUSE_IMPS2_PKTSIZE;
    pKeyboard->kbm_mouse_pktsize = MOUSE_IMPS2_PKTSIZE;

    pdr = create_proc_read_entry("kbm_stats", 0, board->v_pdr,
                                 fpga_kbm_procfile_read, board);
    if (pdr == NULL)
        return ERROR;


    return 0;
}

int kbm_control_init2(struct v2d_board *board) {

    /***********************************
     * Set up keyboard/mouse interrupt *
     ***********************************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        fpga_register_disabled_irq(board, INT_7_KBM,
                                    kbm_interrupt_handler);
        fpga_interrupt_enable(board, INT_7_KBM);
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        fpga_register_disabled_irq(board, INT_12_KEYBOARD,
                                    v4_keyboard_interrupt_handler);
        fpga_register_disabled_irq(board, INT_13_MOUSE,
                                    v4_mouse_interrupt_handler);
        fpga_interrupt_enable(board, INT_12_KEYBOARD);
        fpga_interrupt_enable(board, INT_13_MOUSE);
    }

    return 0;
}

/********************
 * KBM exit routine *
 ********************/
void kbm_control_exit(struct v2d_board *board) {
    struct kbm_device *pKeyboard, *pMouse;

    pKeyboard = (struct kbm_device*) board->v_kbdev;
    pMouse = (struct kbm_device*) board->v_msdev;

    /*************************
     * Unregister interrupts *
     *************************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        fpga_unregister_irq(board, INT_7_KBM);
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        fpga_unregister_irq(board, INT_12_KEYBOARD);
        fpga_unregister_irq(board, INT_13_MOUSE);
    }

    /**********************************
     * Unregister the proc file entry *
     **********************************/
    remove_proc_entry("kbm_stats", board->v_pdr);

    /**********************************
     * Free the kb and ms devs        *
     **********************************/
    kfree(pKeyboard);
    kfree(pMouse);

    return ;
}

/********************************
 * V2DI50 KBM interrupt handler *
 ********************************/
static void  kbm_interrupt_handler(unsigned long data) {
    struct v2d_board *board;
    unsigned short asreg;
    static unsigned long dataval;
    struct kbm_device *dev;

    board = (struct v2d_board *) data;

/*  again:     */
    asreg = get_fpga_val(board, FPGA_ASREG);

    if (asreg & ASREG_KBM_KEYBOARD_DATA) {
        memcpy_fromio(&dataval,
                      (void *) board->v_DropMemBase + KEYBOARD_OFFSET, 4);

/*      LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,  */
/*               "KBM Interrupt -- KEYBOARD data %x\n", dataval); */

        dev = (struct kbm_device*)board->v_kbdev;
        if (dev->kbm_bytecount < (KBM_DATA_SIZE - 1)) {

            putabyte(dev, (dataval & 0xff00) >> 8);
            putabyte(dev, (dataval & 0xff));
            dev->kbm_rbytes+=2;
            wake_up_interruptible(&dev->kbm_inq);
        }
        else
            dev->kbm_rdropped++;
/*      goto again; */
    }
    else if (asreg & ASREG_KBM_MOUSE_DATA) {

        memcpy_fromio(&dataval,
                      (void *) board->v_DropMemBase + MOUSE_OFFSET, 4);

        dev = (struct kbm_device*)board->v_msdev;
        if (dev->kbm_bytecount < (KBM_DATA_SIZE - 1)) {

            dev->kbm_type = (((dataval & 0xff00) >> 8) == MS_ID_IMPS2) ?
                MS_TYPE_IMPS2 : MS_TYPE_PS2;
            dev->kbm_mouse_pktsize = (dev->kbm_type == MS_TYPE_IMPS2) ?
                MS_IMPS2_PKTSIZE : MS_PS2_PKTSIZE;
            putabyte(dev, dev->kbm_type);
            putabyte(dev, (dataval & 0xff));

            dev->kbm_rbytes += 2;
            dev->kbm_flushcount = 0;

            if (dev->kbm_bytecount >= dev->kbm_mouse_pktsize)
                wake_up_interruptible(&dev->kbm_inq);
        }
        else
            dev->kbm_rdropped++;

    }
    else {
        dev = (struct kbm_device*)board->v_kbdev;
        dev->kbm_bad_interrupts++;
    }

    fpga_interrupt_enable(board,INT_7_KBM);

    return ;
}

/***********************************
 * XP200/220 KBM interrupt handler *
 ***********************************/
static void  v4_keyboard_interrupt_handler(unsigned long data) {
    struct v2d_board *board;
    static unsigned long dataval;
    struct kbm_device *dev;

    board = (struct v2d_board *) data;

    dev = (struct kbm_device*)board->v_kbdev;

    if (get_fpga_val(board, FPGA_DSREG) & DSREG_XP_KEYBOARD_DATA) {
        dataval = get_fpga_val(board, FPGA_KBDIN_DATA);

        if (dev->kbm_bytecount < (KBM_DATA_SIZE - 1)) {
            putabyte(dev, (dataval & 0xff00) >> 8);
            putabyte(dev, (dataval & 0xff));
            dev->kbm_rbytes+=2;
            wake_up_interruptible(&dev->kbm_inq);
        }
        else
            dev->kbm_rdropped++;
    }
    else {
        dev->kbm_bad_interrupts++;
    }

    fpga_interrupt_enable(board, INT_12_KEYBOARD);
}

static void  v4_mouse_interrupt_handler(unsigned long data) {
    struct v2d_board *board;
    static unsigned long dataval;
    struct kbm_device *dev;

    board = (struct v2d_board *) data;

    dev = (struct kbm_device*)board->v_msdev;

    if (get_fpga_val(board, FPGA_DSREG) & DSREG_XP_MOUSE_DATA) {
        dataval = get_fpga_val(board, FPGA_MSEIN_DATA);

        if (dev->kbm_bytecount < (KBM_DATA_SIZE - 1)) {
            dev->kbm_type = (((dataval & 0xff00) >> 8) == MS_ID_IMPS2) ?
                MS_TYPE_IMPS2 : MS_TYPE_PS2;
            dev->kbm_mouse_pktsize = (dev->kbm_type == MS_TYPE_IMPS2) ?
                MS_IMPS2_PKTSIZE : MS_PS2_PKTSIZE;
            putabyte(dev, dev->kbm_type);
            putabyte(dev, (dataval & 0xff));
            dev->kbm_rbytes+=2;
            if (dev->kbm_bytecount >= dev->kbm_mouse_pktsize)
                wake_up_interruptible(&dev->kbm_inq);
        }
        else
            dev->kbm_rdropped++;
    }
    else {
        dev->kbm_bad_interrupts++;
    }

    fpga_interrupt_enable(board, INT_13_MOUSE);
}

static ssize_t kb_read(struct file *filp, char *buff,
                       size_t count, loff_t *offp) {

    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_kbdev;
    char kbmbyte;
    unsigned long lock_flags;

    if (!access_ok(VERIFY_WRITE, buff, count) || !(board->v_hardware_detected))
        return -EFAULT;

    /*******************************************************
     * If we dont have any data, block till data arrives.  *
     *******************************************************/
    while (dev->kbm_bytecount == 0) { /* nothing to read */

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        if (wait_event_interruptible(dev->kbm_inq,
                                     (dev->kbm_bytecount != 0)))
            return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
    }

    count = min(dev->kbm_bytecount, (int) count);

    spin_lock_irqsave(&dev->kbm_lock, lock_flags);
    while (dev->kbm_bytecount > 0) {
        kbmbyte = getabyte(dev);
        *buff++ = kbmbyte;
    }

    spin_unlock_irqrestore(&dev->kbm_lock, lock_flags);

    return count;
}

static ssize_t kb_write(struct file *filp, const char *buff,
                        size_t count, loff_t *offp) {

    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_kbdev;
    unsigned char kbmdata[KBM_DATA_SIZE];
    int i, waitcount, gotdata = 0;
    unsigned long dataval;

    if (count == 0 || !(board->v_hardware_detected))
        return 0;

    /*****************************************
     * Verify access on user supplied buffer *
     *****************************************/
    if (!access_ok(VERIFY_READ, buff, count))
        return -EFAULT;

    /**************************************
     * Dont write more than we can handle *
     **************************************/
    if (count > KBM_DATA_SIZE) {
        dev->kbm_wdropped += (count - KBM_DATA_SIZE);
        count = KBM_DATA_SIZE;
    }

    /***************************************
     * Fill up the buffer with user data.  *
     ***************************************/
    if (copy_from_user(kbmdata, buff, count)) {  /* no need to hold spin lock
                                                    during copy */
        return -EFAULT;
    }

    for (i = 0; i < count; i += 2) {
        dataval = 0;
        dataval |= kbmdata[i] << 8;
        dataval |= kbmdata[i+1];

        /****************
         * Write to PS2 *
         ****************/
        waitcount = 0;
        while (1) {
            if (board->v_pci_dev_type == V2DI50_DEVICE)
                gotdata = get_fpga_val(board, FPGA_DSREG) &
                                            DSREG_KBM_KEYBOARD_DATA;
            else if (board->v_pci_dev_type == PLX_DEVICE)
                gotdata = (get_fpga_val(board, FPGA_ASREG) &
                                        ASREG_XP_KEYBOARD_DATA) ? 0 : 1;
            if (!gotdata) break;

            udelay(1);
            dev->kbm_long++;
            if (waitcount++ > 10) {
                dev->kbm_toolong++;
                break;
            }
        }

        if (board->v_pci_dev_type == V2DI50_DEVICE)
            memcpy_toio((void *)board->v_DropMemBase + KEYBOARD_OFFSET,
                    &dataval, 4);
        else if (board->v_pci_dev_type == PLX_DEVICE)
            set_fpga_val(board, FPGA_KBDOUT_DATA, dataval);

        /*LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
         *  "Wrote KB Data %lx from location %d\n", dataval, i);*/

        dev->kbm_wbytes += count;

        if (board->v_pci_dev_type != PLX_DEVICE) continue;

        /****************
         * Write to USB *
         ****************/
        waitcount = 0;
        while (1) {
            gotdata = (get_fpga_val(board, FPGA_ASREG) &
                                        ASREG_XP_KEYBOARD_DATA_Y) ? 0 : 1;
            if (!gotdata) break;

            udelay(1);
            dev->kbm_long++;
            if (waitcount++ > 10) {
                dev->kbm_toolong++;
                break;
            }
        }

        set_fpga_val(board, FPGA_KBDOUT_DATA_Y, dataval);

        dev->kbm_usb_wbytes += count;
    }

    return count;
}

static loff_t kb_llseek(struct file *filp,  loff_t off, int whence)
{
    return -ESPIPE; /* unseekable */
}

static unsigned int kb_poll(struct file *filp, poll_table *wait)
{
    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_kbdev;
    unsigned int mask = 0;

    poll_wait(filp, &dev->kbm_inq,  wait);
    poll_wait(filp, &dev->kbm_outq, wait);

    if (dev->kbm_bytecount > 0)
        mask |= POLLIN | POLLRDNORM;    /* readable if there's data in buffer */

    mask |= POLLOUT | POLLWRNORM;       /* always writable */

    return mask;
}

static ssize_t ms_read(struct file *filp, char *buff,
                       size_t count, loff_t *offp) {

    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_msdev;
    char kbmbyte;
    unsigned long lock_flags;

    if (!access_ok(VERIFY_WRITE, buff, count) || !(board->v_hardware_detected))
        return -EFAULT;

    /*******************************************************
     * If we dont have any data, block till data arrives.  *
     *******************************************************/
    while (dev->kbm_bytecount == 0) { /* nothing to read */

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        if (wait_event_interruptible(dev->kbm_inq,
                                     (dev->kbm_bytecount != 0)))
            return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
    }

    count = min(dev->kbm_bytecount, (int) count);

    spin_lock_irqsave(&dev->kbm_lock, lock_flags);
    while (dev->kbm_bytecount > 0) {
        kbmbyte = getabyte(dev);
        *buff++ = kbmbyte;
    }
    spin_unlock_irqrestore(&dev->kbm_lock, lock_flags);

    return count;
}


static ssize_t ms_write(struct file *filp, const char *buff,
                        size_t count, loff_t *offp) {

    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_msdev;
    unsigned char kbmdata[KBM_DATA_SIZE];
    int i, waitcount, gotdata = 0;
    unsigned long dataval;

    if (count == 0 && !(board->v_hardware_detected))
        return 0;

    /*****************************************
     * Verify access on user supplied buffer *
     *****************************************/
    if (!access_ok(VERIFY_READ, buff, count))
        return -EFAULT;

    /**************************************
     * Dont write more than we can handle *
     **************************************/
    if (count > KBM_DATA_SIZE) {
        dev->kbm_wdropped += (count - KBM_DATA_SIZE);
        count = KBM_DATA_SIZE;
    }

    /***************************************
     * Fill up the buffer with user data.  *
     ***************************************/
    if (copy_from_user(kbmdata, buff, count)) {  /* no need to hold spin lock
                                                    during copy */
        return -EFAULT;
    }

    for (i = 0; i < count; i+= 2) {
        dataval = 0;
        dataval |= kbmdata[i] << 8;
        dataval |= kbmdata[i+1];

        /****************
         * Write to PS2 *
         ****************/
        waitcount = 0;
        while (1) {
            if (board->v_pci_dev_type == V2DI50_DEVICE)
                gotdata = get_fpga_val(board, FPGA_DSREG) & DSREG_KBM_MOUSE_DATA;
            else if (board->v_pci_dev_type == PLX_DEVICE)
                gotdata = (get_fpga_val(board, FPGA_ASREG) &
                                            ASREG_XP_MOUSE_DATA) ? 0 : 1;
            if (!gotdata) break;

            udelay(1);
            dev->kbm_long++;
            if (waitcount++ > 10) {
                dev->kbm_toolong++;
                break;
            }
        }
        if (board->v_pci_dev_type == V2DI50_DEVICE)
            memcpy_toio((void *)board->v_DropMemBase + MOUSE_OFFSET,
                    &dataval, 4);
        else if (board->v_pci_dev_type == PLX_DEVICE)
            set_fpga_val(board, FPGA_MSEOUT_DATA, dataval);

        /*LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
         *  "Wrote MS Data %lx from location %d\n", dataval, i);*/

        dev->kbm_wbytes += count;

        if (board->v_pci_dev_type != PLX_DEVICE) continue;

        /****************
         * Write to USB *
         ****************/
        waitcount = 0;
        while (1) {
            gotdata = (get_fpga_val(board, FPGA_ASREG) &
                                        ASREG_XP_MOUSE_DATA_Y) ? 0 : 1;
            if (!gotdata) break;

            udelay(1);
            dev->kbm_long++;
            if (waitcount++ > 10) {
                dev->kbm_toolong++;
                break;
            }
        }

        set_fpga_val(board, FPGA_MSEOUT_DATA_Y, dataval);

        dev->kbm_usb_wbytes += count;
    }

    return count;
}

static loff_t ms_llseek(struct file *filp,  loff_t off, int whence)
{
    return -ESPIPE; /* unseekable */
}

static unsigned int ms_poll(struct file *filp, poll_table *wait)
{
    struct v2d_board *board = (struct v2d_board*) filp->private_data;
    struct kbm_device *dev = board->v_msdev;
    unsigned int mask = 0;

    poll_wait(filp, &dev->kbm_inq,  wait);
    poll_wait(filp, &dev->kbm_outq, wait);

    if (dev->kbm_bytecount >= dev->kbm_mouse_pktsize) {
        mask |= POLLIN | POLLRDNORM;  /* readable if there's data in buffer */
        if (dev->kbm_bytecount > dev->kbm_mouse_pktsize)
            dev->kbm_long++;
    }
    else if (dev->kbm_flushcount == -1) {
        mask |= POLLIN | POLLRDNORM;  /* readable if force flushed the buffer */
        dev->kbm_toolong++;
        /***************************************************************
         * Fix for Bugzilla bug 122 - reset flush count so we only set *
         * mask readable when flush_mouse_inqueue was called again.    *
         ***************************************************************/
        dev->kbm_flushcount = 0;
    }

    mask |= POLLOUT | POLLWRNORM; /* always writable */

    return mask;

}

void flush_kbm_queues(struct v2d_board *board) {

    struct kbm_device *pKeyboard, *pMouse;
    unsigned long lock_flags;

    pKeyboard = (struct kbm_device*) board->v_kbdev;
    pMouse = (struct kbm_device*) board->v_msdev;

    spin_lock_irqsave(&pKeyboard->kbm_lock, lock_flags);
    pKeyboard->kbm_rp = pKeyboard->kbm_wp = &pKeyboard->kbm_buffer[0];
    pKeyboard->kbm_bytecount = 0;
    spin_unlock_irqrestore(&pKeyboard->kbm_lock, lock_flags);

    spin_lock_irqsave(&pMouse->kbm_lock, lock_flags);
    pMouse->kbm_rp = pMouse->kbm_wp = &pMouse->kbm_buffer[0];
    pMouse->kbm_bytecount = 0;
    spin_unlock_irqrestore(&pMouse->kbm_lock, lock_flags);
}

void flush_mouse_inqueue(struct v2d_board *board) {
    struct kbm_device *pKeyboard, *pMouse;

    pKeyboard = (struct kbm_device*) board->v_kbdev;
    pMouse = (struct kbm_device*) board->v_msdev;

    if (++pMouse->kbm_flushcount == 2) {
        if (pMouse->kbm_bytecount > 0) {
            wake_up_interruptible(&pMouse->kbm_inq);
            pMouse->kbm_flushcount = -1;
            pMouse->kbm_flush_mouse++;
        }
    }
}

int kbm_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    struct kbm_device *pKeyboard, *pMouse;

    pKeyboard = (struct kbm_device*) board->v_kbdev;
    pMouse = (struct kbm_device*) board->v_msdev;

    switch (cmd) {
    case FPGA_IOC_KBM_PARAM_GET:
    {
        struct kbm_param_io kbmpio;
        struct kbm_param_io *kbmpiop = (struct kbm_param_io *) data;

        if ((kbmpiop == NULL) ||
            (copy_from_user(&kbmpio, kbmpiop, sizeof(struct kbm_param_io))))
            return -EFAULT;

        kbmpio.kb_rbytes = pKeyboard->kbm_rbytes;
        kbmpio.kb_rdropped = pKeyboard->kbm_rdropped;
        kbmpio.ms_rbytes = pMouse->kbm_rbytes;
        kbmpio.ms_rdropped = pMouse->kbm_rdropped;
        kbmpio.kb_wbytes = pKeyboard->kbm_wbytes;
        kbmpio.kb_usb_wbytes = pKeyboard->kbm_usb_wbytes;
        kbmpio.kb_wdropped = pKeyboard->kbm_wdropped;
        kbmpio.ms_wbytes = pMouse->kbm_wbytes;
        kbmpio.ms_usb_wbytes = pMouse->kbm_usb_wbytes;
        kbmpio.ms_wdropped = pMouse->kbm_wdropped;

        if (copy_to_user((struct kbm_param_io *)data,
                         &kbmpio, sizeof(struct kbm_param_io)) != 0)
            return -EFAULT;
        return 0;
    }

    case FPGA_IOC_KBM_PARAM_SET:
    {
        pKeyboard->kbm_rbytes = 0;
        pKeyboard->kbm_wbytes = 0;
        pKeyboard->kbm_usb_wbytes = 0;
        pKeyboard->kbm_rdropped = 0;
        pKeyboard->kbm_wdropped = 0;

        pMouse->kbm_rbytes = 0;
        pMouse->kbm_wbytes = 0;
        pMouse->kbm_usb_wbytes = 0;
        pMouse->kbm_rdropped = 0;
        pMouse->kbm_wdropped = 0;

        return 0;
    }

    case FPGA_IOC_KBM_FLUSH:
    {
        flush_kbm_queues(board);

        return 0;
    }

    case FPGA_IOC_KB_TYPE_GET:
    {
        if ((int *)data == NULL || board->v_type != HARDWARE_V2D_RX)
            return -EFAULT;
        if (copy_to_user((int *)data, &(pKeyboard->kbm_type), sizeof(int)) != 0)
            return -EFAULT;

        return 0;
    }

    case FPGA_IOC_KB_TYPE_SET:
    {
        int kbtype;
        unsigned long mkcreg;

        if ((int *)data == NULL || board->v_type != HARDWARE_V2D_RX)
            return -EFAULT;
        if (copy_from_user(&kbtype, (int *)data, sizeof(int)) != 0)
            return -EFAULT;

        pKeyboard->kbm_type = (kbtype == KB_TYPE_SGI) ? KB_TYPE_SGI : KB_TYPE_PC;
        if (board->v_pci_dev_type == V2DI50_DEVICE)
            mkcreg = get_fpga_val(board, FPGA_MKCREG) &
                        ~(MKCREG_SCANCODE_MASK | MKCREG_KEYBOARD_ENABLE |
                          MKCREG_MOUSE_ENABLE);
        else if (board->v_pci_dev_type == PLX_DEVICE)
            mkcreg = get_fpga_val(board, FPGA_MKCREG) &
                        ~(MKCREG_XP_SCANCODE_MASK | MKCREG_XP_KBDOUT_ENABLE |
                          MKCREG_XP_KBDIN_ENABLE | MKCREG_XP_MSEOUT_ENABLE |
                          MKCREG_XP_MSEIN_ENABLE);
        else
            return -EFAULT;

        set_fpga_val(board, FPGA_MKCREG, mkcreg);

        mdelay(1);

        if (board->v_pci_dev_type == V2DI50_DEVICE) {
            mkcreg |= (pKeyboard->kbm_type << MKCREG_SCANCODE_SHIFT);
            mkcreg |= (MKCREG_KEYBOARD_ENABLE|MKCREG_MOUSE_ENABLE);
        }
        else {
            mkcreg |= MKCREG_XP_KBDIN_ENABLE|MKCREG_XP_KBDOUT_ENABLE;
            if (pKeyboard->kbm_type == KB_TYPE_PC)
                mkcreg |= MKCREG_XP_SCANCODE_MASK;
        }

        set_fpga_val(board, FPGA_MKCREG, mkcreg);

        return 0;
    }

    case FPGA_IOC_MS_TYPE_GET:
    {
        if ((int *)data == NULL || board->v_type != HARDWARE_V2D_RX)
            return -EFAULT;
        if (copy_to_user((int *)data, &(pMouse->kbm_type), sizeof(int)) != 0)
            return -EFAULT;

        return 0;
    }

    }

    return -1;  /* Unsupported command */
}

static int fpga_kbm_procfile_read(char* page, char** start, off_t off,
                                  int count, int* eof, void* data)
{
    int len = 0;
    struct v2d_board *board= (struct v2d_board*) data;
    struct kbm_device *pKeyboard, *pMouse;

    pKeyboard = (struct kbm_device*) board->v_kbdev;
    pMouse = (struct kbm_device*) board->v_msdev;

    /***********************************************************
     * Since we provide all the data at one shot, any non zero *
     * offset means we dont have anything more to say          *
     ***********************************************************/
    if (off > 0) {
        *eof = -1; /* EOF ?? */
        return 0;
    }

    len += sprintf(page + len, "*** Keyboard Mouse Stats ***\n");
    len += sprintf(page + len, "%-24s = %d\t", "Keyboard Type",
                   pKeyboard->kbm_type);
    len += sprintf(page + len, "%-24s = %d\n", "Mouse Type",
                   pMouse->kbm_type);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard Errors",
                   pKeyboard->kbm_errors);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Errors",
                   pMouse->kbm_errors);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard Read Bytes",
                   pKeyboard->kbm_rbytes);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Read Bytes",
                   pMouse->kbm_rbytes);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard Read Dropped",
                   pKeyboard->kbm_rdropped);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Read Dropped",
                   pMouse->kbm_rdropped);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard PS2 Write Bytes",
                   pKeyboard->kbm_wbytes);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse PS2 Write Bytes",
                   pMouse->kbm_wbytes);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard USB Write Bytes",
                   pKeyboard->kbm_usb_wbytes);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse USB Write Bytes",
                   pMouse->kbm_usb_wbytes);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard Write Dropped",
                   pKeyboard->kbm_wdropped);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Write Dropped",
                   pMouse->kbm_wdropped);
    len += sprintf(page + len, "%-24s = %lu\t", "Bad Keyboard Interrupts",
                   pKeyboard->kbm_bad_interrupts);
    len += sprintf(page + len, "%-24s = %lu\n", "Bad Mouse Interrupts",
                   pMouse->kbm_bad_interrupts);
    len += sprintf(page + len, "%-24s = %lu\t", "Keyboard Overwrites",
                   pKeyboard->kbm_toolong);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Overwrites",
                   pMouse->kbm_toolong);
    len += sprintf(page + len, "%-24s = %lu\t", "Possible KB Overwrite",
                   pKeyboard->kbm_long);
    len += sprintf(page + len, "%-24s = %lu\n", "Possible MS Overwrite",
                   pMouse->kbm_long);
    len += sprintf(page + len, "%-24s = %lu\n", "Mouse Flush",
                   pMouse->kbm_flush_mouse);


    return len;
}
