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
 * Image Processing Techniques makes no guarantee or representations regarding the use of, 
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
#include <linux/workqueue.h>
#include <linux/types.h>	/* size_t */
#include <linux/errno.h>	/* error codes */
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/pci.h>
#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <asm/atomic.h>
#include <linux/slab.h>


#include "OmniTekEFDMA.h"
#include "OmniTekEFDMA_Debug.h"

struct efdma_module_data efdma_data;

static int efdma_channel_rx_restart(struct efdma_channel * pEFDMA_Channel);


void efdma_channel_dma_process(struct efdma_ringbuffer * pEFDMA_RingBuffer)
{
  int amt = 0;

  if (!pEFDMA_RingBuffer)
  {
    OmniTekDebug(EFDMA,KERN_ERR,"RingBuffer Deleted?\n");
    dump_stack();
    return;
  }
  

  //Call ringbuffer_xferred with the amount waiting.
  amt = atomic_xchg(&(pEFDMA_RingBuffer->dma_waiting),amt);

  OmniTekDebug(EFDMA,KERN_INFO,"Channel %p dma_work transferred %d bytes\n",pEFDMA_RingBuffer,amt);

  ringbuffer_xferred(&(pEFDMA_RingBuffer->ringbuffer),amt);

}

void efdma_channel_dma_work(struct work_struct * work)
{
  //Work item to call ringbuffer_xferred on the supplied ringbuffer.  This might trigger the slices being
  //re-queued or all sorts of other stuff, so it must be able to sleep

//  struct efdma_ringbuffer * pEFDMA_RingBuffer = (struct efdma_ringbuffer *)arg;
  struct efdma_ringbuffer * pEFDMA_RingBuffer = container_of(work,struct efdma_ringbuffer,dma_work);
  efdma_channel_dma_process(pEFDMA_RingBuffer);

}

void efdma_channel_process_func(struct ringbuffer * pRingBuffer, ssize_t amt)
{
  //The process func tells the hardware that an amount of data is available for transmission
  //This is just a register write to the FDMA channel bytes_available 

  OmniTekDebug(EFDMA,KERN_INFO,"Channel %p process_func writing 0x%zx / %zd bytes available\n",pRingBuffer,amt,amt);
  DmaChannelWriteBytesAvail(pRingBuffer->pChannel,amt);  

}


void efdma_channel_rx_event_func(struct _Resource * pChannel, void * arg)
{
  //The event func is called as part of the DMA channel interrupt handler
  //We should read the amount of data transferred here and call xFerred
  //for the appropriate ringbuffer

  //NB: This will be called by the ISR (or probably the interrupt thread)
  //it probably shouldn't go to sleep...

  //We should probably schedule the dma_xferred function with the amount read from the hardware
  //or keep an atomic count which will effectively coalesce the received data...

  //This will be called with the pChannel SpinLock held...
  
  size_t amt;
  struct efdma_ringbuffer * pEFDMA_RingBuffer = (struct efdma_ringbuffer *)arg;

  if (unlikely(!arg))
  {
    OmniTekDebug(EFDMA,KERN_ERR,"EFDMA Event function called but no arg value, not set up correctly?\n");
    dump_stack();
    return;
  }

  if (unlikely(!pChannel))
  {
    OmniTekDebug(EFDMA,KERN_ERR,"EFDMA Event function called but no DMA Channel, not set up correctly?\n");
    dump_stack();
    return;
  }


  amt = DmaChannelGetBytesAvail(pChannel);

  if (unlikely(amt == 0))
  {
	  OmniTekDebug(EFDMA,KERN_ERR,"Channel %p no. %d got event with zero size transferred!\n",pChannel, pChannel->u.DmaChannel.Index);
  }

  if (unlikely(amt < 0))
  {
	  OmniTekDebug(EFDMA,KERN_ERR,"Channel %p no. %d got event with negative size transferred!\n",pChannel, pChannel->u.DmaChannel.Index);
  }


  atomic_add(amt,&(pEFDMA_RingBuffer->dma_waiting));
  OmniTekDebug(EFDMA,KERN_INFO,"Channel %p no. %d got event, queueing work\n",pEFDMA_RingBuffer, pChannel->u.DmaChannel.Index);

  //For polling versions we should probably just do the event work here
  if (!queue_work(efdma_data.work_queue,&(pEFDMA_RingBuffer->dma_work)))
    OmniTekDebug(EFDMA,KERN_INFO,"Didn't queue event work!\n");
  else
    OmniTekDebug(EFDMA,KERN_INFO,"Event work queued!\n");


}

void efdma_channel_tx_event_func(struct _Resource * pChannel, void * arg)
{
  OmniTekDebug(EFDMA,KERN_ERR,"Channel %p no. %d Tx event function called - this shouldn't occur...\n", pChannel, pChannel->u.DmaChannel.Index);
}

