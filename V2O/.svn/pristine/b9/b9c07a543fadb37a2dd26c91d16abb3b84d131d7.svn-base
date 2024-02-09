
#include "fpga.h"
#include "lut_table.h"
#include "fpga_ioctl.h"
#include "legacyv2d.h"
#include "v4_fpga.h"
#include "log_control.h"

MODULE_AUTHOR("Shanker Iyer, IP Video Systems.  siyer@ipvideosys.com");
MODULE_AUTHOR("Rahul P. Kale, IP Video Systems.  rkale@ipvideosys.com");
MODULE_AUTHOR("Pearl Wang, IP Video Systems.  pwang@ipvideosys.com");
MODULE_DESCRIPTION("FPGA module for the V2D project");
MODULE_SUPPORTED_DEVICE("V2D FPGA");
MODULE_LICENSE("GPL");

/* #define DEBUG_DETAIL */
#define DEBUG_STATS

struct proc_dir_entry *modproc;

static int virtualboards[MAX_FPGA_BOARDS];
static int nVirtualBoards;
static struct v2d_board *pVirtualBoards[MAX_FPGA_BOARDS];



module_param_array(virtualboards, int , &nVirtualBoards, 0000);
MODULE_PARM_DESC(virtualboards, "The list of boards");

static int nMaxBoards;
static int boardpcislots[MAX_FPGA_BOARDS];
module_param_array(boardpcislots, int , &nMaxBoards, 0000);
MODULE_PARM_DESC(boardpcislots, "Mapping of board to PCI slots");


static int create_virtual_devices(void);
static int destroy_virtual_devices(void);


static int fpga_open(struct inode *inode, struct file *filp);
static long fpga_ioctl(struct file *filp,
                      unsigned int cmd, unsigned long data);
static int fpga_release(struct inode *inode, struct file *filp);

/*************************************************************
 * Default File OPs for our device. Other operations defined *
 * dynamically (see flow_control.c)                          *
 *************************************************************/
struct file_operations fpga_fops = {
    open:                fpga_open,
    release:             fpga_release,
    unlocked_ioctl:      fpga_ioctl,
};

static int vpci_probe (struct pci_dev *pdev,
                       const struct pci_device_id *ent);
static void vpci_remove(struct pci_dev *pdev);


static char *devnames[] = {"video", "keyboard", "mouse", "audio", "i2c"};
struct class *v2d_class;

static int boards=0;


static struct pci_device_id vpci_pci_tbl[]  = {
    { V2DI50_VENDOR, V2DI50_DEVICE , PCI_ANY_ID, PCI_ANY_ID },
    { PLX_VENDOR, PLX_DEVICE, PCI_ANY_ID, PCI_ANY_ID },
    {0,}
};


static struct pci_driver vpci_driver = {
    name:               "fpga_pci",
    id_table:           vpci_pci_tbl,
    probe:              vpci_probe,
    remove:             vpci_remove,
};

static int v2d_setup_cdev (struct v2d_board *board)
{
    struct devlist *v2dcdev;
    struct cdev *cdev;
    struct file_operations *fops;

    dev_t dev ;
    int i=0, minor=0;
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Setup Character Devices %d\n", board->v_id);
    sprintf (board->v_name, "%d", board->v_id);

    board->v_pdr = create_proc_entry (board->v_name, S_IFDIR, modproc);
    if (board->v_pdr == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
            "Could not create %s %s\n", V2D_DRIVER_NAME, board->v_name);
    }
    minor = board->v_id*V2D_CHAR_DEVICES;
    for (i = 0; i < V2D_CHAR_DEVICES; i++) {
        dev = MKDEV(FPGA_DEV_MAJOR, minor+ i);
        v2dcdev = board->v_devlist + i;
        fops = &v2dcdev->d_fops;
        fops->open = fpga_open;
        fops->release = fpga_release;
        fops->unlocked_ioctl = fpga_ioctl;
        cdev = cdev_alloc();
        cdev->ops = fops;

        v2dcdev->d_pcdev = cdev;
        sprintf (v2dcdev->d_name,
                 "%s%d_%s", V2D_DRIVER_NAME, board->v_id, devnames[i]);
        if (cdev_add(cdev, dev, 1) < 0) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
                     "Could not create char device %s for file operations.\n",
                     v2dcdev->d_name);
            return -1;
        }
        else {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                     "Created char device %s for file operations %px .\n",
                     v2dcdev->d_name, cdev);
        }
        device_create( v2d_class, NULL , dev , "%s", v2dcdev->d_name);
    }
    return 0;
}
static int v2d_teardown_cdev (struct v2d_board *board)
{
    int i=0;
    dev_t dev;
    int minor = board->v_id * V2D_CHAR_DEVICES;
    for (i = 0; i < V2D_CHAR_DEVICES; i++) {
        dev = MKDEV(FPGA_DEV_MAJOR, minor+ i);
        cdev_del(board->v_devlist[i].d_pcdev);
        device_destroy(v2d_class,dev);
    }
    remove_proc_entry(board->v_name, modproc);
    return 0;
}



