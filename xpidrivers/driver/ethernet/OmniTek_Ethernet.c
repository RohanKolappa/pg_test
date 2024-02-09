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
#include <linux/moduleparam.h>
#include <linux/ethtool.h>
#include <linux/version.h>
#include <mach/gpio.h>
#include "OmniTek_Ethernet.h"
#include "Barco_Ethernet.h"

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "../../include/OmniTek_Exports.h"
#include "../ethernet/OmniTek_Ethernet_exports.h"
#include "../OmniTek_linux.h"   //ReadHWValue

#define RETRY_NETWORK 60

static int ethdebug = ETH_INFO;
static int efdma_channel = -1;
static struct OmniTek_Ethernet_struct OmniTek_Ethernet;

module_param(efdma_channel,int,S_IRUGO);

/*----------------------------------- sysfs ----------------------------------*/

#ifdef CONFIG_SYSFS

/* sysfs interface definition:
 *
 * Upon loading the driver will create a sysfs directory under
 * /sys/devices/pci0000:00/0000:00:00.0/net/eth0
 *
 * This directory exports the following interfaces.
 * ethdebug      :  flag for debug messages (integer in hex format, i.e.
 *                  echo 11 > ethdebug sets ethdebug to 0x11)
 * phy0_status   :  up or down status of PHY 0
 * phy1_status   :  up or down status of PHY 1
 * link_index    :  which link is active
 * link0_speed   :  link 0 speed (1G, 100M, 10M or undefined)
 * link1_speed   :  link 0 speed (1G, 100M, 10M or undefined)
 */

static ssize_t show_ethdebug(struct device *dev,
                             struct device_attribute *attr,
                             char *buf)
{
    return sprintf(buf, "%x\n", ethdebug);
}

static ssize_t store_ethdebug(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;

    tmp = simple_strtoul(buf, NULL, 16);
    ethdebug = tmp;
    if (ethdebug & ETH_VERBOSE)
        ethdebug |= ETH;
    if (ethdebug & ETH_TX_VERBOSE)
        ethdebug |= ETH_TX;
    if (ethdebug & ETH_RX_VERBOSE)
        ethdebug |= ETH_RX;
    return strnlen(buf, count);
}

static DEVICE_ATTR(ethdebug, S_IRUGO|S_IWUSR, show_ethdebug, store_ethdebug);

static ssize_t show_phy0_status(struct device *dev,
                               struct device_attribute *attr,
                               char *buf)
{
    struct net_device *netdev = to_net_dev(dev);
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(netdev);

    return sprintf(buf, "%u\n", pDevice->phy_status[0]);
}
static DEVICE_ATTR(phy0_status, S_IRUGO, show_phy0_status, NULL);

static ssize_t show_phy1_status(struct device *dev,
                               struct device_attribute *attr,
                               char *buf)
{
    struct net_device *netdev = to_net_dev(dev);
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(netdev);

    return sprintf(buf, "%u\n", pDevice->phy_status[1]);
}
static DEVICE_ATTR(phy1_status, S_IRUGO, show_phy1_status, NULL);

static ssize_t show_link_index(struct device *dev,
                               struct device_attribute *attr,
                               char *buf)
{
    struct net_device *netdev = to_net_dev(dev);
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(netdev);

    return sprintf(buf, "%u\n", pDevice->link_index);
}

static DEVICE_ATTR(link_index, S_IRUGO, show_link_index, NULL);

static ssize_t show_link0_speed(struct device *dev,
                                struct device_attribute *attr,
                                char *buf)
{
    struct net_device *netdev = to_net_dev(dev);
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(netdev);

    return sprintf(buf, "%u\n", pDevice->link_speed[0]);
}

static DEVICE_ATTR(link0_speed, S_IRUGO, show_link0_speed, NULL);

static ssize_t show_link1_speed(struct device *dev,
                                struct device_attribute *attr,
                                char *buf)
{
    struct net_device *netdev = to_net_dev(dev);
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(netdev);

    return sprintf(buf, "%u\n", pDevice->link_speed[1]);
}

static DEVICE_ATTR(link1_speed, S_IRUGO, show_link1_speed, NULL);

static int OmniTek_Ethernet_init_sysfs(struct net_device *netdev)
{
    int err = 0;

    err = device_create_file(&netdev->dev, &dev_attr_ethdebug);
    err |= device_create_file(&netdev->dev, &dev_attr_phy0_status);
    err |= device_create_file(&netdev->dev, &dev_attr_phy1_status);
    err |= device_create_file(&netdev->dev, &dev_attr_link_index);
    err |= device_create_file(&netdev->dev, &dev_attr_link0_speed);
    err |= device_create_file(&netdev->dev, &dev_attr_link1_speed);

    if (err)
        printk(KERN_WARNING "Error creating OmniTek_Ethernet sysfs files.\n");

    return err;
}

static void OmniTek_Ethernet_remove_sysfs(struct net_device *netdev)
{
    device_remove_file(&netdev->dev, &dev_attr_ethdebug);
    device_remove_file(&netdev->dev, &dev_attr_phy0_status);
    device_remove_file(&netdev->dev, &dev_attr_phy1_status);
    device_remove_file(&netdev->dev, &dev_attr_link_index);
    device_remove_file(&netdev->dev, &dev_attr_link0_speed);
    device_remove_file(&netdev->dev, &dev_attr_link1_speed);
}

#endif

/*----------------------------------------------------------------------------*/

int OmniTek_Ethernet_open(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH, KERN_INFO,
        "Ethernet driver requested open for device %p - OmniTek Device %p\n",
        dev,pDevice);

    netif_start_queue(dev);

#ifdef USE_POLLED_EFDMA
    if (pDevice->napi.poll) {
        OmniTekEthDebug(ETH_VERBOSE,KERN_INFO,"Enabling NAPI %p\n",
            &(pDevice->napi));
        OmniTekEthDebug(ETH, KERN_INFO,
            "Napi Struct has: device %p, gro_list %p, skb %p, poll func %p\n",
            pDevice->napi.dev, pDevice->napi.gro_list, pDevice->napi.skb, 
            pDevice->napi.poll);

        napi_enable(&(pDevice->napi));

        OmniTekEthDebug(ETH, KERN_INFO,"Enabling Event Interrupts %p\n",
            pDevice->pEFDMA_Channel);
    }
