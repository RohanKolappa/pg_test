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
#include "OmniTekDMARequest.h"

static atomic_t transaction_index = ATOMIC_INIT(0);

int OmniTekDMACreateRequest(
  PResource pChannel,
  const char        *           pBuffer,
  dma_addr_t                    dma_addr,
  size_t                        Size,
  loff_t                        LocalAddr,
  bool                          Write,
  
  OmniTekTransactionType        type,
  void *                        requestData,
  OmniTekTransactionCompleteCb *callback,
  
  struct _OmniTekDmaTransactionContext ** ppTransaction  


)
{

	int result = -EBUSY;
  struct _OmniTekDmaTransactionContext * pTransaction;

  //Requests from the IO level (async, sync or IOCtl) are sent here.  We create transactions for them and handle their queueing.
  //It is necessary for us to have mechanisms to make the calling processes wait.
  //We attempt to queue requests into the DMA core here, which sets up SGLs etc. for the hardware.  MDMA channels only support
  //One outstanding request, FDMA channels can support as many requests as there is coherent (SGL) buffer space.

  //This leaves us needing some synchronisation between the layers.  We maintain a pending queue here that stores requests that have
  //yet to be queued to the DMA core.  The FDMA channels will also have a queue of active transactions (the MDMA having only one active
  //transaction).
  
  //This leaves us with handling the blocking / non-blocking IO behaviour.  For the non-blocking IO we can simply return success and
  //the callback function for the request will complete the AIO.  For the blocking AIO the calling process needs to sleep after
  //this call until the callback function wakes it (it will be waiting on a condition in the request).

  //In both cases, if there are outstanding requests (pending queue is not empty) then the request will always be added to the queue.  
  //The completion routine will attempt to schedule requests from the pending queue as requests complete.

  //Cancellation is supported - blocking threads will timeout, and cancel their pending requests in this eventuality.  Requests can
  // be cancelled from the pending queue without any difficulty, they will simply have their completion call back called with a cancelled
  // status.  
  
  //All of which requires many bits of data structure etc. in order to work...
  
  //So the request creation process is:

  // * Check that the transfer direction is valid for the channel
  if (pChannel->u.DmaChannel.FDMA_Enabled)
  {
    if ((Write && !(pChannel->u.DmaChannel.FDMA_Write)) | ( !Write && !(pChannel->u.DmaChannel.FDMA_Read)))
    {
      OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Transaction is wrong direction for channel: attempted %s to channel %d!\n", 
          (Write ? "Write" : "Read"),pChannel->u.DmaChannel.Index);
      return -EINVAL;
    }
  }

  //TODO: Changed to atomic allocation for ethernet test, can be changed back in future
  // * Create the transaction context - allocate memory for pTransaction 
  *ppTransaction = kmalloc(sizeof(struct _OmniTekDmaTransactionContext),GFP_ATOMIC);
  // * Fill out the struct with the required information.

  pTransaction = *ppTransaction;


  if (!pTransaction)
  {
    OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Couldn't allocate memory for transaction!\n");
    return -ENOMEM;
  }

  pTransaction->pChannel = pChannel;
//  INIT_LIST_HEAD(&(pTransaction->Object)); //for placing in the workqueue?

  pTransaction->Xfer.Buffer     = pBuffer;
  pTransaction->Xfer.DMA_Addr   = dma_addr;
  pTransaction->Xfer.LocalAddr  = LocalAddr;
  pTransaction->Xfer.Size       = Size;
  pTransaction->Xfer.Write      = Write;

