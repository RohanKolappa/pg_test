/*
 *
 */

#ifndef _NETVIZ_FB_H_
#define _NETVIZ_FB_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/if.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <linux/highmem.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/swap.h>  // mark_page_accessed()

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "../../include/OmniTek_Exports.h"
#include "../OmniTek_linux.h"

//extern ssize_t OmniTek_MDMA_dev_transfer(struct file * filp, const char __user * buf, dma_addr_t dma_addr, size_t count, loff_t * f_pos, bool write, bool kernel_buffer);



/* Name of the driver for use by all the modules */
#define DRIVER_NAME "netvizfb"

typedef struct
{
	u16 width;
	u16 height;

}fb_resolution;

/* I/O control commands and structures for the netviz_fb driver */
//#define IOC_NETVIZFB_SET_RESOLUTION    _IO('F', 0x10)
/* ioctls */
#define FBIOPUT_VSCREENINFO 	0x4601
#define FBIO_TRANSPARANCY_ENABLE	0x4620
#define FBIO_BACKGROUND_ENABLE		0x4621



#endif /* _NETVIZ_FB_H_ */