#endif

    efdma_channel_rx_interrupt_enable(pDevice->pEFDMA_Channel, true);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    return 0;
}

int OmniTek_Ethernet_stop(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH, KERN_INFO,
        "Ethernet driver requested stop for device %p\n",dev);

    netif_stop_queue(dev);

#ifdef USE_POLLED_EFDMA
    efdma_channel_rx_interrupt_enable(pDevice->pEFDMA_Channel, false);
    napi_disable(&(pDevice->napi));
#endif

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    return 0;
}

int OmniTek_Ethernet_set_mac_address(struct net_device *dev, void *addr)
{
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH, KERN_INFO,
        "Ethernet driver got set mac address for %p\n",dev);
    OmniTekEthDebug(ETH, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    return eth_mac_addr(dev,addr);
}

void OmniTek_Ethernet_set_multicast_list(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH, KERN_INFO, 
        "Set Multicast list for %p - MAC addr %02x:%02x:%02x:%02x:%02x:%02x\n",
        dev, dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], 
        dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
    //OmniTekEthDebug(ETHERNET,KERN_INFO,
    //   "Set Multicast list for %p - %s\n",dev,dev->dev_addr);

    if (pDevice->ndo_set_multicast_list)
        (*pDevice->ndo_set_multicast_list)(dev);
    else
        OmniTekEthDebug(ETH, KERN_ERR,
            "No Multicast config callback provided - not setting multicast\n");

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
}

int OmniTek_Ethernet_set_multicast_list_func(
    OmniTek_ext_ndo_set_multicast_list *ndo_set_multicast_list,
    struct net_device * dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    pDevice->ndo_set_multicast_list = ndo_set_multicast_list;

    OmniTekEthDebug(ETH, KERN_INFO,
        "set_multicast_list function set to %p\n",ndo_set_multicast_list);
    return 0;
}
EXPORT_SYMBOL(OmniTek_Ethernet_set_multicast_list_func);

int OmniTek_Ethernet_clear_multicast_list_func(
    OmniTek_ext_clear_multicast_list *clear_multicast_list,
    struct net_device *dev)
{
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(dev);

    pDevice->clear_multicast_list = clear_multicast_list;

    OmniTekEthDebug(ETH, KERN_INFO,
        "clear_multicast_list function set to %p\n",clear_multicast_list);

    return 0;
}
EXPORT_SYMBOL(OmniTek_Ethernet_clear_multicast_list_func);


int OmniTek_Ethernet_validate_addr(struct net_device *dev)
{
    int result;

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH, KERN_INFO,"Validate mac address for %p - %s\n",
        dev,dev->dev_addr);

    if ((result = eth_validate_addr(dev)))
        OmniTekEthDebug(ETH, KERN_INFO,
            "validate mac address for %p returned %d\n",dev,result);

    OmniTekEthDebug(ETH, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
  
    return result;
}

int OmniTek_Ethernet_rx_restart(struct OmniTek_Ethernet_Device *pDevice)
{
    int result = 0;

    OmniTekEthDebug(ETH_RX, KERN_ERR,"Rx Restart\n");

    result =  efdma_channel_rx_restart(pDevice->pEFDMA_Channel);
    if (result)
        OmniTekEthDebug(ETH_RX, KERN_ERR,"Rx Restart FAILED!\n");

    // Restore the multicast entries
    OmniTekEthDebug(ETH_RX, KERN_ERR,"Restore MAC filter entries\n");
    if (pDevice->ndo_set_multicast_list) {
        (*pDevice->ndo_set_multicast_list)(pDevice->dev);
    }

    return result;
}

void OmniTek_Ethernet_Rx_restart_work(struct work_struct *work)
{
    struct OmniTek_Ethernet_Device *pDevice = container_of(work, 
        struct OmniTek_Ethernet_Device, rx_restart_work);
    OmniTek_Ethernet_rx_restart(pDevice);
}

void OmniTek_Ethernet_Rx_failed(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);  

    // Clear the multicast entries --> block MAC filter for all 
    // consecutive entry attempts
    OmniTekEthDebug(ETH_RX, KERN_ERR,
        "Ethernet RX failed --> block MAC filter \n");

    if (pDevice->clear_multicast_list) {
        (*pDevice->clear_multicast_list)(dev);
    }

    OmniTekEthDebug(ETH_RX_VERBOSE, KERN_ERR,
        "Queueing RX Restart Work.  Work struct @ %p. Function %p\n", 
        &(pDevice->rx_restart_work), pDevice->rx_restart_work.func);
    
#if 0
    dump_stack();
#endif

    PREPARE_WORK(&(pDevice->rx_restart_work), OmniTek_Ethernet_Rx_restart_work);
    schedule_work(&(pDevice->rx_restart_work));
}

int OmniTek_Ethernet_tx_restart(struct OmniTek_Ethernet_Device *pDevice)
{
    int result = 0;

    OmniTekEthDebug(ETH_TX, KERN_ERR,"Tx Restart\n");

    result =  efdma_channel_tx_restart(pDevice->pEFDMA_Channel);
    if (!result) {
        pDevice->dev->trans_start = jiffies;
        netif_wake_queue(pDevice->dev);
    } 
    else {
        OmniTekEthDebug(ETH_TX, KERN_ERR,"Tx Restart FAILED!\n");  
    }
    return result;
}


void OmniTek_Ethernet_Tx_restart_work(struct work_struct *work)
{
    struct OmniTek_Ethernet_Device *pDevice = container_of(work, 
        struct OmniTek_Ethernet_Device, tx_restart_work);

    pDevice->tx_restarting = true;
    OmniTek_Ethernet_tx_restart(pDevice);
    pDevice->tx_restarting = false;
}