//	pTransaction->Sgl.Offset = 0;
//	pTransaction->Sgl.NumElements = 0;


	pTransaction->type = type;
	pTransaction->state = OMNITEK_TRANSACTION_NEW;

	pTransaction->typeInfo.generic = requestData;

  pTransaction->CompleteFunc = callback;

  pTransaction->DMACoreInfo.num_pages = 0;
  pTransaction->DMACoreInfo.offset = 0;
  pTransaction->DMACoreInfo.pages = 0;
  pTransaction->DMACoreInfo.Chained = false;

  pTransaction->index = atomic_inc_return(&transaction_index);

  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Creating Transaction %08x: user buffer @ %p, size: %d, Direction: %s, Local address: %x\n",
            pTransaction->index,pBuffer,(int)Size,(Write ? "Write" : "Read"),(unsigned int)LocalAddr);

  if (OMNITEK_TRANSACTION_BUFFER_KERNEL(type))
  {
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction is for kernel buffer\n");
    if (dma_addr == 0)
    {
      OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Kernel buffer specified, but no DMA Address provided!\n");
      return -EINVAL;
    }
  }


	OmniTekRequestQueueInitRequest(&(pTransaction->QueueObject));


  //Get the transaction ready for queueing
  result = DMAPrepareTransaction(pTransaction);
  if (result)
  {
    OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Couldn't prepare transaction\n");            
    return result;
  }
  pTransaction->state = OMNITEK_TRANSACTION_PREPARED;

  // * Check the pending queue - if there are pending transactions then this one gets queued, return EBUSY
  // * Otherwise ask the core to take the transaction - if it returns busy then queue it, otherwise return success.
	//If the queue is empty
	if (OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Pending)))
	{
		//Attempt to schedule it - three cases here:
    //MDMA - if there is no active request then queue this one
    //FDMA Manual - we can queue it if there's space
    //FDMA Auto   - we can queue it if there are no active transactions
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Attempting to add transaction %08x to Active queue\n",pTransaction->index);

    if (
         ((pChannel->u.DmaChannel.FDMA_Enabled == true) && (pChannel->u.DmaChannel.FDMA_AutoStart == false)) ||
         (OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active)) && 
            ((pChannel->u.DmaChannel.FDMA_AutoStart == true) || (pChannel->u.DmaChannel.FDMA_Enabled == false)))
          )
    {    
      OmniTekDebug(DMA_REQUEST,KERN_INFO,"Starting transaction %08x\n",pTransaction->index);    

  		result = DMAStartTransaction(pTransaction);
    }
    else
    {
      OmniTekDebug(DMA_REQUEST,KERN_INFO,"Channel busy \n");
      result = -EBUSY;
    }
	}
  else
  {
    result = -EBUSY;
  }

  //If the active queue is empty, but we couldn't queue the request something has definitely gone wrong
  if ((OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active))) && (result == -EBUSY))
  {
    OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Transaction %08x could not be added to empty active queue!\n",pTransaction->index);
    result = -EFAULT;
  }
	else if ((result == -EBUSY) && (pChannel->u.DmaChannel.FDMA_Enabled == true) && (pChannel->u.DmaChannel.FDMA_AutoStart == false))
	{
		//If we got a busy back for a manual start FDMA channel (streaming mode), then it's likely that we have starved the FDMA 
    OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Transaction %08x could not be started (starved FDMA?)!\n",pTransaction->index);
    result = -EFAULT;
  }    
  else if (result == -EBUSY)
  {
    //If we got a busy back (too many transactions/not enough SGL space OR MDMA that's already got an active transaction) then we can queue it to the pending queue
		result = OmniTekRequestQueueAddRequest(&(pChannel->u.DmaChannel.Pending),&(pTransaction->QueueObject));
  	pTransaction->state = OMNITEK_TRANSACTION_PENDING;
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x added to Pending queue\n",pTransaction->index);  
	}
  else if (result == 0)
  {
    //Otherwise it was accepted, add it to the active queue
		result = OmniTekRequestQueueAddRequest(&(pChannel->u.DmaChannel.Active),&(pTransaction->QueueObject));
  	pTransaction->state = OMNITEK_TRANSACTION_PROGRAMMED;
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x added to Active queue\n",pTransaction->index);
  }
  if (result) //If result is set at this stage then something went wrong when adding the transaction to one of the queues
  {
    OmniTekDebug(DMA_REQUEST_ERR,KERN_ERR,"Transaction couldn't be created.  Returning error %d.\n",result);
    //Need to clear up transaction:
    DMAFinishTransaction(pTransaction);
    kfree(pTransaction);
  }

	return result;
}

EXPORT_SYMBOL(OmniTekDMACreateRequest);