void efdma_channel_rx_schedule_poll_func(struct _Resource * pChannel, void * arg)
{
  struct efdma_channel * pEFDMA_Channel = (struct efdma_channel *)arg;

  //So... whatever lies above us (ethernet) needs to provide a callback so we can tell it to poll for data
  if (pEFDMA_Channel->poll_func)
  {
    OmniTekDebug(EFDMA,KERN_INFO, "Channel %p no. %d Calling schedule poll callback!\n", pChannel, pChannel->u.DmaChannel.Index);
    (*(pEFDMA_Channel->poll_func))(pEFDMA_Channel,pEFDMA_Channel->rx_priv);  
  }
  else
  {
    OmniTekDebug(EFDMA,KERN_ERR, "Channel %p no. %d No polling callback!\n", pChannel, pChannel->u.DmaChannel.Index);
  }

}

void efdma_channel_rx_poll(struct efdma_ringbuffer * pEFDMA_RingBuffer)
{

  //Polling EFDMA - read the bytes available from the channel and store them.
  size_t bytes_avail = DmaChannelReadBytesAvail(pEFDMA_RingBuffer->ringbuffer.pChannel);
  atomic_add(bytes_avail, &(pEFDMA_RingBuffer->dma_waiting));
  //Top up bytes unhandled, to fix statistics messages...
  atomic_add(bytes_avail, &(pEFDMA_RingBuffer->ringbuffer.pChannel->u.DmaChannel.nBytesUnhandled));

  //Call process function which triggers ringbuffer update
  efdma_channel_dma_process(pEFDMA_RingBuffer);
  //The polling function then needs to start receiving individual packets...
  //So what we can do is modify the wake function to take an optional packet quota...
   //and be called by the poll function instead
}
EXPORT_SYMBOL(efdma_channel_rx_poll);

void efdma_channel_rx_interrupt_enable(struct efdma_channel * pEFDMA_Channel, bool enable)
{
  DMAChanEventInterruptEnable(pEFDMA_Channel->pReceive_Channel,enable,pEFDMA_Channel->pReceive_Channel->u.DmaChannel.FDMA_Write);
}
EXPORT_SYMBOL(efdma_channel_rx_interrupt_enable);