/************************
 * Module load function *
 ************************/
static int __init fpga_module_init(void)
{
    dev_t dev = MKDEV(FPGA_DEV_MAJOR, 0);
    int i=0;
    boards=0;

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Loading Driver...\n");

    modproc = proc_mkdir(V2D_DRIVER_NAME, NULL);

    v2d_class = class_create (THIS_MODULE, "v2d");
    register_chrdev_region(dev, V2D_CHAR_DEVICES*MAX_FPGA_BOARDS,
                           V2D_DRIVER_NAME);

    if (nVirtualBoards > 0) {
        create_virtual_devices();
    }
    if (nMaxBoards > 0) {
        for (i=0; i < nMaxBoards; i++) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                    "Board %d slot %d\n", i, boardpcislots[i]);
        }
    }
    else {
        // This is the default for the XP200
        nMaxBoards=2;
        boardpcislots[0] = 13;
        boardpcislots[1] = 12;
    }


    /*************************
     * Initialize PCI driver *
     *************************/
    if (pci_register_driver(&vpci_driver) != 0) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
                 "V2D FPGA not detected on PCI Bus!!\n");
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Loading driver anyway with most features disabled.\n");
    }
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Driver Loaded.\n");

    return 0;
}

/**************************
 * Module unload function *
 **************************/
static void __exit fpga_module_exit(void)
{
    dev_t dev = MKDEV(FPGA_DEV_MAJOR, 0);

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Unloading Driver...\n");
    pci_unregister_driver(&vpci_driver);
    destroy_virtual_devices();

    unregister_chrdev_region(dev, V2D_CHAR_DEVICES*MAX_FPGA_BOARDS);
    class_destroy(v2d_class);
    remove_proc_entry (V2D_DRIVER_NAME, NULL);
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Driver UnLoaded.\n");
}

/************************************************************
 * Register the module load/unload fucntion with the kernel *
 ************************************************************/
module_init(fpga_module_init);
module_exit(fpga_module_exit);

struct v2d_board *get_board_info(struct inode *inode) {
    struct pci_dev *pdev=NULL;
    struct v2d_board *board=NULL;
    int vid=0;
    int i=0;

    vid = MINOR(inode->i_rdev)/V2D_CHAR_DEVICES;
    /* Now search for Legacy V2D  devices */
    while ((pdev =
            pci_get_device(V2DI50_VENDOR, V2DI50_DEVICE, pdev)) != NULL) {
        board = pci_get_drvdata(pdev);
        if (board == NULL) continue;
        if (board->v_id == vid) break;
        break;
    }
    if (board != NULL) return board;

    /* Now search for XP200 devices */
    while ((pdev =
            pci_get_device(PLX_VENDOR, PLX_DEVICE, pdev)) != NULL) {
        board = pci_get_drvdata(pdev);
        if (board->v_id == vid) break;
    }
    /* Now search for Pseudo devices */
    for (i=0; i < nVirtualBoards; i++) {
        if (pVirtualBoards[i] != NULL) {
            board = pVirtualBoards[i];
        }
    }
    return board;
}