int OmniTekDMAReleaseRequest(
  struct _OmniTekDmaTransactionContext * pTransaction  
)
{
  //Release the request once everyone has finished with it.
  //TODO: Add some checking code to make sure it has been properly finished with!
  if (unlikely((pTransaction->state != OMNITEK_TRANSACTION_COMPLETED) && (pTransaction->state != OMNITEK_TRANSACTION_CANCELLED)))
  {
    OmniTekDebug(DMA_REQUEST, KERN_ERR, "Transaction %08x released that is neither completed nor cancelled? State is: %d\n",
      pTransaction->index,pTransaction->state);
    dump_stack();
  }
  else
    OmniTekDebug(DMA_REQUEST, KERN_INFO, "Releasing transaction %08x\n",pTransaction->index);

  kfree(pTransaction);
  return 0;
};

EXPORT_SYMBOL(OmniTekDMAReleaseRequest);

void OmniTekDMAChannelComplete(struct _Resource * pChannel)
{
  u32 Val;
  int i;

  OmniTekDebug(DMA_REQUEST,KERN_INFO,"ISR Triggered complete for channel: %d\n",pChannel->u.DmaChannel.Index);
  
  //If the channel has stopped we update it's status here. 
  ReadRegValue(pChannel->pExt,pChannel->General.Bar,pChannel->General.RegisterOffset + DMA_CHANNEL_CSR ,&Val);
  if (Val & 0x10)
  {
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Flagging channel %d as NOT running...\n",pChannel->u.DmaChannel.Index);
    pChannel->u.DmaChannel.Running = false;  
    OmniTekDebug(IRQ,KERN_INFO,"DMA Channel has stopped (done)!\n");
    WriteRegValue(pChannel->pExt,pChannel->General.Bar,pChannel->General.RegisterOffset + DMA_CHANNEL_CSR ,0);
  }
  else if (!(Val & 0x1))
  {
    //TODO: Do we need to do anything else here...
    OmniTekDebug(IRQ,KERN_INFO,"DMA Channel is not enabled - paused!\n");
  }

#if 0   //Not convinced this is safe because we use the number of sg irqs to control throttling... much better to:
  //Multiple completion handling... copy the number of available ISRs and set to zero.
  i = atomic_xchg(&(pChannel->u.DmaChannel.nSgIrqs),0);
  if (i==0)
    OmniTekDebug(IRQ,KERN_ERR,"Asked to perform complete work but no ISRs stored up?\n");
  else
    OmniTekDebug(IRQ,KERN_INFO,"Completing %d requests\n",i);

  if (i > 1)
    OmniTekDebug(GENERAL, KERN_ERR, "Multiple SG Completions: %d\n",i);

  while(i--)
  {
    OmniTekDMAChannelCompleteRequest(pChannel);
  } 
#else

  i=0;
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Have %d requests\n",atomic_read(&(pChannel->u.DmaChannel.nSgIrqs)));
  while (atomic_read(&(pChannel->u.DmaChannel.nSgIrqs)))
  {
    OmniTekDMAChannelCompleteRequest(pChannel);
    atomic_dec(&(pChannel->u.DmaChannel.nSgIrqs));
    i++;
  }
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Completed %d requests\n",i);
#endif
}
EXPORT_SYMBOL(OmniTekDMAChannelComplete);

void OmniTekDMAChannelCompleteWork(
  struct work_struct * work
)
{
  PResource pChannel = container_of(work,Resource,u.DmaChannel.ISRWork);

  OmniTekDMAChannelComplete(pChannel);
} 