int efdma_channel_receive_packets(struct efdma_ringbuffer * pEFDMA_RingBuffer, int nPackets)
{
  //This is called when a packet has been received
  //if the nPackets is -1 then we receive all packets, otherwise we presume a quota (e.g. from NAPI poll)
  //and will only 'receive' the specified number of packets.
  //If we run out of data to process then we return 0, otherwise return 1

  //We need to get our efdma channel back:
  struct ringbuffer * pRingBuffer = &(pEFDMA_RingBuffer->ringbuffer);
  struct efdma_channel * pEFDMA_Channel = container_of(pEFDMA_RingBuffer,struct efdma_channel, receive);

  if (pEFDMA_Channel->rx_func)
  {

    //So what we do is:
    // * work out total available
    // * request packet size
    // * dump packet header (this should probably be part of the packet_size function)
    // * call rx_func with packet_size, rx_func will call ringbuffer_receive to retrieve data directly to its own buffer
    // * repeat until no more packets (received + headers == available)

           
    ssize_t avail = ringbuffer_avail(pRingBuffer);
    ssize_t packets_total = 0;
    ssize_t padding_recv = 0;
    int packets_received = 0;
    char padding[8];


    if (avail < 0)
    {
      OmniTekDebug(EFDMA,KERN_ERR,"Error occurred receiving packet %zd\n",avail);
      efdma_channel_rx_restart(pEFDMA_Channel);
   }


    while ((packets_total < avail) && (avail > 0) )
    {
      int result;
      ssize_t packet_handled = 0;
      ssize_t packet_size = ringbuffer_next_packet_size(pRingBuffer);
      ssize_t padding_size = (8 - (packet_size % 8)) % 8;
      packets_total += sizeof(u64); //Add header amount to total
    
      packet_handled += sizeof(u64);

      if ((packet_size < 0) || (packet_size + packets_total > avail))
      {
        OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error getting next packet size: returned %zd, packets_total  %zd, avail %zd\n",packet_size, packets_total, avail);
        
        //Pass error onto receiving function
        if (packet_size < 0)
          result = (*(pEFDMA_Channel->rx_func))(pEFDMA_Channel,pEFDMA_Channel->rx_priv,packet_size);

        break;
      }

      packets_received++;
	  // Do not process packets with size 0 that are used for interrupt generation
      if (packet_size > 0)
      {
	  if ((result = (*(pEFDMA_Channel->rx_func))(pEFDMA_Channel,pEFDMA_Channel->rx_priv,packet_size)))
	  {
	    OmniTekDebug(EFDMA,KERN_ERR,"receive function for channel returned error\n");
	    break;
	  }
      }

      OmniTekDebug(EFDMA,KERN_INFO,"receive packet processed\n");

      packets_total += packet_size;
      packet_handled += packet_size;

      //If the size wasn't 64 bit aligned, then we need to dump some padding...
      if (padding_size)
      {
	OmniTekDebug(EFDMA,KERN_INFO,"Stripping %zd bytes padding from packet\n",padding_size);
	padding_recv = ringbuffer_receive(pRingBuffer,padding_size,padding);
	if (padding_recv != padding_size)
	{
	  OmniTekDebug(EFDMA,KERN_ERR,"Error stripping padding from packet\n");
	  packets_total = padding_recv;
	  packet_handled += padding_recv;
	}
	else
	{
	  packets_total += padding_recv;
	  packet_handled += padding_recv;
	}
      }


      OmniTekDebug(EFDMA, KERN_INFO, "Total data consumed for packet was %zd / %08x long\n",packet_handled,(unsigned int)packet_handled);

      if ((nPackets != -1) && (packets_received == nPackets))
      {
	//drop out of loop if we have hit our packet quota
	break;
      }
      else if ((packets_total == avail) && (nPackets != -1))    //Only if we are polling...
      {
        size_t new_avail;
        //If we've used up all the data then we should re-enable interrupts
        //and check to see if there is any more data awaiting...
        //The ordering here is important...we must have re-enabled interrupts before our final check for data...                    
        //Re-enable interrupts - just to be safe...
        efdma_channel_rx_interrupt_enable(pEFDMA_Channel, true);
        //Perform a poll operation which should update the ringbuffer if there has been any subsequent data available
        efdma_channel_rx_poll(pEFDMA_RingBuffer);
        //Grab any data that has occured since we started
        new_avail = ringbuffer_avail(pRingBuffer);         

        if (new_avail < 0)
        {
          OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error occurred receiving packet %zd\n",new_avail);
		  efdma_channel_rx_restart(pEFDMA_Channel);
		 break;
    }
        avail += new_avail;

        //This should be 'safe'.  If an interrupt occurs between the interrupt enable and the poll, the NAPI won't be re-scheduled because
        //it is currently running, _but_ we will catch the data when we poll.  This should be a rare occurence, but there is an edge case
        //where a single packet is received after the initial poll above, but before the interrupts are re-enabled and hence there is
        //no indication that it has been received to the driver, so the above is necesary to ensure that this sequence of events is
        //correctly covered

      }

    }

    //Otherwise if the size doesn't match then something got mis-aligned - unless we received the quota but still have data...
    //TODO: Will just dump data to attempt to re-align buffer...
    if ((packets_total < avail) && (avail > 0) && (packets_total > 0) && (nPackets != packets_received))
    {
      ssize_t dumped = 0;        
      OmniTekDebug(EFDMA,KERN_ERR,"Error receiving packets: total received did not match amount available! %zd received, %zd available\n",
                    packets_total,avail); 

      if ((dumped = ringbuffer_dump(pRingBuffer, avail-packets_total)) < 0)
      {
        OmniTekDebug(EFDMA,KERN_ERR,"Error dumping ringbuffer data, returned %zd\n",dumped);
        return -1;
      }

      packets_total += dumped;
      if (packets_total != avail)
      {
        OmniTekDebug(EFDMA,KERN_ERR,"Still didn't manage to dump correct amount - total %zd expected %zd!\n", packets_total, avail);
        return -1;
      }

      OmniTekDebug(EFDMA,KERN_ERR,"Ringbuffer data dumped\n");
    }
  
  
    //Possibly un-throttle the channel
    if (pRingBuffer == NULL)
      OmniTekDebug(EFDMA, KERN_INFO, "Ringbuffer is NULL\n");

    if (pRingBuffer->pChannel == NULL)
      OmniTekDebug(EFDMA, KERN_INFO, "Ringbuffer->Channel is NULL\n");


    if (pRingBuffer->pChannel->u.DmaChannel.FDMA_Throttling == true)
    {
      bool result = DMAChannelUnThrottle(pRingBuffer->pChannel,4);
      //OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "Processed %zd bytes\n",packets_total);

      if (result)
        OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel unthrottled\n");
      else
        OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel remains throttled\n");
    }


    OmniTekDebug(EFDMA,KERN_INFO,"Received %d / %d packets\n",packets_received,nPackets);
    if (nPackets == packets_received)
    {
      //packet quota received, if there is still data we need to indicate such to the polling function
      if (packets_total < avail)
      {
        OmniTekDebug(EFDMA,KERN_INFO,"Packet quota filled, have more data available\n");
        return 1;
      }
      efdma_channel_rx_interrupt_enable(pEFDMA_Channel, true);
      OmniTekDebug(EFDMA,KERN_INFO,"Used up all data (and packets)\n");
      return 0;
    }
    else    //Didn't use all the packets, did we run out of data?
    {

      if (packets_total == avail)
      {
        efdma_channel_rx_interrupt_enable(pEFDMA_Channel, true);
        OmniTekDebug(EFDMA,KERN_INFO,"Used up all data (packet quota remaining)\n");
        return 0;
      }
    
      OmniTekDebug(EFDMA,KERN_INFO,"Didn't use packet quota or data - error?\n");
      return 1; //Come back later?
    }

  }
  else
  {
    //Dummy receive process - take the packet from the ringbuffer and drop it.
    ssize_t amt =  ringbuffer_avail(pRingBuffer);
    char * buffer =  kmalloc(amt,GFP_KERNEL);

    if (amt < 0)
      OmniTekDebug(EFDMA,KERN_ERR,"Error occurred receiving packet (dummy) %zd\n",amt);
    else
      ringbuffer_receive(pRingBuffer,amt,buffer);

    OmniTekDebug(EFDMA, KERN_INFO,"Dummy Received packet of size: %zd\n",amt);

    kfree(buffer);
    return -1;
  }
  return -1;
}