void OmniTek_Ethernet_tx_timeout(struct net_device * dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);  

    OmniTekEthDebug(ETH, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;

    PREPARE_WORK(&(pDevice->tx_restart_work), OmniTek_Ethernet_Tx_restart_work);
    if (pDevice->tx_restarting == false)
        schedule_work(&(pDevice->tx_restart_work));

    OmniTekEthDebug(ETH, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
}

int OmniTek_Ethernet_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int result = -EBUSY;
    int retval = 0;
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n",
          __func__, jiffies_to_msecs(jiffies)) ;

    //TODO: Transmit side throttling...
    OmniTekEthDebug(ETH_TX_VERBOSE, KERN_INFO, 
            "Requested to transmit packet\n");  
  
    dev->trans_start = jiffies;

    result = efdma_channel_transmit_packet(pDevice->pEFDMA_Channel, skb->data, 
        skb->len, 0, ETH_ZLEN);

    if (result == -EBUSY) {
        OmniTekEthDebug(ETH_TX_VERBOSE, KERN_ERR,
            "Ringbuffer returned busy, stopping queue!  "
            "*****************************************************\n");
        netif_stop_queue(dev);
        pDevice->stats.tx_dropped++;
        retval = NET_XMIT_DROP;
        //dump_stack();
    }
    else if (result < 0) {
        OmniTekEthDebug(ETH_TX, KERN_ERR,
            "Error transmitting packet: %d\n",result);
        pDevice->stats.tx_dropped++;

        //Let the watchdog handle timeouts...
        //OmniTek_Ethernet_tx_restart(pDevice->pEFDMA_Channel);   
        //netif_wake_queue(dev);    //Resume queue after restart

        retval = NET_XMIT_DROP;
        //dump_stack();
    }  
    else {
        pDevice->stats.tx_packets++;
        pDevice->stats.tx_bytes+=skb->len;
        OmniTekEthDebug(ETH_TX_VERBOSE, KERN_INFO,"Packet Transmitted\n");  

        //Finished with packet?
        dev_kfree_skb_any(skb);
        retval = NET_XMIT_SUCCESS;
    }

    //Once we've transmitted our packet check to see if there is space 
    //in the ringbuffer for the next packet + header + potential packing
    //If the space is running low then we stop the queue. It will be 
    //resumed on a DMA completion...
    if (efdma_channel_transmit_space(pDevice->pEFDMA_Channel) < 
            dev->mtu + 2*sizeof(u64)) {
        OmniTekEthDebug(ETH_TX, KERN_INFO,
            "Ringbuffer low on space [%d], stopping queue.\n", 
            efdma_channel_transmit_space(pDevice->pEFDMA_Channel));
        netif_stop_queue(dev);
    }
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    return retval;
}

struct net_device_stats *OmniTek_Ethernet_get_stats(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;

    if (pDevice->ext_stats_func)
        (*pDevice->ext_stats_func)(dev,&pDevice->stats);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    return &(pDevice->stats);
}

void OmniTek_Ethernet_set_ext_stats_func(OmniTek_ext_stats_func *ext_stats_func,
                                         struct net_device *dev)
{
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(dev);

    pDevice->ext_stats_func = ext_stats_func;

    OmniTekEthDebug(ETH, KERN_INFO,
        "external statistics function set to %p\n",ext_stats_func);

}
EXPORT_SYMBOL(OmniTek_Ethernet_set_ext_stats_func);

void OmniTek_Ethernet_tx_wake(void * priv)
{
    struct net_device * dev = (struct net_device*)priv;

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH_TX_VERBOSE, KERN_INFO,
        "TX has completed a slice, resuming queue\n");

    netif_wake_queue(dev);

    OmniTekEthDebug(ETH, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
}

efdma_tx_wake_func OmniTek_Ethernet_tx_wake_func_ptr = OmniTek_Ethernet_tx_wake;

int OmniTek_Ethernet_receive_func(struct efdma_channel *pEFDMA_Channel,
                                  void *priv, 
                                  ssize_t size)
{
    int result; 
    struct sk_buff * skb;
    struct net_device *dev = (struct net_device *)priv;
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n", 
       __func__, jiffies_to_msecs(jiffies)) ;

    //If something went wrong then auto-restart the channel...
    if (unlikely(size < 0)) {
        OmniTekEthDebug(ETH_RX, KERN_ERR,
            "Error receiving packet : %d restarting channel\n", size);
        OmniTek_Ethernet_Rx_failed(dev);
        return -EINVAL;
    }

    if (unlikely(size == 0)) {
        OmniTekEthDebug(ETH_RX, KERN_ERR,"Got zero sized packet\n");
        OmniTek_Ethernet_Rx_failed(dev);
        return -EINVAL;
    }

    //+2 allows us to align the IP address on a 16b boundary below  
    skb = dev_alloc_skb(size + 2);  

    if (unlikely(!skb)) {
        if (printk_ratelimit()) {
            OmniTekEthDebug(ETH_RX, KERN_ERR,
                "Couldn't allocate packet buffer - low memory?\n");
            //TODO: dropped packet stats...
        }
        return -EINVAL;
    }

    skb_reserve(skb, 2); /* align IP on 16B boundary */  
    efdma_channel_receive_packet(pEFDMA_Channel,skb_put(skb,size),size);
    //efdma_channel_receive_packet(pEFDMA_Channel,buffer,size);
    //memcpy(skb_put(skb,size),buffer,size);  
    
    if (unlikely(!dev)) {
        OmniTekEthDebug(ETH_RX,KERN_ERR,
            "Net Device has gone away but receive func still called!\n");
        dump_stack();
        return -EINVAL;
    }

    skb->dev = dev;
    skb->protocol = eth_type_trans(skb,dev);
    skb->ip_summed = CHECKSUM_NONE;

    OmniTekEthDebug(ETH_RX_VERBOSE, KERN_INFO,
        "Ethernet Packet received, size %zd\n", size);
  
    if (in_interrupt())
        result = netif_rx(skb);
    else
        result = netif_rx_ni(skb);
  
    pDevice->stats.rx_packets++;
    pDevice->stats.rx_bytes+=size;

    if (skb->pkt_type == PACKET_MULTICAST) 
        pDevice->stats.multicast++;

    if (result)
        OmniTekEthDebug(ETH_RX, KERN_ERR, 
            "netif_rx returned %d for packet\n", result);

    //kfree(buffer);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;

    return 0;
}

efdma_receive_func OmniTek_Ethernet_receive_func_ptr = 
    OmniTek_Ethernet_receive_func;

#ifdef USE_POLLED_EFDMA

