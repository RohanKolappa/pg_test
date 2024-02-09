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

#include "OmniTek_linux.h"
#include "OmniTekFops_linux.h"
#include "OmniTekResources_sysfs.h"
#include "OmniTekDMACore.h"
#include "../include/OmniTek.h" 

#include <asm/atomic.h>

/*!
  \brief       Interrupt Handling
  
  Interrupts in the system may be caused by many sources and may occur very close together 
  (in the case of event interrupts with small packets as little as 10 us apart).  This 
  requires a robust interrupt handling approach to avoid dropping interrupts.

  The interrupt handling is divided into 3 stages.  The first is the ISRs which are called
  immediately the interrupt fires.  The next stage is the handlers that are scheduled from
  the ISRs and will execute immediately after.  Finally there are the work processes that
  may be scheduled by the handlers to execute at a later time.

  The ISR routines should operate as fast as possible, and primarily grab status from the
  hardware and store it to the relevant data structures.  The main handler performs the
  following process:
    - Read Hardware interrupt status register - store to extension struct
    - Read relevant status registers as indicated by the status
    - Call any related routines (e.g. DMA ISR)
    - Wake handler routine
  The DMA ISR routine determines the DMA channels that have interrupted and reads their
  status, storing it to the channels data structure.

  The handler routine will retrieve status from the extension struct and execute the 
  handlers for each interrupt type.  These handlers will then deal with the interrupt or
  schedule a processing function to execute later.

  Finally the processing functions can perform whatever actions are required as a result
  of the interrupt.

  It is possible that interrupts will occur at any stage of processing, for that reason
  it is imperative that interrupt status is handled in a way that ensures synchronization.

  
  Status stored back to channels is contained in atomic data structures, thus allowing interrupt
  safe handling of values such as bytes transferred.

*/

