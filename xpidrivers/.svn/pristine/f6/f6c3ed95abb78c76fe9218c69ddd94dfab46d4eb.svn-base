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
#include <linux/types.h>	/* size_t */
#include <linux/errno.h>	/* error codes */
#include <linux/etherdevice.h>

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "../../include/OmniTek_Exports.h"
#include "../ethernet/OmniTek_Ethernet_exports.h"
#include "../OmniTek_linux.h" //ReadHWValue
#include "Barco_Ethernet.h"

#define MIN_REG_BLK_SIZE	0x40
#define BAD_OFFSET_BITS		(MIN_REG_BLK_SIZE - 1)

struct _OMNITEK_INTERFACE_EXTENSION * pExt = NULL;
struct net_device * pOmniTek_Net_Device = NULL;

int Barco_ndo_set_multicast_list(struct net_device * dev)
{
	struct _OMNITEK_INTERFACE_EXTENSION * pExt;
	struct OmniTek_Ethernet_Device * pDevice;
	int i = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
  struct dev_mc_list * mc_entry;
  int j = 0;
  pDevice = netdev_priv(dev);
  mc_entry = dev->mc_list;

#else
  struct netdev_hw_addr * ha;
  pDevice = netdev_priv(dev);
#endif
  pExt = pDevice->pExt;

  BarcoEthDebug(ETHERNET, KERN_INFO, "Configuring Multicast MAC Filters\n");
  

  if (pDevice->Barco_multicast_offset == 0)
    return -EINVAL;

#ifdef NO_MASK_ENTRIES
  BarcoEthDebug(ETHERNET, KERN_INFO, "Adding my mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
		dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i    ,dev->dev_addr[0] << 8 | dev->dev_addr[1]);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i + 1,dev->dev_addr[2] << 24 | dev->dev_addr[3] << 16 | dev->dev_addr[4] << 8 | dev->dev_addr[5] );
  i++;
#else
  BarcoEthDebug(ETHERNET, KERN_INFO, "Adding my mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
		dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    ,dev->dev_addr[0] << 8 | dev->dev_addr[1]);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1,dev->dev_addr[2] << 24 | dev->dev_addr[3] << 16 | dev->dev_addr[4] << 8 | dev->dev_addr[5] );
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0);
  i++;
#endif

#ifdef NO_MASK_ENTRIES
  BarcoEthDebug(ETHERNET, KERN_INFO, "Adding broadcast entry\n");
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i    , 0xFFFF);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i + 1, 0xFFFFFFFF);
  i++;
#else
  BarcoEthDebug(ETHERNET, KERN_INFO, "Adding broadcast entry\n");
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    , 0xFFFF);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1, 0xFFFFFFFF);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0);
  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0);
  i++;
#endif

#ifdef NO_MASK_ENTRIES
  if ((dev->flags & IFF_ALLMULTI) || (dev->flags & IFF_PROMISC))
  {
	BarcoEthDebug(ETHERNET, KERN_INFO, "IFF_ALLMULTI or IFF_PROMISC flag set, enabling everything...\n");
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i    , 0);
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i + 1, 0);
	i++;
  }
  else if (dev->flags & IFF_MULTICAST)
  {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
	if (dev->mc_count > (MAC_FILTER_ENTRIES - i) )
#else
	if (netdev_mc_count(dev) > (MAC_FILTER_ENTRIES - i) )
#endif
	{
	  BarcoEthDebug(ETHERNET, KERN_INFO, "Too many multicast entries in list: can only support %d, trying to set %d\n", (int)MAC_FILTER_ENTRIES,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
	  dev->mc_count
#else
	  netdev_mc_count(dev)
#endif
	  );
	  return -EINVAL;
	}
#else
	if ((dev->flags & IFF_ALLMULTI) || (dev->flags & IFF_PROMISC))
	{
	  BarcoEthDebug(ETHERNET, KERN_INFO, "IFF_ALLMULTI or IFF_PROMISC flag set, enabling everything...\n");
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    , 0);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1, 0);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0xFFFF);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0xFFFFFFFF);
	  i++;
	}
	else if (dev->flags & IFF_MULTICAST)
	{
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
	  if (dev->mc_count > (MAC_FILTER_ENTRIES - i) )
  #else
	  if (netdev_mc_count(dev) > (MAC_FILTER_ENTRIES - i) )
  #endif
	  {
		BarcoEthDebug(ETHERNET, KERN_INFO, "Too many multicast entries in list: can only support %d, trying to set %d\n", (int)MAC_FILTER_ENTRIES,
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
		dev->mc_count
  #else
		netdev_mc_count(dev)
  #endif
		);
		return -EINVAL;
	  }

#endif



    //Need to decode multicast list into the register space here...


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
    for ( j = 0;  j < dev->mc_count ; j++)
    {

      BarcoEthDebug(ETHERNET, KERN_INFO, "Setting Ethernet MAC filter for %02x:%02x:%02x:%02x:%02x:%02x\n",
        mc_entry->dmi_addr[0],mc_entry->dmi_addr[1],mc_entry->dmi_addr[2],mc_entry->dmi_addr[3],mc_entry->dmi_addr[4],mc_entry->dmi_addr[5]);

	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    ,mc_entry->dmi_addr[0] << 8 | mc_entry->dmi_addr[1]);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1,mc_entry->dmi_addr[2] << 24 | mc_entry->dmi_addr[3] << 16 | mc_entry->dmi_addr[4] << 8 | mc_entry->dmi_addr[5] );
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0); //?
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0); //?
      mc_entry = mc_entry->next;
      i++;
    }