static long fpga_ioctl(struct file *filp,
                      unsigned int cmd, unsigned long data) {
    struct v2d_board *board = (struct v2d_board *)filp->private_data;
    int ival=0;
    switch (cmd) {
        case FPGA_IOC_RESET_LUT:
        case FPGA_IOC_SET_LUT:
        case FPGA_IOC_GET_LUT:
            return lut_table_ioctl(board,cmd, data);

        case FPGA_IOC_MOD_REG:
        case FPGA_IOC_GET_HDELAY:
        case FPGA_IOC_SET_HDELAY:
        case FPGA_IOC_GET_VDELAY:
        case FPGA_IOC_SET_VDELAY:
        case FPGA_IOC_GET_PSHIFT:
        case FPGA_IOC_SET_PSHIFT:
        case FPGA_IOC_GET_PIXADJUST:
        case FPGA_IOC_SET_PIXADJUST:
        case FPGA_IOC_RESET_ADDSIDE:
        case FPGA_IOC_RESET_DROPSIDE:
        case FPGA_IOC_GET_ADDSIDELUT:
        case FPGA_IOC_SET_ADDSIDELUT:
        case FPGA_IOC_GET_DROPSIDELUT:
        case FPGA_IOC_SET_DROPSIDELUT:
        case FPGA_IOC_GET_DISPLAYLUT:
        case FPGA_IOC_SET_DISPLAYLUT:
        case FPGA_IOC_GET_VID_AVL:
        case FPGA_IOC_SET_VID_AVL:
        case FPGA_IOC_GET_AMQUANT:
        case FPGA_IOC_SET_AMQUANT:
        case FPGA_IOC_GET_ADCQUANT:
        case FPGA_IOC_SET_ADCQUANT:
        case FPGA_IOC_SET_ASLICEDROP:
        case FPGA_IOC_GET_ANOBPSLICE:
        case FPGA_IOC_SET_ANOBPSLICE:
        case FPGA_IOC_GET_DNOBPSLICE:
        case FPGA_IOC_SET_DNOBPSLICE:
        case FPGA_IOC_GET_ACNOISET:
        case FPGA_IOC_SET_ACNOISET:
        case FPGA_IOC_GET_DCNOISET:
        case FPGA_IOC_SET_DCNOISET:
        case FPGA_IOC_GET_ACOMPMODE:
        case FPGA_IOC_SET_ACOMPMODE:
        case FPGA_IOC_GET_DCOMPMODE:
        case FPGA_IOC_SET_DCOMPMODE:
        case FPGA_IOC_SET_VIDEOCHECK:
        case FPGA_IOC_SET_EXTSYNC:
        case FPGA_IOC_SET_ADD_DRIVERIDLE:
        case FPGA_IOC_SET_DEFAULT_CPMP:
        case FPGA_IOC_GET_DEFAULT_CPMP:
        case FPGA_IOC_GET_INTERFACE:
        case FPGA_IOC_SET_INTERFACE:
        case FPGA_IOC_FORCE_INTERFACE:
        case FPGA_IOC_GET_DDC_EDID:
        case FPGA_IOC_SET_NOISETHRESMODE:
        case FPGA_IOC_RESET_FPGA:
        case FPGA_IOC_GET_OV_PARAM:
        case FPGA_IOC_SET_OV_PARAM:
        case FPGA_IOC_GET_OV_INFO:
        case FPGA_IOC_FRAMECNT_ENABLE:
        case FPGA_IOC_GET_AVFLAG:
        case FPGA_IOC_SET_AVFLAG:
        case FPGA_IOC_GET_DISPLAY_HPOL:
        case FPGA_IOC_SET_DISPLAY_HPOL:
        case FPGA_IOC_GET_DISPLAY_VPOL:
        case FPGA_IOC_SET_DISPLAY_VPOL:
        case FPGA_IOC_GET_CODEC_VERSION:
            return fpga_control_ioctl(board,cmd, data);

        case FPGA_IOC_GET_INTR_STAT:
        case FPGA_IOC_GET_INTR_RST:
        case FPGA_IOC_INTR_FIRE:
            return fpga_interrupt_ioctl(board,cmd, data);

        case FPGA_IOC_MOD_PLX:
        case FPGA_IOC_GET_PLXCAP:
        case FPGA_IOC_RESET_PLX:
        case FPGA_IOC_PLX_TEST:
            return plx_control_ioctl(board,cmd, data);

        case FPGA_IOC_SET_LED:
        case FPGA_IOC_SET_LEDSTATE:
        case FPGA_IOC_SET_LEDALIVE:
            return led_control_ioctl(board,cmd, data);

        case FPGA_IOC_SET_LOGLVL:
            return set_current_log_level(data);
            break;
        case FPGA_IOC_FC_PARAM_GET:
        case FPGA_IOC_FC_WPARAM_GET:
        case FPGA_IOC_FC_RSTAT_GET:
        case FPGA_IOC_FC_WSTAT_GET:
        case FPGA_IOC_FC_FLUSH:
        case FPGA_IOC_FC_QUEUE_SET:
        case FPGA_IOC_FC_QUEUE_GET:
        case FPGA_IOC_PC_PARAM_GET:
        case FPGA_IOC_PC_PARAM_SET:
        case FPGA_IOC_GET_LATENCY:
        case FPGA_IOC_SET_FPGA_DROP:
        case FPGA_IOC_QAR_PARAM_GET:
        case FPGA_IOC_QAR_PARAM_SET:
        case FPGA_IOC_PROGCMPRS_ENABLE:
            return flow_control_ioctl(board,cmd, data);
            break;

        case FPGA_IOC_KBM_PARAM_GET:
        case FPGA_IOC_KBM_PARAM_SET:
        case FPGA_IOC_KBM_FLUSH:
        case FPGA_IOC_KB_TYPE_GET:
        case FPGA_IOC_KB_TYPE_SET:
        case FPGA_IOC_MS_TYPE_GET:
            return kbm_control_ioctl(board,cmd, data);
            break;

        case FPGA_IOC_BOARD_STATUS:
            if (copy_to_user((unsigned char *)data,
                         &board->v_status, sizeof(int)) != 0)
                return -EFAULT;

        case FPGA_IOC_BOARD_REVISION:
            return 0;

        case FPGA_IOC_BOARD_INIT:
            switch (board->v_pci_dev_type) {
                case PLX_DEVICE:
                    v4_fpga_board_init(board);
                    break;
                default:
                    return -EINVAL;
                    break;
            }
            break;

        case FPGA_IOC_BOARD_EXIT:
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                     "%s. calling fpga_board_exit for bno %p\n",
                     __FUNCTION__, board);
            switch (board->v_pci_dev_type) {
                case PLX_DEVICE:
                    v4_fpga_board_exit(board);
                    break;
                default:
                    return -EINVAL;
                    break;
            }
            break;

        case FPGA_IOC_SET_DRIVERENABLE:
            if (copy_from_user(&ival, (int *)data, sizeof(int)) != 0)
                return -EFAULT;
            if (ival == 0)
                board->v_status |= FPGA_BOARD_IDLE;
            else
                board->v_status &= ~FPGA_BOARD_IDLE;
            break;

        case FPGA_IOC_GET_HWCOUNT:
            if (!copy_to_user((unsigned char *)data, &boards, sizeof(int)))
                return -EFAULT;
            break;

        case FPGA_IOC_LOCK_ADDSIDELUT:
            {
                unsigned long lock_flags;
                if (copy_from_user(&ival, (int *) data, sizeof(int)) != 0)
                    return -EFAULT;
                spin_lock_irqsave(&board->v_lut_lock, lock_flags);
                if (ival == 0)
                    clear_bit(FPGA_BOARD_ADDSIDELUT_BUSY_BIT, &board->v_status);
                else
                    set_bit(FPGA_BOARD_ADDSIDELUT_BUSY_BIT, &board->v_status);
                spin_unlock_irqrestore(&board->v_lut_lock, lock_flags);
            }
            break;

        case FPGA_IOC_LOCK_DROPSIDELUT:
            if (copy_from_user(&ival, (int *) data, sizeof(int)) != 0)
                return -EFAULT;
            if (ival == 0)
                test_and_clear_bit(FPGA_BOARD_DROPSIDELUT_BUSY_BIT,
                        &board->v_status);
            else {
                if (test_and_set_bit(FPGA_BOARD_DROPSIDELUT_BUSY_BIT,
                        &board->v_status) != 0)
                    return -EFAULT;
            }
            break;

        case FPGA_IOC_LOCK_DISPLAYLUT:
            if (copy_from_user(&ival, (int *) data, sizeof(int)) != 0)
                return -EFAULT;
            if (ival == 0)
                test_and_clear_bit(FPGA_BOARD_DISPLAYLUT_BUSY_BIT,
                        &board->v_status);
            else {
                if (test_and_set_bit(FPGA_BOARD_DISPLAYLUT_BUSY_BIT,
                        &board->v_status) != 0)
                    return -EFAULT;
            }
            break;

        case FPGA_IOC_LOCK_RDPIXEL:
            if (copy_from_user(&ival, (int *) data, sizeof(int)) != 0)
                return -EFAULT;
            if (ival == 0)
                test_and_clear_bit(FPGA_BOARD_RDPIXEL_BUSY_BIT, &board->v_status);
            else {
                if (test_and_set_bit(FPGA_BOARD_RDPIXEL_BUSY_BIT,
                            &board->v_status) != 0)
                    return -EFAULT;
            }
            break;

        case FPGA_IOC_GET_HARDWARE_INFO:
        {
            struct hardware_info hw_info;
            if ((struct hardware_info *)data == NULL)
                return -EFAULT;
            hw_info.hardware_type = board->v_pci_dev_type;
            hw_info.board_type = board->v_type;
            hw_info.hardware_version = board->v_hardware_version;
            hw_info.hardware_detected = board->v_hardware_detected;
            hw_info.number_of_boards = boards;
            if (copy_to_user((struct hardware_info *)data,
                         &hw_info, sizeof(struct hardware_info)) != 0)
                return -EFAULT;
        }
        break;

    case FPGA_IOC_RESET:
        /* Cause a panic */
        printk("Forcing kernel panic...\n");
        panic("Oh no!!");
        break;
    default:
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_WARNING,
                 "Unkown IOCTL request\n");
        return -EINVAL;
        break;

    }
    return 0;
}