//Called by EFDMA after an event to indicate that data is available 
//and a polling function should be scheduled
int OmniTek_Ethernet_schedule_poll_func(struct efdma_channel *pEFDMA_Channel, 
                                        void *priv)
{
    struct net_device * dev = (struct net_device *)priv;
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Entering, jiffies %u.\n",
          __func__, jiffies_to_msecs(jiffies)) ;
    //if (!napi_schedule_prep(&(pDevice->napi))) {
    //    OmniTekEthDebug(ETH_RX, KERN_ERR,
    //          "napi_schedule_prep failed - already running? Shouldn't be!\n");
    //    return -1;
    //}
    OmniTekEthDebug(ETH_RX_VERBOSE, KERN_INFO,
        "Ethernet NAPI Schedule Poll called\n");
  
    napi_schedule(&(pDevice->napi));
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s,Exiting, jiffies %u.\n", 
      __func__, jiffies_to_msecs(jiffies)) ;
    return 0;
}

efdma_schedule_poll_func OmniTek_Ethernet_schedule_poll_ptr = 
    OmniTek_Ethernet_schedule_poll_func;

static int OmniTek_Ethernet_napi_poll(struct napi_struct *napi, int budget)
{
    int result = 0;
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(napi->dev);

    //So we get called here when we are ready for packets...
    //call down to the efdma layer receive_packets?

    OmniTekEthDebug(ETH_RX_VERBOSE, KERN_INFO,
        "Ethernet NAPI Poll called - budget: %d packets\n", budget);

    //Poll the device to get data xferred etc.
    efdma_channel_rx_poll(&(pDevice->pEFDMA_Channel->receive));

    result = efdma_channel_receive_packets(&(pDevice->pEFDMA_Channel->receive), 
        budget);

    if (result == 0) {
        OmniTekEthDebug(ETH_RX_VERBOSE, KERN_INFO,
            "Ethernet NAPI Poll returned complete\n");
        napi_complete(&pDevice->napi);

        //Do the interrupt enabling in the packet layer... 
        //Re-enable interrupts now that we've finished   
        //efdma_channel_rx_interrupt_enable(pDevice->pEFDMA_Channel, true);
    
        //If the channel has stopped  
        if (!efdma_channel_rx_running(pDevice->pEFDMA_Channel)) {
            OmniTekEthDebug(ETH_RX, KERN_ERR,
                "Ethernet NAPI Poll Complete but channel stopped?\n");    
            OmniTek_Ethernet_Rx_failed(napi->dev);
        }
        return 0;
    }
    else if (result == 1) {
        OmniTekEthDebug(ETH_RX, KERN_INFO,
            "Ethernet NAPI Poll returned incomplete\n");
        return 1;
    }
    else {
        //Something went wrong?  Should restart - re-enable interrupts
        //Do the interrupt enabling in the packet layer
        //Re-enable interrupts now that we've finished   
        //efdma_channel_rx_interrupt_enable(pDevice->pEFDMA_Channel, true);

        if (!efdma_channel_rx_running(pDevice->pEFDMA_Channel)) {
            OmniTekEthDebug(ETH_RX, KERN_ERR,
                "Ethernet NAPI Poll Failed and channel is stopped?\n");    
            OmniTek_Ethernet_Rx_failed(napi->dev);
        }
        return result;
    }
}
#endif

static const struct net_device_ops OmniTek_Ethernet_netdev_ops = {
    .ndo_open               = OmniTek_Ethernet_open,
    .ndo_stop               = OmniTek_Ethernet_stop,
    .ndo_validate_addr      = OmniTek_Ethernet_validate_addr,
    .ndo_set_mac_address    = OmniTek_Ethernet_set_mac_address,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,1,0)
    .ndo_set_rx_mode        = OmniTek_Ethernet_set_multicast_list,
#else
    .ndo_set_multicast_list = OmniTek_Ethernet_set_multicast_list,
#endif
    .ndo_get_stats          = OmniTek_Ethernet_get_stats,
    .ndo_do_ioctl           = NULL,
    .ndo_start_xmit         = OmniTek_Ethernet_start_xmit,
    .ndo_tx_timeout         = OmniTek_Ethernet_tx_timeout,
#ifdef BROKEN
    .ndo_change_mtu         = NULL,
#endif

#ifdef CONFIG_NET_POLL_CONTROLLER
    .ndo_poll_controller    = NULL,
#endif
};

void OmniTek_Ethernet_get_drvinfo(struct net_device *dev, 
                                  struct ethtool_drvinfo *info)
{
    struct OmniTek_Ethernet_Device *pDevice = netdev_priv(dev);

    strcpy(info->driver, "OmniTek XPi 10/100/1gE driver");
    strcpy(info->version, "1.0");
    strcpy(info->bus_info,pci_name(pDevice->pExt->Device));
}

u32 OmniTek_Ethernet_get_link(struct net_device *dev)
{
    struct OmniTek_Ethernet_Device * pDevice = netdev_priv(dev);

    if (pDevice->link_speed[pDevice->link_index] != LINK_SPEED_IDLE) {
        return 1;
    }
    return 0;
}

int OmniTek_Ethernet_get_settings(struct net_device *dev, 
                                  struct ethtool_cmd *cmd)
{
    return -EOPNOTSUPP;
}

#if 0
static void OmniTek_phy_init(struct OmniTek_Ethernet_Device *pDevice)
{
    int offset[2] = {0, 0x400};
    int phyad[2] = {1, 1};
    int i;
    int mo = pDevice->Barco_mdio_offset;
    struct _OMNITEK_INTERFACE_EXTENSION *pExt;

    pExt = pDevice->pExt;
}

    if (pDevice->phy_status[0] && pDevice->phy_status[1]) {
        //Set LEDs on phy to give link status
        for (i = 0; i < 2; i++) {
            xpi_paged_mdio_write(pExt, mo, offset[i], phyad[i], 0, 
                    PHY_CU_INTR_EN, 0x6400);
            //LED[2] 10, LED[1] 100, LED[2] 10000
            xpi_paged_mdio_write(pExt, mo, offset[i], phyad[i], 3, 
                    PHY_LED_CTRL, 0x17aa);
            xpi_paged_mdio_write(pExt, mo, offset[i], phyad[i], 3, 
                    PHY_LED_POL, 0x4415);
            xpi_paged_mdio_write(pExt, mo, offset[i], phyad[i], 3, 
                    PHY_LED_TIMER_CTRL, 0x0885);
        }
    }
}
#endif