void OmniTekDMAChannelStartNextRequest(
  PResource pChannel
)
{
  struct _OmniTekDmaTransactionContext * pTransaction;
	struct _OmniTekRequestQueueObject    * pRequest;
  int result = 0;
  int count = 0;

  while ((!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Pending))) && (result == 0))
  {

    // * Check that the channel doesn't already have outstanding active requests...
    //If this is an MDMA channel, or an auto-starting FDMA channel we can only have one active request
    //otherwise we can queue as many as possible...
    if (((pChannel->u.DmaChannel.FDMA_Enabled == true) && (pChannel->u.DmaChannel.FDMA_AutoStart == true)) ||
        (pChannel->u.DmaChannel.FDMA_Enabled == false))
    {
      if (!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active)))
        break;
    }


    // * Take next pending transaction from channel
    result = OmniTekRequestQueueNext(&(pChannel->u.DmaChannel.Pending),&pRequest);
    if (result)
    {
	    OmniTekDebug(DMA_REQUEST,KERN_INFO,"No more requests in Queue! Re-Queued %d\n",count);
	    break;
    }
    
    pTransaction = container_of(pRequest,struct _OmniTekDmaTransactionContext, QueueObject);

    //Attempt to schedule it:
    result = DMAStartTransaction(pTransaction);
    
    if (result == 0)
    {
      int result = OmniTekRequestQueueMoveRequest(&(pChannel->u.DmaChannel.Pending), &(pChannel->u.DmaChannel.Active),pRequest);
      pTransaction->state = OMNITEK_TRANSACTION_PROGRAMMED;
      if (unlikely(result))
      {  
	      OmniTekDebug(DMA_REQUEST,KERN_ERR,"Couldn't move transaction %08x from pending to active!\n",pTransaction->index);
	      return;
      }
      OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x moved from pending to active\n",pTransaction->index);
      count++;
    }



  }

  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Started %d pending requests\n",count);

  //If the channel is not running then start it?
  //TODO: Is this the correct behaviour for FDMA channels?

  if (pChannel->u.DmaChannel.Running == false)   
  {

    result = DMAChannelStart(pChannel);
    if (unlikely(result))
    {  
      OmniTekDebug(DMA_REQUEST,KERN_ERR,"Couldn't start DMA Channel %p no. %d\n",pChannel,pChannel->u.DmaChannel.Index);
    }

  }

}

void OmniTekDMAChannelCompleteRequest(
  PResource pChannel
)
{
  struct _OmniTekDmaTransactionContext * pTransaction;
	struct _OmniTekRequestQueueObject    * pRequest;
	int result = 0;
//  int nRequests = 1;    //TODO: We should probably have an atomic int or similar in the channel that
                        //gets incremented each interrupt to allow us to handle multiple completions here

  //Called by the DMA ISR when it detects a channel has completed a transaction
    
  // * Take current transaction from channel
  result = OmniTekRequestQueueNext(&(pChannel->u.DmaChannel.Active),&pRequest);
  if (unlikely(result))
  {
	  OmniTekDebug(DMA_REQUEST,KERN_ERR,"No requests in Queue!\n");
	  return;
  }

  result = OmniTekRequestQueueRemoveRequest(&(pChannel->u.DmaChannel.Active),pRequest);
  if (unlikely(result))
  {
	  OmniTekDebug(DMA_REQUEST,KERN_ERR,"Couldn't remove request from queue!\n");
	  return;
  }

  pTransaction = container_of(pRequest,struct _OmniTekDmaTransactionContext, QueueObject);
  // * Ask DMA Core to complete DMA (DMAFinishTransaction)
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Completing Transaction %08x for channel %d\n",pTransaction->index,pChannel->u.DmaChannel.Index);
  DMAFinishTransaction(pTransaction);
  	pTransaction->state = OMNITEK_TRANSACTION_COMPLETED;

  //If there are active requests in the queue, but the channel has stopped then we've broken something...
  //This is likely to be an FDMA that has starved and failed an SGL join or a bug...
  if ((!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active))) && (pChannel->u.DmaChannel.Running == false))
  {
    OmniTekDebug(DMA_REQUEST,KERN_ERR,"Channel %d has %d active requests but channel has stopped.  Starved FDMA?\n",
                  pChannel->u.DmaChannel.Index,
                  OmniTekRequestQueueSize(&(pChannel->u.DmaChannel.Active)));
    dump_stack();
  
    //Try to start the remaining transactions:
    DMAChannelStart(pChannel);
    result = -ERESTARTSYS;
  }


  //If there are pending requests on this channel we should attempt to resubmit them...
  if (!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Pending)))
  {
	  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Scheduling Pending requests for channel %d!\n",pChannel->u.DmaChannel.Index);
    OmniTekDMAChannelStartNextRequest(pChannel);
  }

  //If there are no active transactions for this channel then it is no longer running
  if (OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active)))
  {
	  OmniTekDebug(DMA_REQUEST,KERN_INFO,"No outstanding requests for channel %d, channel has stopped!\n",pChannel->u.DmaChannel.Index);
  }

  // * Call the transaction's Completion Callback function with relevant status.
  //TODO: should this be a schedule work?  If the complete func wants to grab the channel spinlock we need to change this...it will deadlock atm.
  if (*(pTransaction->CompleteFunc))
    (*(pTransaction->CompleteFunc))(pTransaction,result);
  else
    OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x Completion Function is NULL?\n",pTransaction->index);

  //Wake up waiting processes on this channel
  wake_up(&(pChannel->pExt->pDmaCtrl->u.DmaCtrl.DMA_Wait_Queue));

}

