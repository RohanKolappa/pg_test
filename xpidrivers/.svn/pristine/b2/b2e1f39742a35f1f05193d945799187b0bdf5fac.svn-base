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

/*!
 \brief				Create a dma request (transaction)	
 \param [in]	pChannel pointer to channel resource to create request for
 \param [in]	pBuffer pointer to user space buffer that is the source/destination for the request
 \param [in]	Size size of transfer
 \param [in]	LocalAddr device side address for transfer
 \param [in]	Write true if this is a write to the device
 \param [in]	type	type of request
 \param [in]	requestData pointer to data to store in request
 \param [in]	callback completion callback function
 \param [in]	ppTransaction pointer to transaction struct pointer - transaction will be allocated by this function
 \return			0 on success, -EBUSY if the request was accepted into the pending queue, or error code if there is a problem
							creating the request
*/
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

);

/*!
 \brief				Release a completed request
 \param	[in]	pTransaction pointer to completed transaction (will be freed, cannot be used after this call)
 \return 			0 on success
*/
int OmniTekDMAReleaseRequest(
  struct _OmniTekDmaTransactionContext * pTransaction  
);

/*!
 \brief				Complete work function
 \param	[in]	work struct pointer
 \details			This function is scheduled when a transaction completes, it calls the channel complete function for
							the channel.
*/
void OmniTekDMAChannelCompleteWork(
  struct work_struct * work
);

/*!
 \brief				Complete a request on this channel
 \param [in]	pChannel pointer to channel resource that has a completed request
*/
void OmniTekDMAChannelCompleteRequest(
  PResource pChannel
);

/*!
 \brief				Cancel a dma transaction
 \param	[in]	pTransaction pointer to transaction to cancel
 \return 			0 on success
 \details 		If the request is pending then it is completed with cancelled status.  If it is active then the channel
							is stopped and all transactions outstanding will be cancelled
*/
int OmniTekDMARequestCancel(
  struct _OmniTekDmaTransactionContext * pTransaction
);

/*!
 \brief				Cancel all transactions on a DMA Channel
 \param	[in]	pChannel resource to cancel.
 \return 			0 on success
 \details			All transactions on this channel will be cancelled (and completed with cancelled status)
*/
int OmniTekDMAChannelCancel(
  PResource pChannel
);


/************* ADDITIONAL DOCUMENTATION **************/
/*!
 \page Driver_DMA_API NetViz Driver DMA API Documentation
 \section Introduction
 The following details the driver level API for DMA operation

 The DMA functionality of the driver is provided in several layers.  The lowest layer, the DMA core is responsible for programming the required
 scatter gather entries etc. for the device, and operating the device DMA Channels.

 On top of the core layer is the DMA Request layer.  This processes DMA requests, handling the necessary completion, cancellation and channel
 management operations.  The request layer makes use of queues to manage multiple outstanding requests.  Essentially this is the API layer.

 The DMA request layer is designed to accept requests from multiple sources, e.g. blocking IO calls, async IO calls and IOCtl IO calls.  It can also accept
 transfer requests to user memory or kernel memory buffers.

 \section DMA_Implementation Implementation
 As an example we can look at OmniTekDma.c  This uses DMA to provide blocking IO functionality.
 
 \subsection DMA_Request_Creation Request Creation
 DMA Transfers are represented as transaction requests.  In this case we provide the DMA channel, the user buffer (buf), size of transfer (count)
 local address (f_pos) and direction (write).  We also specify the transaction type, and a callback function.  The routine will allocate memory for
 and initialize a request with the supplied information, and submit it to the queue for processing.

 \code
  //Create a transaction
  result = OmniTekDMACreateRequest(
    pChannel, 
    buf,
    count,
    *f_pos,
    write,  
    OMNITEK_TRANSACTION_SYNC_IO,
    NULL,   //We don't need to store any extra information
    &OmniTek_MDMA_dev_complete,  
    &pTransaction 
  );
 \endcode

 The result will either be a 0 (success) indicating the transaction has been accepted for the hardware, -EBUSY if the hardware is busy and the
 transaction has been added to a queue, or another code indicating that some error occurred.

 \subsection DMA_Request_Completion Completion
 Once created the  DMA request will be processed, and once the transfer has occurred, the callback routine will be called, with a status 
 indicating whether the transaction completed succesfully, or if it was cancelled.

 In the OmniTekDma code the process requesting the transfer is put to sleep after the request is created, and placed on a wait queue.  The callback
 function wakes up this wait queue, causing any processes that have completed requests to resume.  The requesting process can then continue, in the
 OmnitekDma case returning to the user application the transfer status.

 Once the process no longer needs the request it should call OmniTekDMARequestRelease() which will free the memory allocated for the request.

 \subsection DMA_Request_Cancellation Cancellation
 Requests may need cancellation.  This is achieved through calling  OmniTekDMARequestCancel() with the transaction to be cancelled.

 Cancellation is complicated by the nature of DMA - the current state of the hardware is not known.  If the supplied transaction is pending and
 has not been programmed for the hardware then it can be cancelled immediately.  If, however, it has been programmed to hardware it is not possible
 to know whether the hardware has already begun this request.  In this case cancellation requires stopping the DMA channel.  This will cancel \em ALL
 transactions for the channel, whether pending or active, and hard abort the channel.  This may leave the DMA controller in an unstable state, and
 in practice there should be no need to cancel requests except in exceptional situations (shutdown, application crash etc.).
 
 The OmniTekDma.c code cancels transactions that do not complete within a specified time.  The blocking IO process waits until either the transaction
 completes or until a timeout occurs.  If the timeout occurs the request is cancelled and the process sleeps again.  Once the request is cancelled
 the associated completion callback will be called, which will cause the process to wake up.

 This second wait is important as it is possible that the request completes during the cancel call.  In any case OmniTekDMARequestRelease() cannot
 be called until the transaction is finished with - e.g. after the completion callback has occurred.

*/
