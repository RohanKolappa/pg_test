
#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding the use o  f, 
 * or the results of the use of, the software and documentation in terms of correctness, 
 * accuracy, reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF USE, 
 * LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND.
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
#include "../../include/OmniTek_Exports.h"
#include "../efdma/OmniTekEFDMA_Exports.h"
#include "test_efdma_ioctl.h"


struct test_efdma_struct
{
  dev_t   dev;
  struct cdev                  cDev;             /*!< cdev struct for the BAR device */

  struct _OMNITEK_INTERFACE_EXTENSION * pExt;
  struct efdma_channel *                pEFDMA_Channel;

  struct list_head  Packets;

  struct class * efdma_class;

  spinlock_t        SpinLock;
  
  int   nPackets;
  int   nPackets_txd;
  int   nPackets_rxd;


};

struct test_efdma_packet
{
  struct list_head    Object;
  char * buffer;
  size_t length;
};


/************ DEBUG STUFF ***********/
#define TESTEFDMA        1
#define GENERAL          2

#define OMNITEK_TESTEFDMA_DEBUG_CATEGORIES (TESTEFDMA | GENERAL)
//#define OMNITEK_TESTEFDMA_DEBUG_CATEGORIES (0)

#define OmniTekDebug(category, level, format, Args...) \
    ({if (category & OMNITEK_TESTEFDMA_DEBUG_CATEGORIES) printk(level "%-35s: " format ,__func__,##Args);})