int OmniTekDMARequestCancel(
  struct _OmniTekDmaTransactionContext * pTransaction
)
{
  int result = 0;

  //Called to cancel a pending Transaction

  //Cancellation can occur at several different times:  Before the transaction is programmed (pending queue), during processing 
  //(e.g. belongs to hardware), and possibly after completion (but before it returns to the originator of the request).

  //This requires some synchronisation.  Blocking requests must wait for the request to be either cancelled or completed.  If they timeout then the 
  //blocking process must wait again on the request to expect completion.

  //So processing is:
  // * Get transaction status

  OmniTekDebug(DMA_REQUEST,KERN_NOTICE,"Cancelling transaction: %08x\n",pTransaction->index);
  //This lock should already be held...
  
  if (pTransaction->state == OMNITEK_TRANSACTION_PENDING)
  {
  // * If pending then remove it from the pending queue, and callback it's completion function with cancelled status
    result = OmniTekRequestQueueRemoveRequest(&(pTransaction->pChannel->u.DmaChannel.Pending), &(pTransaction->QueueObject));
    pTransaction->state = OMNITEK_TRANSACTION_CANCELLED;
    DMAFinishTransaction(pTransaction);
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x Cancelled\n",pTransaction->index);
    if (*(pTransaction->CompleteFunc))
    	(*(pTransaction->CompleteFunc))(pTransaction,-ECANCELED);
    else
      OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x Completion Function is NULL?\n",pTransaction->index);


  
  }
  else if (pTransaction->state == OMNITEK_TRANSACTION_PROGRAMMED)
  {
    // * If programmed then we need to stop the channel.  From here we cancel the other requests (as it is practically impossible
    //   to determine their status, and for streaming FDMA you're breaking everything here anyway
    // * Basically we just call channel cancel which will stop the channel and then cancel all requests, so we can just return it's status
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x is programmed, calling DmaChannelCancel\n",pTransaction->index);
    return OmniTekDMAChannelCancel(pTransaction->pChannel);
  }
  else if (pTransaction->state == OMNITEK_TRANSACTION_COMPLETED) 
  {
    // * If completed (and we've slipped in the gap somehow) then we _should_ do nothing.  If this happens then there is a flaw in
    //   the locking or the interrupt arrived while we were attempting to cancel the channel - channel cancel should block interrupts 
    //   to avoid this.
    OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x is already complete!\n",pTransaction->index);
    result =  -EINVAL;

  }
  else if  (pTransaction->state == OMNITEK_TRANSACTION_CANCELLED)
  {
    // * If it's already cancelled then we've got a problem as well!
    OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x is already cancelled!\n",pTransaction->index);
    result =  -EINVAL;
  }
  else
  {
    OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x is in unknown state? %d\n",pTransaction->index,pTransaction->state);
    result =  -EINVAL;
  }

  return result;
}