static void OmniTek_Ethernet_get_link_speed(
    struct OmniTek_Ethernet_Device *pDevice)
{
    int offset[2] = {MAIN_OFFSET, EXPANSION_OFFSET};
    int phyad[2] = {XPI_MDIO_PHYAD, XPI_MDIO_PHYAD};
    int link_status[2], old_link_speed[2];
    int intr[2];
    u32 result[2];
    int ind, speed, i, temp1, temp2;
    unsigned long value;
    int mo = pDevice->Barco_mdio_offset;
    struct _OMNITEK_INTERFACE_EXTENSION *pExt;

    pExt = pDevice->pExt;

    //read page 0 register 2 to check we can read phy registes
    for (i = 0; i < 2; i++) {
        temp1 = xpi_paged_mdio_read(pExt, mo, offset[i], phyad[i], 0x0, 0x2);
        temp2 = xpi_paged_mdio_read(pExt, mo, offset[i], phyad[i], 0x0, 
                PHY_CU_INTR_EN);
        if ((temp1 == 0x0141) && (temp2 == 0x6400)) 
            pDevice->phy_status[i] = 1;
        else {
            pDevice->phy_status[i] = 0;
            OmniTekDebug(ETH, KERN_INFO, "%s: Couldn't access phy %d, "
                    "0x0141 != 0x%x or 0x6400 != 0x%x\n", __func__, i, 
                    temp1, temp2);
        }
    }

    if (pDevice->phy_status[0] && pDevice->phy_status[1] ) {
        //interrupt status from FPGA for mainboard
        xpi_get_omnitek_val(pExt,1,0x33,&value) ;
        result[0] = value & 0x200;
        xpi_get_s3fpga_val(pExt, 0x0c, &value) ;
        result[1] = value & 0x10 ;

        if ((pDevice->upcount == 0) || result[0] || result[1] || 
                ((pDevice->link_speed[0] == LINK_SPEED_IDLE) && 
                 (pDevice->link_speed[1] == LINK_SPEED_IDLE))) {
            if (result[0])
                OmniTekEthDebug(ETH, KERN_INFO, 
                        "%s Main fpga interrupt = 0x%x\n",__func__, 
                        result[0]);
            if (result[1])
                OmniTekEthDebug(ETH, KERN_INFO, 
                        "%s Expansion fpga interrupt = 0x%x\n",__func__, 
                        result[1]);

            //OmniTek_phy_init(pDevice);

            //Read phy registers to get link speed
            for (i = 0; i < 2; i++) {

                old_link_speed[i] = pDevice->link_speed[i] ;

                //read status with register space set to copper (page 0)
                link_status[i] = xpi_paged_mdio_read(pExt, mo, offset[i], 
                        phyad[i], 0, PHY_COPPER_STSREG1);

                //interrupt should clear after reading status register
                intr[i] = xpi_paged_mdio_read(pExt, mo, offset[i], 
                        phyad[i], 0, PHY_CU_INTR_STS);
                if (intr[i]) {
                    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO,
                        "%s index %d interrupt status 0x%x link_status 0x%x\n",
                        __func__, i, intr[i], link_status[i]);
                }

                //link is up and speed & duplex are resolved
                if (link_status[i] & 0x0c00) {
                    switch ((link_status[i] & 0xc000) >> 14 ) {
                    case 2:
                        OmniTekEthDebug(ETH, KERN_INFO,
                                "Link %d Detected Line Speed 1G\n", i);
                        pDevice->link_speed[i] = LINK_SPEED_1G ;
                        break;
                    case 1:
                        OmniTekEthDebug(ETH, KERN_INFO,
                                "Link %d Detected Line Speed 100M\n", i);
                        pDevice->link_speed[i] = LINK_SPEED_100M ;
                        break;
                    case 0:
                        OmniTekEthDebug(ETH, KERN_INFO,
                                "Link %d Detected Line Speed 10M\n", i);
                        pDevice->link_speed[i] = LINK_SPEED_10M ;
                        break;
                    default:
                        OmniTekEthDebug(ETH, KERN_INFO,
                                "Link %d Unknown Line Speed\n", i);
                        pDevice->link_speed[i] = LINK_SPEED_IDLE ;
                        break;            
                    }
                }
                else {
                    OmniTekEthDebug(ETH, KERN_INFO,
                            "Link %d Ethernet link is not resolved.\n", i);
                    pDevice->link_speed[i] = LINK_SPEED_IDLE ;
                }
            }

            //Check which links are up and if link_index needs to change
            if ((pDevice->link_speed[0] == LINK_SPEED_IDLE) && 
                    (pDevice->link_speed[1] == LINK_SPEED_IDLE)) 
                ind = 0;
            else if ((pDevice->link_speed[0] != LINK_SPEED_IDLE) && 
                    (pDevice->link_speed[1] == LINK_SPEED_IDLE)) 
                ind = 0;
            else if ((pDevice->link_speed[0] == LINK_SPEED_IDLE) && 
                    (pDevice->link_speed[1] != LINK_SPEED_IDLE)) 
                ind = 1;
            else if ((pDevice->link_speed[0] != LINK_SPEED_IDLE) && 
                    (pDevice->link_speed[1] != LINK_SPEED_IDLE)) {
                //if both links are up, keep previously up link
                if(pDevice->link_speed[pDevice->link_index] != LINK_SPEED_IDLE) 
                    ind = pDevice->link_index ;
                else 
                    //mainboard link is primary
                    ind = 0;
            }
            speed = pDevice->link_speed[ind];

            //if active link has changed
            if ((pDevice->link_index != ind) || (speed != old_link_speed[ind])) {
                OmniTekEthDebug(ETH, KERN_INFO, 
                        "%s old port =%d new port =%d.\n",__func__,
                        pDevice->link_index,ind); 
                OmniTekEthDebug(ETH, KERN_INFO, 
                        "%s old speed =%d new speed =%d.\n",__func__,
                        old_link_speed[ind],speed); 

                pDevice->link_index = ind;
                OmniTekEthDebug(ETH,KERN_INFO,"Port to %d\n",ind);

                ReadHWValue(pExt,1,0x34);
                if(ind == 0)
                    WriteHWValue(pExt,1,0x34, 
                            ReadHWValue(pExt,1,0x34) & 0xFFFFFFFB);
                else
                    WriteHWValue(pExt,1,0x34, 
                            ReadHWValue(pExt,1,0x34) | 0x00000004);

                //mac needs to be reconfigured if speed or active link changes
                //TO DO add code to change MAC speed autoneg
                switch (speed) {
                case LINK_SPEED_1G:
                    xpi_mac_write(pExt, mo, 0x300+offset[ind], 0x94000032);
                    OmniTekEthDebug(ETH, KERN_INFO, 
                            "%s, link is gig, mac is 0x%x.\n", __func__, 
                            xpi_mac_read(pExt, mo, 0x300+offset[ind]));
                    break;
                case LINK_SPEED_100M:
                    xpi_mac_write(pExt, mo, 0x300+offset[ind], 0x54000032);
                    OmniTekEthDebug(ETH, KERN_INFO, 
                            "%s, link is 100, mac is 0x%x.\n", __func__, 
                            xpi_mac_read(pExt, mo, 0x300+offset[ind]));
                    break;
                case LINK_SPEED_10M:
                    xpi_mac_write(pExt, mo, 0x300+offset[ind], 0x14000032);
                    OmniTekEthDebug(ETH, KERN_INFO, 
                            "%s, link is 10, mac is 0x%x.\n", __func__, 
                            xpi_mac_read(pExt, mo, 0x300+offset[ind]));
                    break;
                default:
                    xpi_mac_write(pExt, mo, 0x300+offset[ind], 0x94000032);
                    OmniTekEthDebug(ETH, KERN_INFO, 
                            "%s, link is idle, mac is 0x%x.\n", __func__, 
                            xpi_mac_read(pExt, mo, 0x300+offset[ind]));
                    break;
                }
            }
            else
                OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
                        "%s, link change bypassed.\n",__func__);
        }
    }

    if (pDevice->upcount == 0 ) {
        OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
                "%s upcount expired in change \n",__func__);
        pDevice->upcount = RETRY_NETWORK ;
    }
    else
        pDevice->upcount--;
}