static int fpga_open(struct inode *inode, struct file *filp) {
    int minor = MINOR(inode->i_rdev) % V2D_CHAR_DEVICES;
    struct v2d_board *board = get_board_info(inode);
    filp->private_data = (void *) board;
    if (minor == I2C_DEV_MINOR) {
        if (test_and_set_bit(FPGA_BOARD_I2C_BUSY_BIT, &board->v_status) != 0)
            return -EBUSY;
    }
    return 0;
}

static int fpga_release(struct inode *inode, struct file *filp) {
    int minor = MINOR(inode->i_rdev) % V2D_CHAR_DEVICES;
    struct v2d_board *board = (struct v2d_board *) filp->private_data;
    if (minor == I2C_DEV_MINOR)
        test_and_clear_bit(FPGA_BOARD_I2C_BUSY_BIT, &board->v_status);
    return 0;
}

static int vpci_probe (struct pci_dev *pdev,
                       const struct pci_device_id *ent) {
    unsigned long pConfigMemBase = 0;
    struct v2d_board *board;
    int i=0;

    pci_set_master(pdev);


    pConfigMemBase = pci_resource_start(pdev, 2);
    switch (pdev->device) {
    case PLX_DEVICE:
        board = kzalloc(sizeof(*board), GFP_KERNEL);
        /// Check for malloc failure
            /// Set the Drop MemBase
            /// Set the ConifgMemBase
            board->v_pci_dev_type = pdev->device;
            board->v_pci_dev = pdev;
            board->v_hardware_detected = HARDWARE_DETECTED;
            // This depend on the motherboard and the slots that
            // the cards are in Shanker 04/22
            for (i=0; i < nMaxBoards; i++) {
                if (PCI_SLOT(pdev->devfn) == boardpcislots[i]) {
                    board->v_id = i;
                    break;
                }
            }
            if (i >= nMaxBoards) {
                board->v_id = boards;
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, "SLOT %d => %d\n",
                    PCI_SLOT(pdev->devfn) , boards);
            }
            //board->v_id = 0xd - PCI_SLOT(pdev->devfn);
            boards++;
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "PLX Enable : Bus -> 0x%x Dev fn -> 0x%x Vendor 0x%x Device 0x%x Class 0x%x IRQ -> 0x%x\n",
                     pdev->bus->number, PCI_SLOT(pdev->devfn), pdev->vendor,
                     pdev->device, pdev->class, pdev->irq);
            pci_set_drvdata (pdev, board);
            v2d_setup_cdev(board);
            v4_fpga_hardware_init(board);
            break;
    case V2DI50_DEVICE:
        board = kzalloc(sizeof(*board), GFP_KERNEL);
        board->v_id = boards;
        board->v_hardware_detected = HARDWARE_DETECTED;
        board->v_pci_dev_type = pdev->device;
        board->v_irq = pdev->irq;
        board->v_pci_dev = pdev;
        boards++;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "V2DI50 Enable : Bus -> 0x%x Dev fn -> 0x%x Vendor 0x%x Device 0x%x Class 0x%x IRQ -> 0x%x\n",
                 pdev->bus->number, PCI_SLOT(pdev->devfn), pdev->vendor,
                 pdev->device, pdev->class, pdev->irq);
        pci_set_drvdata (pdev, board);
        v2d_setup_cdev(board);
        v2_fpga_hardware_init(board);
        break;
    default:
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Unknown : Bus -> 0x%x Dev fn -> 0x%x Vendor 0x%x Device 0x%x Class 0x%x IRQ -> 0x%x\n",
                 pdev->bus->number, PCI_SLOT(pdev->devfn), pdev->vendor,
                 pdev->device, pdev->class, pdev->irq);
        break;
    }

    return 0;
}