int OmniTekDMAChannelCancel(
  PResource pChannel
)
{
  //This will cancel _all_ requests on a channel (pending and programmed).  This is primarily aimed at FDMA channels where cancelling
  //One transaction out of a list of subsequent transactions is unlikely and impossible to implement.
  //It will also work for other channel types, e.g. during device removal.  All requests will be completed with cancelled status, so user
  //applications that are waiting on requests should see hardware failure.

  struct _OmniTekRequestQueueObject * pRequest;
  struct _OmniTekDmaTransactionContext * pTransaction;
  int result = 0;

  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Cancelling DMA Channel %d\n",pChannel->u.DmaChannel.Index);


  //Processing is:
  // * If channel is active, call DMAStop on channel.
  if (pChannel->u.DmaChannel.Running == true)
  {
    OmniTekDebug(DMA_REQUEST,KERN_INFO,"Channel is running, stopping\n");
    DMAChannelStop(pChannel);
  }

  // * Complete all active requests on channel with cancelled status and remove from list
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Cancelling DMA Channel %d Active Requests\n",pChannel->u.DmaChannel.Index);
  while (!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Active)))
  {
      result = OmniTekRequestQueueNext(&(pChannel->u.DmaChannel.Active),&pRequest);
      if (result)
      {
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Weird: Couldn't get request from active queue? Channel %p no. %d\n",pChannel,pChannel->u.DmaChannel.Index);
        break;
      }

      pTransaction=container_of(pRequest,struct _OmniTekDmaTransactionContext,QueueObject);

      if (pTransaction->state != OMNITEK_TRANSACTION_PROGRAMMED)
      {

        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x on Channel %p no. %d is not programmed, but is in active queue...\n",
            pTransaction->index,
            pChannel,
            pChannel->u.DmaChannel.Index);
        break;
      }

      result = OmniTekRequestQueueRemoveRequest(&(pTransaction->pChannel->u.DmaChannel.Active), &(pTransaction->QueueObject));
      if (result)
      {
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Weird: Couldn't remove request from active queue? Channel %p no. %d\n",pChannel,pChannel->u.DmaChannel.Index);
        break;
      }
      pTransaction->state = OMNITEK_TRANSACTION_CANCELLED;
      DMAFinishTransaction(pTransaction);
      OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x Cancelled from Active\n",pTransaction->index);
      //TODO: This is a bit unsafe when we use the event FDMA module as the module can be removed from beneath us and the 
      //completion function is removed.  Should we check the dereferenced CompleteFunc for non-null?
      

      if (*(pTransaction->CompleteFunc))
      	(*(pTransaction->CompleteFunc))(pTransaction,-ECANCELED);
      else
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x Completion Function is NULL?\n",pTransaction->index);

  }

  // * Complete all pending requests on channel queue with cancelled status and remove from list
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"Cancelling DMA Channel %d Pending Requests\n",pChannel->u.DmaChannel.Index);
  while (!OmniTekRequestQueueIsEmpty(&(pChannel->u.DmaChannel.Pending)))
  {
      result = OmniTekRequestQueueNext(&(pChannel->u.DmaChannel.Pending),&pRequest);
      if (result)
      {
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Weird: Couldn't get request from pending queue? Channel %p no. %d\n",pChannel,pChannel->u.DmaChannel.Index);
        break;
      }

      pTransaction=container_of(pRequest,struct _OmniTekDmaTransactionContext,QueueObject);

      if (pTransaction->state != OMNITEK_TRANSACTION_PENDING)
      {

        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x on Channel %p no. %d is not pending, but is in pending queue...\n",
            pTransaction->index,
            pChannel,
            pChannel->u.DmaChannel.Index);
        break;
      }

      result = OmniTekRequestQueueRemoveRequest(&(pTransaction->pChannel->u.DmaChannel.Pending), &(pTransaction->QueueObject));
      if (result)
      {
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Weird: Couldn't remove request from pending queue? Channel %p no. %d\n",pChannel,pChannel->u.DmaChannel.Index);
        break;
      }
      pTransaction->state = OMNITEK_TRANSACTION_CANCELLED;
      DMAFinishTransaction(pTransaction);
      OmniTekDebug(DMA_REQUEST,KERN_INFO,"Transaction %08x Cancelled from Pending\n",pTransaction->index);
      if (*(pTransaction->CompleteFunc))
      	(*(pTransaction->CompleteFunc))(pTransaction,-ECANCELED);
      else
        OmniTekDebug(DMA_REQUEST,KERN_ERR,"Transaction %08x Completion Function is NULL?\n",pTransaction->index);
  }

  pChannel->u.DmaChannel.Running = false;
  OmniTekDebug(DMA_REQUEST,KERN_INFO,"DMA Channel %d cancelled!\n",pChannel->u.DmaChannel.Index);

  return 0;
}  

EXPORT_SYMBOL(OmniTekDMARequestCancel);
EXPORT_SYMBOL(OmniTekDMAChannelCancel);