void OmniTek_Ethernet_poll(unsigned long arg)
{
    struct OmniTek_Ethernet_Device *pDevice = 
                    (struct OmniTek_Ethernet_Device *)arg;
    int result;
    bool status;

    OmniTek_Ethernet_get_link_speed(pDevice);

    result = OmniTek_Ethernet_get_link(pDevice->dev);

    status = result ? true : false;

    //printk ("%s link_speed=%d link=%d speed=%d\n", __func__, 
    //      link_speed,(int)result,pDevice->link_speed);
    //OmniTekEthDebug(ETHERNET,KERN_INFO,"Current link status is %s\n",
    //      status ? "UP" : "DOWN");
    if (status != pDevice->link_up) {
        pDevice->link_up = status;
        OmniTekEthDebug(ETH, KERN_INFO, "Status changed\n");
        if (status) {
            netif_carrier_on(pDevice->dev);
            OmniTekEthDebug(ETH, KERN_INFO, "OmniTek Ethernet Link Up\n");
            switch(pDevice->link_speed[pDevice->link_index]) {
            case LINK_SPEED_1G:
                printk("\t\tLink up - speed: 1G\n");
                break;
            case LINK_SPEED_100M:
                printk("\t\tLink up - speed: 100M\n");
                break;
            case LINK_SPEED_10M:
                printk("\t\tLink up - speed: 10M\n");
                break;
            default:
                printk("\t\tLink up - speed: unknown\n");
                break;
            }
        }
        else {
            netif_carrier_off(pDevice->dev);
            OmniTekEthDebug(ETH, KERN_ERR,"OmniTek Ethernet Link Down\n");
            printk("\t\tLink down\n");
        }
    }

    //Re-schedule the timer
    mod_timer(&(pDevice->poll_timer), jiffies + msecs_to_jiffies(1000));
}

static const struct ethtool_ops OmniTek_Ethernet_ethtool_ops = {
    .get_link      = OmniTek_Ethernet_get_link,
    .get_drvinfo   = OmniTek_Ethernet_get_drvinfo,
    .get_settings  = OmniTek_Ethernet_get_settings,
};

static struct net_device *OmniTek_Net_Device_Alloc(void)
{
    struct net_device *pNet_Device;

    // WRONG size = 8:  
    // pNet_Device = alloc_etherdev(sizeof(struct OmniTek_Ethernet_struct));
    // CORRECT: size = 360
    pNet_Device = alloc_etherdev(sizeof(struct OmniTek_Ethernet_Device));

    if (!pNet_Device) {
        OmniTekEthDebug(ETHERNET, KERN_ERR, 
            "Failed to allocate etherdev struct\n");
        return 0;
    }

    ether_setup(pNet_Device);
    pNet_Device->netdev_ops = &OmniTek_Ethernet_netdev_ops;
    pNet_Device->watchdog_timeo = msecs_to_jiffies(1000);
    SET_ETHTOOL_OPS(pNet_Device,&OmniTek_Ethernet_ethtool_ops); 
    memcpy(pNet_Device->dev_addr, "\0NETV0", ETH_ALEN);
    pNet_Device->features= 0;
    pNet_Device->flags |= IFF_BROADCAST | IFF_MULTICAST ;

    return pNet_Device;
}

static int OmniTek_Ethernet_Device_Register(struct net_device *pNet_Device)
{
    int result = 0;

    //Register the network device
    if ((result = register_netdev(pNet_Device))) {
        OmniTekEthDebug(ETH, KERN_ERR, 
                "Failed to register net device: %d\n", result);
        return result;
    }
    else
        OmniTekEthDebug(ETH, KERN_INFO, 
                "OmniTek Ethernet Device Registered\n");
#ifdef CONFIG_SYSFS 
    if ((result = OmniTek_Ethernet_init_sysfs(pNet_Device))) {
        OmniTekEthDebug(ETH, KERN_ERR,
                "Failed to add sysfs: %d\n", result);
        free_netdev(pNet_Device);
    }
    else
        OmniTekEthDebug(ETH, KERN_INFO, 
                "OmniTek Ethernet Device Added Sysfs\n");
#endif
    return result;
}