EXPORT_SYMBOL(efdma_channel_receive_packets);


void efdma_channel_wake_func(struct ringbuffer * pRingBuffer)
{
  efdma_channel_receive_packets(container_of(pRingBuffer,struct efdma_ringbuffer,ringbuffer),-1);
}

int efdma_channel_set_rx_callback(struct efdma_channel * pEFDMA_Channel, efdma_receive_func *  rx_func, void * priv)
{
  if (!pEFDMA_Channel)
    return -EINVAL;

  //TODO: Do we need to lock this?

  pEFDMA_Channel->rx_func = rx_func;
  pEFDMA_Channel->rx_priv = priv;

  OmniTekDebug(EFDMA, KERN_INFO, "Rx Callback function set\n");

  return 0;
}

EXPORT_SYMBOL(efdma_channel_set_rx_callback);

int efdma_channel_set_tx_wake_func(struct efdma_channel * pEFDMA_Channel, efdma_tx_wake_func * tx_func, void * priv)
{
  if (!pEFDMA_Channel)
    return -EINVAL;

  //TODO: Do we need to lock this?

  pEFDMA_Channel->tx_func = tx_func;
  pEFDMA_Channel->tx_priv = priv;

  OmniTekDebug(EFDMA, KERN_INFO, "TX Wake function set\n");

  return 0;
}  

EXPORT_SYMBOL(efdma_channel_set_tx_wake_func);

int efdma_channel_set_schedule_poll_callback(struct efdma_channel * pEFDMA_Channel, efdma_schedule_poll_func *  poll_func, void * priv)
{
  if (!pEFDMA_Channel)
    return -EINVAL;

  //TODO: Do we need to lock this?

  pEFDMA_Channel->poll_func = poll_func;
  pEFDMA_Channel->poll_priv = priv;
  pEFDMA_Channel->rx_poll = (poll_func != NULL);

  OmniTekDebug(EFDMA, KERN_INFO, "Schedule Poll Callback function set\n");

  return 0;
}

EXPORT_SYMBOL(efdma_channel_set_schedule_poll_callback);


#if 0
int efdma_channel_set_throttle_drop(struct efdma_channel * pEFDMA_Channel, bool enable)
{
  OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "%s throttling for channel %p\n",(enable ? "Enabled" : "Disabled"),pEFDMA_Channel);
  pEFDMA_Channel->receive.ringbuffer.rx_throttle = enable;
  return 0;

}
EXPORT_SYMBOL(efdma_channel_set_throttle_drop);
#endif

ssize_t efdma_channel_transmit_packet(struct efdma_channel * pEFDMA_Channel, char * pBuffer, size_t length, unsigned char key, size_t min_length)
{
  ssize_t transmitted = ringbuffer_transmit_packet(&(pEFDMA_Channel->transmit.ringbuffer), length, pBuffer, key, min_length);
  if (transmitted == length)
    return 0;


  OmniTekDebug(EFDMA, KERN_INFO, "Transmitted packet size did not match requested size: %zd, transmitted: %zd!\n",length,transmitted);
  if (transmitted < 0) return transmitted;

  return -EINVAL;  

}
EXPORT_SYMBOL(efdma_channel_transmit_packet);

ssize_t efdma_channel_transmit_space(struct efdma_channel * pEFDMA_Channel)
{
  return ringbuffer_transmit_space(&(pEFDMA_Channel->transmit.ringbuffer));
}
EXPORT_SYMBOL(efdma_channel_transmit_space);