#else
#ifdef NO_MASK_ENTRIES
	netdev_for_each_mc_addr(ha, dev)
	{

	  BarcoEthDebug(ETHERNET, KERN_INFO, "Setting Ethernet MAC filter for %02x:%02x:%02x:%02x:%02x:%02x\n",
		ha->addr[0],ha->addr[1],ha->addr[2],ha->addr[3],ha->addr[4],ha->addr[5]);

	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i    ,ha->addr[0] << 8 | ha->addr[1]);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i + 1,ha->addr[2] << 24 | ha->addr[3] << 16 | ha->addr[4] << 8 | ha->addr[5] );
	  i++;
	}
 #else
	netdev_for_each_mc_addr(ha, dev)
	{

	  BarcoEthDebug(ETHERNET, KERN_INFO, "Setting Ethernet MAC filter for %02x:%02x:%02x:%02x:%02x:%02x\n",
		ha->addr[0],ha->addr[1],ha->addr[2],ha->addr[3],ha->addr[4],ha->addr[5]);

	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    ,ha->addr[0] << 8 | ha->addr[1]);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1,ha->addr[2] << 24 | ha->addr[3] << 16 | ha->addr[4] << 8 | ha->addr[5] );
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0); //?
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0); //?
	  //mc_entry = mc_entry->next;
	  i++;
	}

#endif
#endif

  } 
  else
  {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
    if (dev->mc_count)
#else
    if (netdev_mc_count(dev))
#endif
      BarcoEthDebug(ETHERNET, KERN_INFO, "Multicast list present but IFF_MULTICAST flag not set, so multicast disabled\n");
  }
#ifdef NO_MASK_ENTRIES
  //Clear the other registers?
  for ( ; i < MAC_FILTER_ENTRIES ; i++)
  {
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i    , 0);
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 2*i + 1, 0);
  }
#else
  //Clear the other registers?
  for ( ; i < MAC_FILTER_ENTRIES ; i++)
  {
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    , 0);
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1, 0);
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0);
	WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0);
  }

#endif
  return 0;  
}

OmniTek_ext_ndo_set_multicast_list set_multicast_func_ptr = &Barco_ndo_set_multicast_list;


int Barco_clear_multicast_list(struct net_device * dev)
{
	int i = 0;
	struct _OMNITEK_INTERFACE_EXTENSION * pExt;
	struct OmniTek_Ethernet_Device * pDevice;
	pDevice = netdev_priv(dev);
	pExt = pDevice->pExt;
	//Clear all registers
	for (i = 0 ; i < MAC_FILTER_ENTRIES ; i++)
	{
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i    , 0);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 1, 0);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 2, 0);
	  WriteHWValue(pExt,1,pDevice->Barco_multicast_offset + 4*i + 3, 0);
	}
	return 0;
}
OmniTek_ext_clear_multicast_list clear_multicast_func_ptr = &Barco_clear_multicast_list;


