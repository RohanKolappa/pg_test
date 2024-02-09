/***************************************
 * APIC based Character Sleep Utility  * 
 ***************************************/
#include "kernelpatch.h"

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <asm/apic.h>

#define MAX_NUM_DEVS	50	

#define CSU_DEV_MAJOR	61
#define CSU_DEV_MINOR	0

void (*csu_function)(void);
int csu_register(void (*f)(void));
int csu_deregister(void (*f)(void));

static int csu_open(struct inode *ionde, struct file *filp);
static int csu_release(struct inode *inode, struct file *filp);
static ssize_t csu_read(struct file *filp, 
                           char *buff, 
                           size_t count, 
                           loff_t *offp);
static unsigned int csu_poll(struct file *filp, poll_table *wait);

struct file_operations csu_fops = {
    owner:		THIS_MODULE,
    open:		csu_open,
    release:		csu_release,
    read:		csu_read,
    poll:		csu_poll,
};

struct csu_device {
    wait_queue_head_t	sleepq;	/* queue of process sleeping on device */
    spinlock_t		lock;
    int			in_use;
    int 		wait_time;
};

static struct csu_device our_csu_dev[MAX_NUM_DEVS];
static unsigned long apic_time = 100;	/* Time in usecs between apic ticks */
static int numdevs = 0;	

static int csu_open(struct inode *inode, struct file *filp) {
    int i;

    if (MINOR(inode->i_rdev) != CSU_DEV_MINOR) 
        return -ENODEV;

    if (numdevs >= MAX_NUM_DEVS)
        return -EBUSY;
 
    for (i = 0; i < MAX_NUM_DEVS; i++) { 
        if (our_csu_dev[i].in_use == 0) {
            filp->private_data = &our_csu_dev[i];
            our_csu_dev[i].in_use = 1;
            break;
        }
    }

    numdevs++;

    MOD_INC_USE_COUNT;

    return 0;
}

static int csu_release(struct inode *inode, struct file *filp) {
    struct csu_device *dev = filp->private_data;

    dev->in_use = 0;
    numdevs--;
    MOD_DEC_USE_COUNT;
    return 0;
}

static ssize_t csu_read(struct file *filp, 
                        char *buff, 
                        size_t count, 
                        loff_t *offp) {
    struct csu_device *dev = filp->private_data;

    if (!access_ok(VERIFY_WRITE, buff, count))
        return -EFAULT;

    if (count <= 0)
        return 0;

    dev->wait_time = 10 * count;

    while (dev->wait_time) {
        if (wait_event_interruptible(dev->sleepq, dev->wait_time <= 0) < 0) {
            return -ERESTARTSYS;
        }
    }

    return count; 
}

static unsigned int csu_poll(struct file *filp, poll_table *wait) {
    struct csu_device *dev = filp->private_data;

    poll_wait(filp, &dev->sleepq,  wait);

    if (dev->wait_time <= 0) {
        printk("csu_poll: returning 0x%x\n", POLLIN|POLLRDNORM);
	return POLLIN | POLLRDNORM;  /* readable if wait time expired */
    }
    printk("csu_poll: returning 0\n");
    return 0;
}

static void apic_callback(void) {
    unsigned long lock_flags;
    int i;

    for (i = 0; i < MAX_NUM_DEVS; i++) {

        if (our_csu_dev[i].in_use == 0)
            continue;

        spin_lock_irqsave(&our_csu_dev[i].lock, lock_flags);
        if (our_csu_dev[i].wait_time > 0) {
            --our_csu_dev[i].wait_time;
        }
        spin_unlock_irqrestore(&our_csu_dev[i].lock, lock_flags);
    
        if (our_csu_dev[i].wait_time == 0)
            wake_up_interruptible(&our_csu_dev[i].sleepq);
    }
    if (csu_function != NULL)
        csu_function();
}

static int __init csu_module_init(void)
{
    int i;

    if (register_chrdev(CSU_DEV_MAJOR,  "csu", &csu_fops) < 0)
        return -1;
    SET_MODULE_OWNER(&csu_fops);

    csu_function=NULL;
    set_apic_timer_callback(apic_callback);
    set_apic_timer_frequency(1000000/apic_time);
    
    numdevs = 0;

    for (i = 0; i < MAX_NUM_DEVS; i++) {
	our_csu_dev[i].in_use = 0;
        init_waitqueue_head(&our_csu_dev[i].sleepq); 
        spin_lock_init(&our_csu_dev[i].lock);
    }

    printk("CSU Module Loaded\n");

    return 0;
}

static void __exit csu_module_exit(void) {
    if (unregister_chrdev(CSU_DEV_MAJOR, "csu") < 0) 
	printk("Warning: Failed to remove CSU device\n");

    set_apic_timer_callback(NULL);
 
    printk("CSU Module Terminated\n");
}
int csu_register(void (*f)(void))
{
    csu_function = f;
    printk ("registering function with csu\n");
    return 0;
}

int csu_deregister(void (*f)(void))
{
    if (csu_function == f) {
        csu_function = NULL;
        printk ("de-registering function with csu: Success\n");
    }
    else {
        printk ("de-registering function with csu: Failure\n");
    }
    return 0;
}


module_init(csu_module_init);
module_exit(csu_module_exit);

MODULE_AUTHOR("IP Video Systems");
MODULE_DESCRIPTION("APIC based Character Sleep Utility");
MODULE_LICENSE("GPL");