ssize_t efdma_channel_receive_packet(struct efdma_channel * pEFDMA_Channel, char * pBuffer, size_t length)
{
  int result = ringbuffer_receive(&(pEFDMA_Channel->receive.ringbuffer),length,pBuffer);
  //TODO: Can remove this later, once we know it is ok.
  OmniTekDebug(EFDMA, KERN_INFO, "Packet received: Remaining unhandled %d bytes\n",atomic_read(&(pEFDMA_Channel->receive.ringbuffer.pChannel->u.DmaChannel.nBytesUnhandled)));
  return result;
}
EXPORT_SYMBOL(efdma_channel_receive_packet);


static int efdma_channel_init_rx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;

  DmaChannelSetAutoStart(pEFDMA_Channel->pReceive_Channel,false);
  DmaChannelSetThrottling(pEFDMA_Channel->pReceive_Channel,true);

  result = ringbuffer_init(
            &(pEFDMA_Channel->receive.ringbuffer),
            8,
			//1<<18,  //Gives (1k * 256) * 8 = 2MB buffers...
			1<<16, // pci_alloc_consistent() fails ....
			false,
            pEFDMA_Channel->pReceive_Channel,
            efdma_channel_process_func,
            efdma_channel_wake_func
          );

  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't Initialize receive ringbuffer.\n");
    return -ERESTARTSYS;
  }


  init_waitqueue_head(&(pEFDMA_Channel->receive.wait_queue));
  INIT_WORK(&(pEFDMA_Channel->receive.dma_work),efdma_channel_dma_work);  
  atomic_set(&(pEFDMA_Channel->receive.dma_waiting),0);

  if (pEFDMA_Channel->rx_poll == false)
  {
  result = DmaChannelSetEventFunc(
    pEFDMA_Channel->pReceive_Channel,
    efdma_channel_rx_event_func,
    &(pEFDMA_Channel->receive.ringbuffer)
  );

  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't set read FDMA channel event callback function.\n");
    return -ERESTARTSYS;
  }
  }
  else
#ifndef USE_POLLED_EFDMA
    OmniTekDebug(EFDMA, KERN_ERR,"Polling mode set but polling not compiled into driver?\n");
#else
  {
  result = DmaChannelSetPollFunc(
    pEFDMA_Channel->pReceive_Channel,
    efdma_channel_rx_schedule_poll_func,
    pEFDMA_Channel
  );

  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't set read FDMA channel schedule poll callback function.\n");
    return -ERESTARTSYS;
  }
  }

#endif
  
  //Make sure everything is zeroed before starting channel up  
  //Clear pending data...
  atomic_xchg(&(pEFDMA_Channel->receive.dma_waiting),0);
  //Get any bytes remaining from channel to clear
  DmaChannelGetBytesAvail(pEFDMA_Channel->pReceive_Channel);

  OmniTekDebug(EFDMA, KERN_INFO,"Receive Ringbuffer initialized.\n");
  
  return result;

}


static int efdma_channel_init_tx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;
  
  //TODO: We don't have a 'wake' func for the transmit.  This means the transmit code is slightly blind as to
  //whether something has actually gone out over the wire.

  DmaChannelSetAutoStart(pEFDMA_Channel->pTransmit_Channel,false);

  result = ringbuffer_init(
              &(pEFDMA_Channel->transmit.ringbuffer),
              8,
			  1<<16,
			  true,
              pEFDMA_Channel->pTransmit_Channel,
              efdma_channel_process_func,
              NULL //Wake func is only for receive, so not really relevant here...

            );
  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't Initialize transmit ringbuffer.\n");
    return -ERESTARTSYS;
  }
  

  init_waitqueue_head(&(pEFDMA_Channel->transmit.wait_queue));
  INIT_WORK(&(pEFDMA_Channel->transmit.dma_work),efdma_channel_dma_work);
  atomic_set(&(pEFDMA_Channel->transmit.dma_waiting),0);


  //Set the event func callbacks for the channel
  
//TX Channels don't do events... but we need to enable event mode
  result = DmaChannelSetEventFunc(
    pEFDMA_Channel->pTransmit_Channel,
    efdma_channel_tx_event_func,
    &(pEFDMA_Channel->transmit.ringbuffer)
  );

  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't set write FDMA channel event callback function.\n");
    return -ERESTARTSYS;
  }

  //Start the DMA Channels
  OmniTekDebug(EFDMA, KERN_INFO,"Transmit Ringbuffer initialized.\n");

  return result;

}

static int efdma_channel_stop_rx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;

  DMAChannelStop(pEFDMA_Channel->pReceive_Channel);
  return result;
}