static int OmniTek_Ethernet_Device_Init(struct OmniTek_Ethernet_Device *pDevice,
                                        struct net_device *dev)
{

    int channel = 1;
    bool polling = false;

    memset(pDevice,0,sizeof(struct OmniTek_Ethernet_Device));

    pDevice->tx_restarting = false;
    INIT_WORK(&(pDevice->tx_restart_work), OmniTek_Ethernet_Tx_restart_work);
    INIT_WORK(&(pDevice->rx_restart_work), OmniTek_Ethernet_Rx_restart_work);

    if (!(pDevice->pExt = OmniTekGetExtension(0))) {
        OmniTekEthDebug(ETH, KERN_ERR, "Couldn't get omnitek extension\n");
        return -EINVAL;
    }

    //If a channel paramater is specified then make use of it, 
    //otherwise default...
    if (efdma_channel >= 0)
        channel = efdma_channel;
    else if (pDevice->pExt->pFPGA)
        channel = 1;
    else
        channel = 0;

    SET_NETDEV_DEV(dev, &(pDevice->pExt->Device->dev));

#ifdef USE_POLLED_EFDMA
    polling=true;
#endif

    if (efdma_channel_acquire(pDevice->pExt, channel, 
            &(pDevice->pEFDMA_Channel),polling)) {
        OmniTekEthDebug(ETH, KERN_ERR, "Couldn't create EFDMA channel\n");
        return -EINVAL;
    }

    // Set default link speed to 10G -- this will avoid a speed switch 
    // after startup
    pDevice->link_speed[0] = LINK_SPEED_IDLE;
    pDevice->link_speed[1] = LINK_SPEED_IDLE;
    pDevice->link_index = 0;
    pDevice->link_up = 0x0;
    pDevice->phy_status[0] = 0;
    pDevice->phy_status[1] = 0;
    pDevice->dev = dev;

    //OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s before OmniTek_phy_init %u.\n",
    //        __func__, jiffies_to_msecs(jiffies)) ;
    //OmniTek_phy_init(pDevice) ;
    //OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s after OmniTek_phy_init %u.\n", 
    //    __func__, jiffies_to_msecs(jiffies)) ;

    pDevice->upcount = 0 ;
    init_timer(&(pDevice->poll_timer));
    pDevice->poll_timer.expires = jiffies + msecs_to_jiffies(1000);
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s after poll_timer jiffies %u.\n",
        __func__, jiffies_to_msecs(jiffies)) ;
    pDevice->poll_timer.function = OmniTek_Ethernet_poll;
    pDevice->poll_timer.data = (unsigned long)pDevice;
    add_timer(&(pDevice->poll_timer));
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "%s after add_timer jiffies %u.\n",
        __func__, jiffies_to_msecs(jiffies)) ;

    INIT_LIST_HEAD(&(pDevice->Object));

    return 0;
}

static void OmniTek_Ethernet_Device_Delete(
    struct OmniTek_Ethernet_Device *pDevice) 
{
    struct net_device * pNet_Device = pDevice->dev;

    //TODO: any more cleanup here? 
#ifdef USE_POLLED_EFDMA
    netif_napi_del(&(pDevice->napi));
#endif
    efdma_channel_release(pDevice->pEFDMA_Channel);

    del_timer(&(pDevice->poll_timer));
#ifdef CONFIG_SYSFS
    OmniTek_Ethernet_remove_sysfs(pNet_Device);
#endif
}

struct net_device *OmniTek_Ethernet_get_device(int index)
{
    int count = 0;
    struct OmniTek_Ethernet_Device *pDevice;

    list_for_each_entry(pDevice,&(OmniTek_Ethernet.Devices),Object) {
        struct net_device * pNet_Device = pDevice->dev;
        if (count == index) 
            return pNet_Device;
    }
    return NULL;
}
EXPORT_SYMBOL(OmniTek_Ethernet_get_device);

static int __init OmniTek_Ethernet_init(void)
{
    struct net_device * pDevice;
    struct OmniTek_Ethernet_Device * pOmniTek_Net_Device;
    struct _OMNITEK_INTERFACE_EXTENSION * pExt = NULL;

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, "Starting %s, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH,KERN_INFO,"OmniTek_Ethernet loading\n");

    INIT_LIST_HEAD(&(OmniTek_Ethernet.Devices));

    //TODO: We should probably have some kind of 'probe' function that 
    //looks for ethernet devices, but for now it's hard coded
    pDevice = OmniTek_Net_Device_Alloc();
    if (!pDevice)
        return -EINVAL;
    //Read the required register offsets for the Barco register space
    pExt = OmniTekGetExtension(0);
    if (pExt == NULL) {
        OmniTekEthDebug(ETH, KERN_ERR, 
            "Couldn't get OmniTek Interface Extension\n");
        return -EINVAL;
    }
    else {
        OmniTekEthDebug(ETH, KERN_INFO, 
            "Got OmniTek Interface Extension @ %p\n",pExt);
    }

    pOmniTek_Net_Device = netdev_priv(pDevice);  
    pOmniTek_Net_Device->Barco_mdio_offset = ReadHWValue(pExt,1,10);

    OmniTekEthDebug(ETH,KERN_INFO, "Net Device %p %p\n",pDevice, 
        pOmniTek_Net_Device);
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, checking OmniTek_Ethernet_Device_Init, jiffies %u.\n", __func__, 
        jiffies_to_msecs(jiffies)) ;

    if (OmniTek_Ethernet_Device_Init(pOmniTek_Net_Device, pDevice)) {
        OmniTekEthDebug(ETH,KERN_ERR,
            "Error initialising OmniTek_Ethernet device\n");
        goto out_device;
    }

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, checking efdma_channel_set_rx_callback, jiffies %u.\n", __func__, 
        jiffies_to_msecs(jiffies)) ;

    if (efdma_channel_set_rx_callback(pOmniTek_Net_Device->pEFDMA_Channel, 
            &OmniTek_Ethernet_receive_func_ptr, pDevice)) {
        OmniTekEthDebug(ETH_VERBOSE,KERN_ERR,"Error setting RX callback\n");
        goto out_device;
    }

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, checking efdma_channel_set_tx_wake_func, jiffies %u.\n", __func__,
        jiffies_to_msecs(jiffies)) ;

    if (efdma_channel_set_tx_wake_func(pOmniTek_Net_Device->pEFDMA_Channel, 
            &OmniTek_Ethernet_tx_wake_func_ptr, pDevice)) {
        OmniTekEthDebug(ETH_VERBOSE,KERN_ERR,"Error setting TX callback\n");
        goto out_device;
    }

