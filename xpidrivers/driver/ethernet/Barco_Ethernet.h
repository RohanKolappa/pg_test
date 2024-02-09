#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific 
 * terms and conditions.  Use of any of the software or derviatives thereof in 
 * any product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding 
 * the use of, or the results of the use of, the software and documentation in 
 * terms of correctness, accuracy, reliability, currentness, or otherwise; and 
 * you rely on the software, documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF
 * USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR 
 * CONSEQUENTIAL DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/errno.h>	/* error codes */
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "OmniTek_Ethernet_exports.h"

  /************ DEBUG STUFF ***********/

#define MIN_REG_BLK_SIZE	0x40
#define BAD_OFFSET_BITS		(MIN_REG_BLK_SIZE - 1)

#define ETHERNET        1
#define STATISTICS      2
//#define BARCO_ETH_DEBUG_CATEGORIES (ETHERNET)
//#define BARCO_ETH_DEBUG_CATEGORIES (STATISTICS)
#define BARCO_ETH_DEBUG_CATEGORIES (0)
//#define BARCO_ETH_DEBUG_CATEGORIES (-1)


#define BarcoEthDebug(category, level, format, Args...) \
    ({if (category & BARCO_ETH_DEBUG_CATEGORIES) printk(level "%-35s: " format ,__func__,##Args);})