static int efdma_channel_start_rx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;

  result = DMAChannelStart(pEFDMA_Channel->pReceive_Channel);
  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Error starting receive EFDMA Channel.\n");
  }
  return result;
}


static int efdma_channel_stop_tx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;

  DMAChannelStop(pEFDMA_Channel->pTransmit_Channel);
  return result;
}


static int efdma_channel_start_tx(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;
  
  result = DMAChannelStart(pEFDMA_Channel->pTransmit_Channel);

  if (result)
  {
	OmniTekDebug(EFDMA, KERN_ERR,"Error starting transmit EFDMA Channel.\n");
  }
  return result;
}

static int efdma_channel_start(struct efdma_channel * pEFDMA_Channel)
{
  int result;

  result = efdma_channel_start_rx(pEFDMA_Channel);

  if (result)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"EFDMA Receive Ringbuffer failed to start.\n");
    return result;
  }

  result = efdma_channel_start_tx(pEFDMA_Channel);

  if (result)
    OmniTekDebug(EFDMA, KERN_ERR,"EFDMA Transmit Ringbuffer failed to start.\n");
  else   
    OmniTekDebug(EFDMA, KERN_INFO,"EFDMA Transmit and Receive Ringbuffers started.\n");

  return result;


}

EXPORT_SYMBOL(efdma_channel_start);

static int efdma_channel_init(struct efdma_channel * pEFDMA_Channel,struct _OMNITEK_INTERFACE_EXTENSION * pExt, int fdma_num, bool polling)
{

  
  pEFDMA_Channel->pExt = pExt;

  if (!pEFDMA_Channel->pExt)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't get OmniTek device extension.\n");
    return -ERESTARTSYS;
  }
  else
  {
    OmniTekDebug(EFDMA, KERN_INFO,"Got device extension %p\n",pEFDMA_Channel->pExt);
  }

  //Initialise this channel
  INIT_LIST_HEAD(&(pEFDMA_Channel->Object));  

  //Get a suitable FDMA Transmit (hard coded for now...)
  //TODO: Exclusive access to resources etc?  Need to use the resource reference_count and locked.  We only use DMA channels so we
  //      can probably modify the resourceacquire function to suit...
  pEFDMA_Channel->pTransmit_Channel = DmaChannelFind(pEFDMA_Channel->pExt, fdma_num | DMA_CHANNEL_FDMA | DMA_CHANNEL_WRITE);
  pEFDMA_Channel->pReceive_Channel  = DmaChannelFind(pEFDMA_Channel->pExt, fdma_num | DMA_CHANNEL_FDMA | DMA_CHANNEL_READ);

  if (!pEFDMA_Channel->pTransmit_Channel)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't get FDMA Write channel for EFDMA.\n");
    return -ERESTARTSYS;
  }
  else
  {
    OmniTekDebug(EFDMA, KERN_INFO,"Got EFDMA Transmit channel %p - %s, pExt %p.\n",pEFDMA_Channel->pTransmit_Channel, 
                                                                                   pEFDMA_Channel->pTransmit_Channel->u.DmaChannel.Desc,
                                                                                   pEFDMA_Channel->pTransmit_Channel->pExt);
    OmniTekDebug(EFDMA, KERN_INFO, "sizeof Resource is: %zd\n",sizeof(struct _Resource));
  }

  if (!pEFDMA_Channel->pReceive_Channel)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't get FDMA Read channel for EFDMA.\n");
    return -ERESTARTSYS;
  }
  else
  {
    OmniTekDebug(EFDMA, KERN_INFO,"Got EFDMA Receive channel %p - %s, pExt %p.\n",pEFDMA_Channel->pReceive_Channel, 
                                                                                  pEFDMA_Channel->pReceive_Channel->u.DmaChannel.Desc,
                                                                                  pEFDMA_Channel->pReceive_Channel->pExt);
  }
    
  pEFDMA_Channel->rx_poll = polling;
    
  //We have FDMA channels for both ring buffers, so now we need to initialize them...
  efdma_channel_init_tx(pEFDMA_Channel);
  efdma_channel_init_rx(pEFDMA_Channel);

  return 0;
}