#ifdef USE_POLLED_EFDMA

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, checking efdma_channel_set_schedule_poll_callback, jiffies %u.\n",
        __func__, jiffies_to_msecs(jiffies)) ;

    if (efdma_channel_set_schedule_poll_callback(
            pOmniTek_Net_Device->pEFDMA_Channel, 
            &OmniTek_Ethernet_schedule_poll_ptr, pDevice)) {
        OmniTekEthDebug(ETH_VERBOSE, KERN_ERR,
            "Error setting Schedule Poll callback\n");
        goto out_device;
    }
#endif

    //Enable packet dropping to prevent ringbuffer overflow
    //efdma_channel_set_throttle_drop(pOmniTek_Net_Device->pEFDMA_Channel, true);
    pOmniTek_Net_Device->dev = pDevice;

    list_add_tail(&(pOmniTek_Net_Device->Object), &(OmniTek_Ethernet.Devices));

    OmniTekEthDebug(ETH, KERN_INFO, 
        "%s, checking OmniTek_Ethernet_Device_Register, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;

    if (OmniTek_Ethernet_Device_Register(pDevice)) {
        OmniTekEthDebug(ETH,KERN_ERR,"Failed to register net device\n");
        free_netdev(pDevice);
        goto out_delete;
    }

    OmniTekEthDebug(ETH, KERN_INFO, 
        "%s, finished OmniTek_Ethernet_Device_Register, jiffies %u.\n", 
        __func__, jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH,KERN_INFO,"Device registered and added to list\n");

#ifdef USE_POLLED_EFDMA
    //Setup NAPI context...
    OmniTekEthDebug(ETH,KERN_INFO,
        "Adding NAPI at %p\n",&(pOmniTek_Net_Device->napi));
    netif_napi_add(pDevice, &(pOmniTek_Net_Device->napi), 
        OmniTek_Ethernet_napi_poll, 128);
    OmniTekEthDebug(ETH,KERN_INFO,
        "Napi Struct has: device %p, gro_list %p, skb %p, poll func %p\n", 
        pOmniTek_Net_Device->napi.dev, pOmniTek_Net_Device->napi.gro_list, 
        pOmniTek_Net_Device->napi.skb, pOmniTek_Net_Device->napi.poll);
#endif

    //Start EFDMA Channels for device
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, starting efdma_channel_start, jiffies %u.\n", __func__, 
        jiffies_to_msecs(jiffies)) ;
    OmniTekEthDebug(ETH,KERN_INFO,"Starting EFDMA Channel\n");

    efdma_channel_start(pOmniTek_Net_Device->pEFDMA_Channel);

    OmniTekEthDebug(ETH,KERN_INFO,"EFDMA Channel started\n");
    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "%s, finished efdma_channel_start, jiffies %u.\n", __func__, 
        jiffies_to_msecs(jiffies)) ;

#ifndef CONFIG_MACH_NETVIZ
    //TODO: If there's no FPGA capability then we have Barco firmware 
    //so we don't need to switch.  This is a bit of a hack
    if (pOmniTek_Net_Device->pExt->pFPGA) {
        WriteRegValue(pOmniTek_Net_Device->pExt,
                pOmniTek_Net_Device->pExt->pFPGA->General.Bar,
                pOmniTek_Net_Device->pExt->pFPGA->General.RegisterOffset + 2,
                0x1);
    }
#else
    //TODO: If there's no FPGA capability then we have Barco firmware 
    //so we don't need to switch.  This is a bit of a hack
    if (pOmniTek_Net_Device->pExt->pFPGA) {
        WriteRegValue(pOmniTek_Net_Device->pExt,
                pOmniTek_Net_Device->pExt->pFPGA->General.Bar,
                pOmniTek_Net_Device->pExt->pFPGA->General.RegisterOffset + 2,
                0x11);  //NB: Set ethernet switch to enable ethernet to ARM
    }
#endif

    //TO DO - add init code from test.sh? PGUP

    OmniTekEthDebug(ETH,KERN_INFO,"OmniTek_Ethernet loaded\n");

    OmniTekEthDebug(ETH_VERBOSE, KERN_INFO, 
        "Exiting %s, jiffies %u.\n", __func__, jiffies_to_msecs(jiffies)) ;

    return 0;

out_delete:  
    list_del(&(pOmniTek_Net_Device->Object));
    OmniTek_Ethernet_Device_Delete(pOmniTek_Net_Device);

out_device:
    free_netdev(pDevice);
    return -EINVAL;
}

static void __exit OmniTek_Ethernet_exit(void)
{
    struct OmniTek_Ethernet_Device *pDevice, *pDevice_temp;

    OmniTekEthDebug(ETH,KERN_INFO,"OmniTek_Ethernet unloading\n");  

    //Cleanup...  
    list_for_each_entry_safe(pDevice,pDevice_temp,&(OmniTek_Ethernet.Devices),
            Object) {
        //TODO: Is this correct?
        struct net_device *pNet_Device = pDevice->dev;
        OmniTekEthDebug(ETH,KERN_INFO,
            "Deleting net device @ %p, net_device @ %p\n",pDevice,pNet_Device);
        unregister_netdev(pNet_Device);
        list_del(&(pDevice->Object));
        OmniTek_Ethernet_Device_Delete(pDevice);
        free_netdev(pNet_Device);
    }

    OmniTekEthDebug(ETH,KERN_INFO,"OmniTek_Ethernet unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(OmniTek_Ethernet_init);
module_exit(OmniTek_Ethernet_exit);
