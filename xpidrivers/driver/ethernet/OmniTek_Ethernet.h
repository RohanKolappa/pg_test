#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms
 * and conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding
 * the use of, or the results of the use of, the software and documentation 
 * in terms of correctness, accuracy, reliability, currentness, or otherwise; 
 * and you rely on the software, documentation and results solely at your own 
 * risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF 
 * USE, LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR 
 * CONSEQUENTIAL DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

#ifdef __KERNEL__
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
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/version.h>

#include <linux/phy.h>
#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <linux/timer.h>

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "../../include/OmniTek_Exports.h"
#include "../efdma/OmniTekEFDMA_Exports.h"

#endif


/************ DEBUG STUFF ***********/
#define ETH                 1
#define ETH_VERBOSE         2
#define ETH_TRACE           4
#define ETH_TX              8 
#define ETH_TX_VERBOSE      16
#define ETH_RX              32
#define ETH_RX_VERBOSE      64
#define ETH_INFO            128

#define LINK_SPEED_10G	    0
#define LINK_SPEED_1G	    1
#define LINK_SPEED_100M	    2
#define LINK_SPEED_10M	    4
#define LINK_SPEED_IDLE	    3

#ifdef __KERNEL__

#define PHY_COPPER_STSREG1  0x11
#define PHY_LED_CTRL        0x10
#define PHY_LED_POL         0x11
#define PHY_CU_INTR_EN      0x12
#define PHY_CU_INTR_STS     0x13
#define PHY_LED_TIMER_CTRL  0x12

#define OmniTekEthDebug(category, level, format, Args...) \
    ({if (category & ethdebug) \
     printk(level "%-35s: " format ,__func__,##Args);})

struct OmniTek_Ethernet_struct
{
  struct list_head  Devices;
};

typedef int  (*OmniTek_ext_ndo_set_multicast_list)(struct net_device *dev);
typedef int  (*OmniTek_ext_clear_multicast_list)(struct net_device *dev);
typedef void (*OmniTek_ext_stats_func)(struct net_device *dev, 
          struct net_device_stats * stats);

struct OmniTek_Ethernet_Device
{
  struct list_head Object;
  struct _OMNITEK_INTERFACE_EXTENSION *pExt;
  struct efdma_channel *pEFDMA_Channel;
  struct net_device *dev;
  struct net_device_stats stats;  

  OmniTek_ext_ndo_set_multicast_list *ndo_set_multicast_list;
  OmniTek_ext_clear_multicast_list *clear_multicast_list;
  OmniTek_ext_stats_func *ext_stats_func;

  struct timer_list poll_timer;
  bool link_up;
  int link_speed[2];
  int link_index;
  int phy_status[2];
  unsigned int upcount;

  struct work_struct tx_restart_work;
  bool tx_restarting;
  struct work_struct rx_restart_work;

  u32 Barco_stats_offset;
  u32 Barco_multicast_offset ;
  u32 Barco_mdio_offset;

#ifdef USE_POLLED_EFDMA
  struct napi_struct napi;
#endif
};
#endif      /* __KERNEL__ */