//Force restart of the channel - should never be necessary...
static int efdma_channel_rx_restart(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;
  ssize_t ringbuffer_slice_size = 0;

  // In order to restart the RX channel we must stop the ringbuffer and re-init it
  // Disable all interrupts on this channel
  efdma_channel_rx_interrupt_enable(pEFDMA_Channel, false);

  // Stop the DMA hardware
  result = efdma_channel_stop_rx(pEFDMA_Channel);
  if (result)
  {
	 OmniTekDebug(EFDMA, KERN_INFO, "Receive Ringbuffer failed to stop DMA with result %d\n",result);
	 return result;
  }

  //Delete the event function - this also disables event interrupts on this channel
  result = DmaChannelSetEventFunc(
		pEFDMA_Channel->pReceive_Channel,
		NULL,
		NULL
  );

  //If we've been called from the work queue then we can't flush the work queue.
  if (work_pending(&(pEFDMA_Channel->receive.dma_work)))
	cancel_work_sync(&(pEFDMA_Channel->receive.dma_work));
 // flush_workqueue(efdma_data.work_queue);

  //Get any bytes remaining from channel to clear
  DmaChannelGetBytesAvail(pEFDMA_Channel->pReceive_Channel);
  //Clear pending data...
  atomic_set(&(pEFDMA_Channel->receive.dma_waiting),0);

  // Do not recreate the ringbuffer - re-init the slices
  ringbuffer_slice_size = pEFDMA_Channel->receive.ringbuffer.slices[0]->size;
  ringbuffer_cancel(&(pEFDMA_Channel->receive.ringbuffer));
  ringbuffer_slice_init(&(pEFDMA_Channel->receive.ringbuffer), ringbuffer_slice_size);

  //Ready to restart the channel...
  init_waitqueue_head(&(pEFDMA_Channel->receive.wait_queue));
  PREPARE_WORK(&(pEFDMA_Channel->receive.dma_work),efdma_channel_dma_work);

  OmniTekDebug(EFDMA, KERN_INFO,"Receive Ringbuffer initialized.\n");

  if (pEFDMA_Channel->rx_poll == false)
  {
	  result = DmaChannelSetEventFunc(
		pEFDMA_Channel->pReceive_Channel,
		efdma_channel_rx_event_func,
		&(pEFDMA_Channel->receive.ringbuffer));

	  if (result)
	  {
		OmniTekDebug(EFDMA, KERN_ERR,"Couldn't set read FDMA channel event callback function.\n");
		return -ERESTARTSYS;
	  }
  }
  else
#ifndef USE_POLLED_EFDMA
	OmniTekDebug(EFDMA, KERN_ERR,"Polling mode set but polling not compiled into driver?\n");
#else
  {
	  result = DmaChannelSetPollFunc(
		pEFDMA_Channel->pReceive_Channel,
		efdma_channel_rx_schedule_poll_func,
		pEFDMA_Channel
	  );

	  if (result)
	  {
		OmniTekDebug(EFDMA, KERN_ERR,"Couldn't set read FDMA channel schedule poll callback function.\n");
		return -ERESTARTSYS;
	  }
  }

#endif

  efdma_channel_rx_interrupt_enable(pEFDMA_Channel, true);

  result = efdma_channel_start_rx(pEFDMA_Channel);    
  if (result)
  {
    OmniTekDebug(EFDMA, KERN_INFO, "Receive Ringbuffer failed to re-start with result %d\n",result);
    return result;
  }

  OmniTekDebug(EFDMA, KERN_INFO, "Receive Ringbuffer re-started with result %d\n",result);
  return result;

}
EXPORT_SYMBOL(efdma_channel_rx_restart);

static int efdma_channel_tx_restart(struct efdma_channel * pEFDMA_Channel)
{
  int result = 0;
  efdma_tx_wake_func * tx_func = pEFDMA_Channel->tx_func;
  void * priv = pEFDMA_Channel->tx_priv;
  ssize_t ringbuffer_slice_size = 0;

  // In order to restart the TX channel we must stop the ringbuffer and re-init it
  // Stop the DMA hardware
  result = efdma_channel_stop_tx(pEFDMA_Channel);
  if (result)
  {
	OmniTekDebug(EFDMA, KERN_INFO, "Transmit Ringbuffer failed to stop DMA with result %d\n",result);
	return result;
  }

  // Delete the event function - this also disables event interrupts on this channel
  result = DmaChannelSetEventFunc(
    pEFDMA_Channel->pTransmit_Channel,
    NULL,
    NULL
  );

  // Do not recreate the ringbuffer - re-init the slices
  ringbuffer_slice_size = pEFDMA_Channel->transmit.ringbuffer.slices[0]->size;
  ringbuffer_cancel(&(pEFDMA_Channel->transmit.ringbuffer));
  ringbuffer_slice_init(&(pEFDMA_Channel->transmit.ringbuffer), ringbuffer_slice_size);
  efdma_channel_set_tx_wake_func(pEFDMA_Channel, tx_func, priv);

  //TX Channels don't do events... but we need to enable event mode
  result = DmaChannelSetEventFunc(
	  pEFDMA_Channel->pTransmit_Channel,
	  efdma_channel_tx_event_func,
	  &(pEFDMA_Channel->transmit.ringbuffer)
	);

  // Restart tx channel
  result = efdma_channel_start_tx(pEFDMA_Channel);
  if (result)
  {
	OmniTekDebug(EFDMA, KERN_INFO, "Transmit Ringbuffer failed to re-start with result %d\n",result);
	return result;
  }

   OmniTekDebug(EFDMA, KERN_INFO, "Transmit Ringbuffer re-started with result %d\n",result);
  return result;

}
EXPORT_SYMBOL(efdma_channel_tx_restart);