/*!
 \brief               DMA Channel IRQ Service Routine
 \param [in] pChannel Pointer to dma channel struct
 \details             Get channel interrupt status and save to channel.  Clear channel status in HW
*/
static void OmniTekDMAChannelInterrupt(
  struct _Resource * pChannel
  )
{
  u32 ChanStatus;

  ChanStatus = GetDmaChannelInterruptStatus(pChannel);
  
  if (unlikely(ChanStatus == 0))
  {
    OmniTekDebug(IRQ,KERN_ERR,"DMA Channel %d Interrupt, but no channel interrupt status!\n", pChannel->u.DmaChannel.Index);
  }


  //TODO: We implement two channel pause mechanisms here, they need to be made more universal
  //
  //  The first mechanism pauses the EFDMA channel when there are not enough slices queued up,
  //  the second pauses the channel when there are too many bytes awaiting processing.
  //  These come about in two ways :
  //  1 if the processing of packets is fast enough then
  //  we may easily be keeping up with the incoming rate, but the scatter gather slices
  //  may not be getting queued fast enough, so we need to pause the channel until some
  //  slices are queued
  //  2 if the processing of packets is too slow then we end up with a lot of bytes unhandled
  //  so we back off the DMA channel to allow us to catch up.
  
  //  The DMA Channel throttling is 'released' when the number of bytes to handle has caught up.
  //  this release will only occur once any outstanding DMA transactions have been processed and
  //  re-queued for the EFDMA channel, so we guarantee there are always slices when we release
  //  the throttling.
  
  //The limit of data before throttling occurs could be varied, but currently is set at one slice.

  if (ChanStatus & OMNITEK_DMACHANNEL_INTERRUPT_SG)
  {
    //Scatter Gather Interrupt
    //Increment the number of scatter gather interrupts for the channel...
    int nsgirqs;
    atomic_inc(&(pChannel->u.DmaChannel.nSgIrqs));
    nsgirqs = atomic_read(&(pChannel->u.DmaChannel.nSgIrqs));
    OmniTekDebug(IRQ,KERN_INFO,"DMA Channel %d Interrupt: Scatter Gather Completion, have %d pending\n",pChannel->u.DmaChannel.Index,nsgirqs);

  }
//  if (ChanStatus & OMNITEK_DMACHANNEL_INTERRUPT_EVT)
//  {

    if (pChannel->u.DmaChannel.EFDMA_Polling == false)
  {
    //Event Interrupt
    //Get the amount of bytes transferred, and add it to the store
      size_t bytes_xferred = 0;
    size_t bytes_pending = 0;
    size_t bytes_unhandled = 0;

      bytes_xferred = DmaChannelReadBytesAvail(pChannel);    //Do bytes available read directly in EFDMA...
      

    atomic_add(bytes_xferred, &(pChannel->u.DmaChannel.nBytesUnhandled));
    atomic_add(bytes_xferred, &(pChannel->u.DmaChannel.nBytesXferred));
    bytes_pending = atomic_read(&(pChannel->u.DmaChannel.nBytesXferred));
    bytes_unhandled = atomic_read(&(pChannel->u.DmaChannel.nBytesUnhandled));
    
    OmniTekDebug(IRQ,KERN_INFO,"DMA Channel %d Interrupt: Event Occured, got %zd bytes, now have %zd bytes waiting handler and %zd total unprocessed\n",pChannel->u.DmaChannel.Index, bytes_xferred, bytes_pending, bytes_unhandled);
    }
    else
  #ifndef USE_POLLED_EFDMA
    {
      OmniTekDebug(IRQ,KERN_ERR,"Polling mode specified, but polling not compiled into driver?\n");
    }
#else
    {
    //For NAPI style EFDMA with polling we just want to disable further interrupts.  The interrupt handler will still be triggered so it can trigger the packet layer to respond
    //this way we won't stay jammed in interrupt.  Reading bytes etc. will be handled as part of the polling
    OmniTekDebug(IRQ,KERN_INFO,"DMA Channel %d Interrupt: Event Occured disabling event interrupts\n",pChannel->u.DmaChannel.Index);
    DMAChanEventInterruptEnable(pChannel,false,pChannel->u.DmaChannel.FDMA_Write);

    atomic_inc(&(pChannel->u.DmaChannel.nEvents));
    }
#endif

//  }

}
/*!
 \brief               DMA IRQ Service Routine
 \param [in] pExt     Pointer to device extension
 \details             Gather DMA Status from DMA Controller and get required channel status
*/
static void OmniTekDMAInterrupt(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
  )
{
  u32 channels = 0;
  PResource pChannel;
  
  //Read DMA Controller Interrupt Status:
  
  channels = GetDmaCtrlChannelStatus(pExt->pDmaCtrl);
    
  if (unlikely(channels == 0))
  {
    OmniTekDebug(IRQ,KERN_ERR,"DMA Interrupt, but no channels flagged!\n");
  }
  
  pExt->pDmaCtrl->u.DmaCtrl.ChannelStatus |= channels;

  //For each channel we need to read and clear the relevant status.
  list_for_each_entry(pChannel,&(pExt->pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
  {
		if(channels & (0x1 << pChannel->u.DmaChannel.Index))
    {
      OmniTekDMAChannelInterrupt(pChannel);
    }
  }  

  

}

/*!
 \brief               External interrupt service routine calls
 \param [in] pExt     Pointer to device extension
 \param [in] status   Interrupt bits set
 \details             Will call registered ISR routines for external interrupts.
*/
static void OmniTekExtInterrupt(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt,
  int status
  )
{
  unsigned char i;

  for (i = 4 ; status; i++)
  {
    if ((1 << i) & status)
    {
      status &= ~(1 << i);
      if (pExt->ExtInts.ISRs[i-4])
      {
        OmniTekDebug(IRQ,KERN_INFO,"ExtInterrupt ISR: %d\n",i);
        (*pExt->ExtInts.ISRs[i-4])(pExt);
      }
    }
  }  
}


/*!
 \brief               IRQ Service Routine
 \param [in] irq      IRQ Number that fired
 \param [in] dev_id   Device ID That interrupted
 \return              Value indicating whether an interrupt was handled or not
 \details             This is the 'fast' half of the ISR, 
*/
irqreturn_t 
OmniTekInterrupt(
  int irq,
  void * dev_id
  )
{
  POMNITEK_INTERFACE_EXTENSION pExt = dev_id;
  u32 cInterruptStatus;
  irqreturn_t irq_return = IRQ_NONE;
#ifdef CONFIG_MACH_NETVIZ
  u32 interrupt_status;


#endif

#ifdef CONFIG_MACH_NETVIZ
  reenter_irq:

  //NETVIZ Specific handling of legacy interrupts
  interrupt_status = __raw_readl(0xfee00000 + 0x1820);
  rmb();
  if (interrupt_status & 0x2)
  {
    OmniTekDebug(IRQ,KERN_INFO,"Legacy INTA de-assert set, clearing assert and de-assert!\n");
    __raw_writel(interrupt_status & 0x3,0xfee00000 + 0x1820);
    wmb();
    irq_return = IRQ_HANDLED;

  }


#endif

//TODO: Should we check the IRQ number?


  cInterruptStatus = GetInterruptStatus(pExt);

  OmniTekDebug(IRQ,KERN_INFO,"Interrupt received: status 0x%08x\n",cInterruptStatus);

  if (likely(cInterruptStatus))
  {

    pExt->nInterruptStatus |= cInterruptStatus;   //Store status for handler thread

    //NOTE: likely/unlikely here because we pretty much only get DMA interrupts...
    //Get status values for various sub-components
    if (unlikely(cInterruptStatus & OMNITEK_INTERRUPT_STATUS_TEST))                                   //Test interrupt - do nothing
    {                                   
      OmniTekDebug(IRQ,KERN_INFO,"Test Interrupt Received!\n");
    }

    if (likely(cInterruptStatus & OMNITEK_INTERRUPT_STATUS_DMA))                                     //DMA interrupt - get status
    {
      OmniTekDebug(IRQ,KERN_INFO,"DMA Interrupt Received!\n");
      OmniTekDMAInterrupt(pExt);
    }

    if (unlikely(cInterruptStatus & OMNITEK_INTERRUPT_STATUS_TX))                                   //TX Interrupt - not used
    {
      OmniTekDebug(IRQ,KERN_INFO,"TX Interrupt Received!\n");
    }

    if (unlikely(cInterruptStatus & OMNITEK_INTERRUPT_STATUS_TIMER))                                   //Timer interrupt - not currently used
    {
      OmniTekDebug(IRQ,KERN_INFO,"Timer Interrupt Received!\n");
    }
   
    //Support for external interrupt ISRs and Handlers
    if (cInterruptStatus & pExt->ExtInts.Mask)
    {
      OmniTekExtInterrupt(pExt,cInterruptStatus & pExt->ExtInts.Mask);
    }

    if (unlikely(cInterruptStatus & ~( OMNITEK_INTERRUPT_STATUS_TEST | OMNITEK_INTERRUPT_STATUS_DMA | OMNITEK_INTERRUPT_STATUS_TX | OMNITEK_INTERRUPT_STATUS_TIMER | pExt->ExtInts.Mask)))
    {
      OmniTekDebug(IRQ,KERN_INFO,"Unexpected interrupt status %x!\n",cInterruptStatus);
    }
           
  }
  else
  {
    OmniTekDebug(IRQ,KERN_INFO,"Interrupt not handled (no status?)\n");
  }

  #ifdef USE_IRQ_THREAD
  OmniTekDebug(IRQ,KERN_INFO,"Interrupt processed: Requesting IRQ Thread wakeup\n");
  irq_return = IRQ_WAKE_THREAD;
  #else

  if (interrupt_status & 0x2)
  {
    //Schedule handler when we've managed to clear the status... should coalesce multiple interrupts.
    tasklet_hi_schedule(&(pExt->irq_tasklet));
    OmniTekDebug(IRQ,KERN_INFO,"Interrupt processed: Scheduled Tasklet\n");
    irq_return = IRQ_HANDLED;

    //TODO: Hack!  Test if the de-assert ever gets re-set...
    //So if we have got status and started the ISR due to a de-assert we expect it to be re-raised...
    //Make sure that we get the de-assert message....
    if (cInterruptStatus)
    {
#if 0
      OmniTekDebug(IRQ,KERN_ERR,"Waiting for de-assert\n");
      interrupt_status = __raw_readl(0xfee00000 + 0x1820);
      rmb();
      while(interrupt_status & 0x2 != 0x2)
      {
        interrupt_status = __raw_readl(0xfee00000 + 0x1820);
        rmb();
        OmniTekDebug(IRQ,KERN_ERR,"(still) Waiting for de-assert\n");
      }
#endif
      //TODO: Really, really hack... force the irq handler to spin until we get no status back
      goto reenter_irq;
    }
  }
#endif

  return irq_return;
}

static int OmniTekDMAChannelInterruptHandler(
  PResource pChannel
  )
{
  int handled = 0;
  int sgirqs = 0;
  int bytes = 0;
#ifdef USE_POLLED_EFDMA
  int events = 0;
#endif


  //If the channel has SG interrupts then queue work for them;
  if ((sgirqs = atomic_read(&(pChannel->u.DmaChannel.nSgIrqs))))
  {
    OmniTekDebug(IRQ_HANDLER, KERN_INFO, "Queueing Completion Work for channel %d - have %d\n",pChannel->u.DmaChannel.Index,sgirqs);
    //queue_work(pChannel->pExt->pDmaCtrl->u.DmaCtrl.DMA_Work_Queue,&(pChannel->u.DmaChannel.ISRWork));
    //Don't bother queueing, let's just do the work...
    OmniTekDMAChannelComplete(pChannel);


    handled = 1;

    if (pChannel->u.DmaChannel.FDMA_Throttling == true)
      DMAChannelUnThrottle(pChannel, 4);


  }
  
  if (pChannel->u.DmaChannel.EFDMA_Polling == false)
  {

  //If the channel has bytes transferred then queue event handler.

    bytes = atomic_read(&(pChannel->u.DmaChannel.nBytesXferred));

    if (bytes)
  {

    if (likely(pChannel->u.DmaChannel.event_func))
    {
      OmniTekDebug(IRQ_HANDLER, KERN_INFO, "Calling event function for channel %d\n",pChannel->u.DmaChannel.Index);
      (pChannel->u.DmaChannel.event_func)(pChannel,pChannel->u.DmaChannel.event_priv);
      handled = 1;
    }
    else
    {
      OmniTekDebug(IRQ_HANDLER,KERN_ERR,"No event interrupt callback for channel %d\n",pChannel->u.DmaChannel.Index);
    }
  }

  }
  else
#ifndef USE_POLLED_EFDMA
  {
    OmniTekDebug(IRQ_HANDLER,KERN_ERR,"Polling mode set, but polling support not compiled into driver!\n");
  }
#else
  {
  //If the channel has an event then we need to call the poll function
  if ((events = atomic_read(&(pChannel->u.DmaChannel.nEvents))))
  {
    //Clear the events count - interrupts are disabled for now anyway
    atomic_set(&(pChannel->u.DmaChannel.nEvents),0);

    //Call the poll function
    if (likely(pChannel->u.DmaChannel.poll_func))
    {
      OmniTekDebug(IRQ_HANDLER, KERN_INFO, "Calling schedule poll function for channel %d\n",pChannel->u.DmaChannel.Index);
      (pChannel->u.DmaChannel.poll_func)(pChannel,pChannel->u.DmaChannel.poll_priv);
      handled = 1;
    }
    else
    {
      OmniTekDebug(IRQ_HANDLER,KERN_ERR,"No schedule poll callback for channel %d\n",pChannel->u.DmaChannel.Index);
      }
    }
  }
#endif

  if (unlikely(handled == 0))
  {
    OmniTekDebug(IRQ_HANDLER,KERN_ERR,"Weird: DMA Channel Interrupt Handler didn't handle anything!\n");
  }

  return handled;
}

/*!
 \brief       DMA Interrupt Handler
 \param [in]  pDmaCtrl  Dma Controller to process
*/
static void OmniTekDMAInterruptHandler(
  PResource   pDmaCtrl
  )
{

  PResource pChannel;
  u32       ChannelsHandled = 0;

  if (unlikely(pDmaCtrl->u.DmaCtrl.ChannelStatus == 0))
  {
    OmniTekDebug(IRQ_HANDLER,KERN_ERR,"Weird: DMA Interrupt Handler called with 0 status\n");
  }

  list_for_each_entry(pChannel,&(pDmaCtrl->u.DmaCtrl.Channels),u.DmaChannel.Object)
  {
    if (pDmaCtrl->u.DmaCtrl.ChannelStatus & (0x1 << pChannel->u.DmaChannel.Index))
    {
      if (OmniTekDMAChannelInterruptHandler(pChannel))
      {
        ChannelsHandled |= (0x1 << pChannel->u.DmaChannel.Index); 
      }
    }
  }

  if (unlikely(ChannelsHandled != pDmaCtrl->u.DmaCtrl.ChannelStatus))
  {
    OmniTekDebug(IRQ_HANDLER,KERN_ERR,"Weird: DMA Interrupt Handler didn't handle all channels\n");
  }

  pDmaCtrl->u.DmaCtrl.ChannelStatus &= ~ChannelsHandled;

}

/*!
 \brief               External interrupt handler calls
 \param [in] pExt     Pointer to device extension
 \param [in] status   Interrupt bits set
 \details             Will call registered handler routines for external interrupts.
*/
static void OmniTekExtInterruptHandler(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt,
  int status
  )
{
  unsigned char i;

  for (i = 4 ; status; i++)
  {
    if ((1 << i) & status)
    {
      status &= ~(1 << i);
      if (pExt->ExtInts.Handlers[i])
        (*pExt->ExtInts.Handlers[i])(pExt);
    }
  }  
}


#ifdef USE_IRQ_THREAD
/*!
 \brief       Bottom half of interrupt handler - threaded
 \param [in]  irq IRQ number that has been triggered
 \param [in]  dev_id Pointer to device extension that has interrupted
 \return      Result depending on whether interrupt has been handled
*/
irqreturn_t OmniTekInterruptHandler(
  int irq,
  void * dev_id
  )
#else
/*!
 \brief       Bottom half of interrupt handler - tasklet
 \param [in]  irq_pExt pointer to device extension stored by top half handler
*/
void OmniTekInterruptHandler(
  unsigned long irq_pExt
)
#endif
{
  #ifdef USE_IRQ_THREAD
  POMNITEK_INTERFACE_EXTENSION pExt = (POMNITEK_INTERFACE_EXTENSION)dev_id;
  #else
  POMNITEK_INTERFACE_EXTENSION pExt = (POMNITEK_INTERFACE_EXTENSION)irq_pExt;
  #endif

  unsigned long flags;

  spin_lock_irqsave(&(pExt->IrqLock),flags);    //Take the IRQ Lock

  
  #ifdef USE_IRQ_THREAD
  OmniTekDebug(IRQ_HANDLER,KERN_INFO,"(Threaded IRQs) : IRQ Received! - status 0x%x\n",pExt->nInterruptStatus);
  #else
  OmniTekDebug(IRQ_HANDLER,KERN_INFO,"(Tasklet IRQs) : IRQ Received! - status 0x%x\n",pExt->nInterruptStatus);
  #endif  

  //The interrupt handler is scheduled as a result of the interrupt.  We need to process the interrupt status, and in turn call the sub-handlers for the various functions (e.g. DMA)

  if (unlikely(pExt->nInterruptStatus == 0))
  {
    OmniTekDebug(IRQ_HANDLER,KERN_ERR,"Weird: Interrupt Handler called with 0 status\n");
  }

  if (likely(pExt->nInterruptStatus & OMNITEK_INTERRUPT_STATUS_DMA))
  {
    OmniTekDebug(IRQ_HANDLER,KERN_INFO,"Calling DMA interrupt handler\n");
    OmniTekDMAInterruptHandler(pExt->pDmaCtrl);
  }

  //Support for external interrupt ISRs and Handlers
  if (pExt->nInterruptStatus & pExt->ExtInts.Mask)
  {
    OmniTekExtInterruptHandler(pExt,pExt->nInterruptStatus & pExt->ExtInts.Mask);
  }



  pExt->nInterruptStatus = 0;

  OmniTekDebug(IRQ_HANDLER,KERN_INFO,"Interrupt Handler Finished\n");

  mmiowb(); //Ensure all writes back to hardware have flushed (e.g. status updates).

  spin_unlock_irqrestore(&(pExt->IrqLock),flags);    //Take the IRQ Lock


  #ifdef USE_IRQ_THREAD
  return IRQ_HANDLED;
  #endif
}

/*!
 \brief       Register an external interrupt handler
 \param [in]  irq_pExt pointer to device extension to register for
 \param [in]  irq_number Number of interrupt to register (lines 4 - 15 currently supported)
 \param [in]  Optional ISR interrupt service routine (called in IRQ context)
 \param [in]  Optional Handler Routine (called in soft IRQ tasklet context)
 \details     Stores callback functions for interrupt routines in external modules.  Two functions can be provided:
              an ISR routine and a handler routine.  The former is called directly from interrupt context, the
              latter from a tasklet context called immediately after the ISR.  As such the following guidelines apply:
              - Neither callback may sleep: Both are called from an IRQ context, no sleeping allowed!  If you want to do this
                create a workitem and schedule it from either routine
              - The ISR Routine should be as fast as possible.
              - The handler can do more work, but should still not take too much time as it is in a soft-irq context
              - The handler routine can be interrupted by the ISR routine, so protect shared data structures
              - The pExt->irqLock is held during the handler call - further interrupts are disabled

              As an example implementation: The ISR routine increments an atomic_int each time it is called, this is used
              to indicate the number of interrupts that have occurred since the last handler call.
              When the handler runs, the count is atomic_xchg'd with zero, and the handler can then perform the required
              interrupt work.  If the work takes some time, or may need to sleep etc. it can then schedule a work item
              to do the processing.  Alternatively a sleeping function may be woken etc.

              Both routines are optional (although at least one of them should be set...) so if accurate tracking of multiple
              interrupts is not required then the handler routine can be omitted.  Similarly if the work to be done in the 
              interrupt is very simple and fast then there may be no need for a handler routine.
 */
int OmniTekRegisterExtInterrupt(
   struct _OMNITEK_INTERFACE_EXTENSION * pExt,
   int                 irq_number,
   ExtInterruptISR     ISR,
   ExtInterruptHandler Handler
)
{

  if ((irq_number < 4) || (irq_number >= 16))
  {
    OmniTekDebug(GENERAL,KERN_ERR,"irq_number %d outside of range 4-15\n",irq_number);
    return -EINVAL;
  }
  
  pExt->ExtInts.ISRs[irq_number-4] = ISR;
  pExt->ExtInts.Handlers[irq_number-4] = Handler;
  
  if ((pExt->ExtInts.ISRs[irq_number-4]) || (pExt->ExtInts.Handlers[irq_number-4]))
    pExt->ExtInts.Mask |= 1 << irq_number;
  else
    OmniTekDebug(GENERAL,KERN_ERR,"Neither ISR nor handler specified...\n");

  //Enable external interrupt
  InterruptEnable(pExt,1 << irq_number,true);

  return 0;
}
  
EXPORT_SYMBOL(OmniTekRegisterExtInterrupt);