static void vpci_remove(struct pci_dev *pdev) {
    struct v2d_board *board;
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
             "Disable : Dev fn -> 0x%x Vendor 0x%x Device 0x%x Class 0x%x IRQ -> 0x%x\n",
             pdev->devfn, pdev->vendor, pdev->device, pdev->class, pdev->irq);
    board = pci_get_drvdata(pdev);
    switch (pdev->device) {
    case PLX_DEVICE:
        v4_fpga_hardware_exit(board);
        break;
    case V2DI50_DEVICE:
        v2_fpga_hardware_exit(board);
        break;
    default:
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Unknown : Bus -> 0x%x Dev fn -> 0x%x Vendor 0x%x Device 0x%x Class 0x%x IRQ -> 0x%x\n",
                 pdev->bus->number, PCI_SLOT(pdev->devfn), pdev->vendor,
                 pdev->device, pdev->class, pdev->irq);
        break;
    }
    if (board != NULL) {
        v2d_teardown_cdev(board);
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Device Type %d %d\n",
                 board->v_id, board->v_pci_dev_type);
        kfree(board);
    }
    else {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Device Type %d Null\n", board->v_id);
    }
    pci_set_drvdata (pdev, NULL);
    pci_disable_device(pdev);
}
static int create_virtual_devices() {
    int i=0;
    int type;
    struct v2d_board *board;
    for (i=0; i < nVirtualBoards; i++) {
        type = virtualboards[i];
        switch (type) {
        case PLX_DEVICE:
            board = kzalloc(sizeof(*board), GFP_KERNEL);
            board->v_pci_dev_type = type;
            board->v_pci_dev = NULL;
            board->v_irq = 0;
            board->v_hardware_detected = 0;
            board->v_pci_dev_type = type;
            board->v_id = boards;
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "Creating Pseudo Device of type %d\n", type);
            v2d_setup_cdev(board);
            v4_fpga_hardware_init(board);
            pVirtualBoards[boards] = board;
            boards++;
            break;
        case V2DI50_DEVICE:
            board = kzalloc(sizeof(*board), GFP_KERNEL);
            board->v_pci_dev_type = type;
            board->v_id = boards;
            board->v_hardware_detected = 0;
            board->v_irq = 0;
            board->v_pci_dev = NULL;
            board->v_pci_dev_type = type;
            v2d_setup_cdev(board);
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "Creating Pseudo Device of type %d\n", type);
            v2_fpga_hardware_init(board);
            pVirtualBoards[boards] = board;
            boards++;
            break;
        default:
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "Unknown Pseudo Device of type %d\n", type);
            break;
        }
    }
    return 0;
}

static int destroy_virtual_devices() {
    int i=0;
    struct v2d_board *board=NULL;
    for (i=0; i < nVirtualBoards; i++) {
        board = pVirtualBoards[i];
        switch (virtualboards[i]) {
        case PLX_DEVICE:
            v4_fpga_hardware_exit(board);
            break;
        case V2DI50_DEVICE:
            v2_fpga_hardware_exit(board);
            break;
        default:
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "Unknown Virtual Device");
            break;
        }
        if (board != NULL) {
            v2d_teardown_cdev(board);
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                     "Board Type 0x%x\n",  board->v_pci_dev_type);
            kfree(board);
            pVirtualBoards[i] = NULL;
        }
    }
    return 0;
}