bool efdma_channel_rx_running(struct efdma_channel * pEFDMA_Channel)
{
  return pEFDMA_Channel->receive.ringbuffer.pChannel->u.DmaChannel.Running;
}
EXPORT_SYMBOL(efdma_channel_rx_running);

bool efdma_channel_tx_running(struct efdma_channel * pEFDMA_Channel)
{
  return pEFDMA_Channel->transmit.ringbuffer.pChannel->u.DmaChannel.Running;
}
EXPORT_SYMBOL(efdma_channel_tx_running);



static void efdma_channel_delete(struct efdma_channel * pEFDMA_Channel)
{

  //Cancel DMA channels - cancel all requests on channel - should cancel individual requests, but then we should have
  //exclusive access to this channel - otherwise something will go horribly wrong...

  OmniTekDMAChannelCancel(pEFDMA_Channel->transmit.ringbuffer.pChannel);
  OmniTekDMAChannelCancel(pEFDMA_Channel->receive.ringbuffer.pChannel);

  //Free our ringbuffers
  ringbuffer_delete(&(pEFDMA_Channel->transmit.ringbuffer));
  ringbuffer_delete(&(pEFDMA_Channel->receive.ringbuffer));
}

static int efdma_channel_acquire(struct _OMNITEK_INTERFACE_EXTENSION * pExt, int fdma_num, struct efdma_channel ** ppEFDMA_Channel, bool polling)
{

  struct efdma_channel * pEFDMA_Channel = kmalloc(sizeof(struct efdma_channel),GFP_ATOMIC);
  int result = 0;

  if (!pEFDMA_Channel)
  {
    OmniTekDebug(EFDMA, KERN_ERR,"Couldn't allocate efdma channel structure.\n");
    return -ENOMEM;
  }
  
  if ((result = efdma_channel_init(pEFDMA_Channel,pExt,fdma_num, polling)))
  {

    OmniTekDebug(EFDMA, KERN_ERR,"Failed EFDMA Channel init.\n");
    kfree(pEFDMA_Channel);
    return result;
  }

  list_add_tail(&(pEFDMA_Channel->Object), &(efdma_data.efdma_channels));
  *ppEFDMA_Channel = pEFDMA_Channel;
  return 0;
  
}
EXPORT_SYMBOL(efdma_channel_acquire);

static void efdma_channel_release(struct efdma_channel * pEFDMA_Channel)
{

    //Clear event function - also disables interrupts    
    DmaChannelSetEventFunc(
      pEFDMA_Channel->pReceive_Channel,
      NULL,
      NULL
    );
  
    //Clear callback functions
    efdma_channel_set_rx_callback(pEFDMA_Channel, NULL, NULL);
    efdma_channel_set_tx_wake_func(pEFDMA_Channel, NULL, NULL);

    //Delete Channel  
    efdma_channel_delete(pEFDMA_Channel);
    list_del(&(pEFDMA_Channel->Object));
    kfree(pEFDMA_Channel);
    OmniTekDebug(EFDMA, KERN_INFO,"EFDMA Channel Released.\n");


}
EXPORT_SYMBOL(efdma_channel_release);

static int __init efdma_module_init(void)
{
  //Get a reference to the OmniTek Hardware if possible:

  INIT_LIST_HEAD(&(efdma_data.efdma_channels));

  //TODO: Does this need to be multi-threaded?
  efdma_data.work_queue = create_workqueue("EFDMA WorkQueue\n");

  return 0;
}

static void __exit efdma_module_exit(void)
{
  //TODO: Implement necessary cleanup, wait for completion etc.

  //Do a list_for_each safe and stop each channel, then delete it......
  //Remember to re-set the FDMA autostart to true

  struct efdma_channel * pEFDMA_Channel, *pEFDMA_Channel_temp;

  list_for_each_entry_safe(pEFDMA_Channel, pEFDMA_Channel_temp, &(efdma_data.efdma_channels), Object)
  {
    efdma_channel_delete(pEFDMA_Channel);
    list_del(&(pEFDMA_Channel->Object));
    kfree(pEFDMA_Channel);
  }

  destroy_workqueue(efdma_data.work_queue); efdma_data.work_queue=NULL;


}


MODULE_LICENSE("GPL");
module_init(efdma_module_init);
module_exit(efdma_module_exit);