void Barco_ext_stats_func(struct net_device * dev, struct net_device_stats * stats)
{
	struct _OMNITEK_INTERFACE_EXTENSION * pExt;
	struct OmniTek_Ethernet_Device * pDevice;
	pDevice = netdev_priv(dev);
	pExt = pDevice->pExt;

  BarcoEthDebug(STATISTICS, KERN_INFO, "Getting external stats\n");


  if (pDevice->Barco_stats_offset)
  {
	stats->rx_length_errors  = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 16);  //Undersized
	stats->rx_over_errors    = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 15);  //Oversized
	stats->rx_crc_errors     = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 1);   //Failed CRC - NB this will include any video packets that fail CRC?
	stats->rx_frame_errors   = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 11);  //Type/length mismatch?  Presumably a framing issue
    stats->rx_fifo_errors    = 0;
	stats->rx_missed_errors  = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 36) - stats->rx_packets;  //(Total packets) - (number driver received) to get dropped packets?

    stats->tx_aborted_errors      = 0;
    stats->tx_carrier_errors      = 0;
	stats->tx_fifo_errors         = ReadHWValue(pExt,1,pDevice->Barco_stats_offset + 21);  //TX Underrun?
    stats->tx_heartbeat_errors    = 0;
    stats->tx_window_errors       = 0;

    stats->collisions = 0;      //Do we even get collisions?

  }

}

OmniTek_ext_stats_func stats_func_ptr = &Barco_ext_stats_func;

static int __init Barco_Ethernet_init(void)
{
  struct OmniTek_Ethernet_Device * pOTethDevice;

  //Need to get hold of the OmniTek Ethernet device
  pOmniTek_Net_Device = OmniTek_Ethernet_get_device(0);

  BarcoEthDebug(ETHERNET, KERN_INFO, "Barco Ethernet Extensions Loading\n");

  if (pOmniTek_Net_Device == NULL)
  {
    BarcoEthDebug(ETHERNET, KERN_ERR, "Couldn't get OmniTek Ethernet net device\n");
    return -EINVAL;
  }
  else
  {
    BarcoEthDebug(ETHERNET, KERN_INFO, "Got net_device @ %p\n",pOmniTek_Net_Device);
  }

  //Read the required register offsets for the Barco register space
  pExt = OmniTekGetExtension(0);
  if (pExt == NULL)
  {
    BarcoEthDebug(ETHERNET, KERN_ERR, "Couldn't get OmniTek Interface Extension\n");
    return -EINVAL;
  }
  else
  {
    BarcoEthDebug(ETHERNET, KERN_INFO, "Got OmniTek Interface Extension @ %p\n",pExt);
  }
  
  //Read the two register offsets for the module
  pOTethDevice = netdev_priv(pOmniTek_Net_Device);
  pOTethDevice->Barco_multicast_offset = ReadHWValue(pExt,1,9);
  pOTethDevice->Barco_stats_offset = ReadHWValue(pExt,1,8);
  pOTethDevice->Barco_mdio_offset  = ReadHWValue(pExt,1,10);

  //Rudimentary check whether offsets are reasonable
  if (pOTethDevice->Barco_multicast_offset & BAD_OFFSET_BITS || pOTethDevice->Barco_stats_offset & BAD_OFFSET_BITS || pOTethDevice->Barco_mdio_offset & BAD_OFFSET_BITS)
  {
    BarcoEthDebug(ETHERNET, KERN_ERR, "Bogus multicast and/or ethernet and/or mac interface register offsets.  Exiting.\n");
    return -EINVAL;
  }

  BarcoEthDebug(ETHERNET, KERN_INFO, "Barco multicast filter registers at: %d\n",pOTethDevice->Barco_multicast_offset);
  BarcoEthDebug(ETHERNET, KERN_INFO, "Barco ethernet statistics registers at: %d\n",pOTethDevice->Barco_stats_offset);
  BarcoEthDebug(ETHERNET, KERN_INFO, "Barco MAC interface registers at: %d\n",pOTethDevice->Barco_mdio_offset);

  //Now we simply need to set the callback functions to our own:
  OmniTek_Ethernet_set_multicast_list_func(&set_multicast_func_ptr,pOmniTek_Net_Device);
  OmniTek_Ethernet_clear_multicast_list_func(&clear_multicast_func_ptr,pOmniTek_Net_Device);
  OmniTek_Ethernet_set_ext_stats_func(&stats_func_ptr, pOmniTek_Net_Device);
  
  return 0;
}

static void __exit Barco_Ethernet_exit(void)
{
  OmniTek_Ethernet_set_multicast_list_func(NULL,pOmniTek_Net_Device);
  OmniTek_Ethernet_clear_multicast_list_func(NULL,pOmniTek_Net_Device);
  OmniTek_Ethernet_set_ext_stats_func(NULL,pOmniTek_Net_Device);
}

MODULE_LICENSE("GPL");
module_init(Barco_Ethernet_init);
module_exit(Barco_Ethernet_exit);
