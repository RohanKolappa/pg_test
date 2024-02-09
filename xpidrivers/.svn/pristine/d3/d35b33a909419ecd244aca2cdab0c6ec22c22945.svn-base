
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
#include <linux/spinlock.h>
#include <linux/types.h>	/* size_t */
#include <linux/errno.h>	/* error codes */
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "OmniTekEFDMA_RingBuffer.h"
#include "OmniTekEFDMA_Debug.h"
#include "OmniTekEFDMA.h"
#include "../OmniTek_linux.h" //ReadHWValue

#define TX_RING_MIN_SLICES 3




/*! \brief        Allocate and initialise a ring buffer structure
    \details      This initializes a new ringbuffer, allocating DMA memory etc.
    \param [in]   pRingBuffer   Pointer to a struct ringbuffer that is to be initialised
    \param [in]   nslices       Number of slices to use for ringbuffer
    \param [in]   slice_size    Size of each slice
    \param [in]   transmit      Set to true if this is a transmit ringbuffer
    \param [in]   pChannel      Pointer to underlying FDMA channel
    \param [in]   *process_func Callback function to instigate hardware transmission of data (e.g. DmaChannelWriteBytesAvail()) - only used for Transmit
    \param [in]   *wake_func    Callback function for received data - only used for receive
    \return                     0 on success or negative error code
*/  
int  ringbuffer_init(
		struct ringbuffer * pRingBuffer, 
		unsigned int nslices, 
		ssize_t slice_size, 
		bool transmit,
		struct _Resource * pChannel,
		void (*process_func)(struct ringbuffer * pRingBuffer, ssize_t amt),
		void (*wake_func)(struct ringbuffer * pRingBuffer)
	)
{
	unsigned int i = 0;
	//initialise the ringbuffer struct;

	//First create the slices and queue them
	OmniTekDebug(RINGBUFFER,KERN_INFO,"Allocating buffer slices array\n");
	pRingBuffer->slices = kmalloc(sizeof(struct ringbuffer_slice *) * nslices, GFP_KERNEL);
	OmniTekDebug(RINGBUFFER,KERN_INFO,"Slices array at %p\n",pRingBuffer->slices);

	for (i = 0 ; i < nslices; i++)
	{
		//pRingBuffer->slices[i] = __get_free_pages(GFP_KERNEL, get_order(slice_size));
		pRingBuffer->slices[i] = kmalloc(sizeof(struct ringbuffer_slice ),GFP_KERNEL);
		OmniTekDebug(RINGBUFFER,KERN_INFO,"Allocated slice %d at %p\n",i,pRingBuffer->slices[i]);
    
		//TODO: Slice buffers may need to be in DMA Coherent memory.
		//pRingBuffer->slices[i]->buffer = (char *)__get_free_pages(GFP_KERNEL, get_order(slice_size));
		//pRingBuffer->slices[i]->buffer = (char * )kmalloc(slice_size,GFP_KERNEL);

		OmniTekDebug(RINGBUFFER,KERN_INFO,"Allocating slice buffer with device %p: extension %p\n",pChannel->pExt->Device, pChannel->pExt);

		pRingBuffer->slices[i]->buffer = pci_alloc_consistent(	pChannel->pExt->Device,
															slice_size ,
															&(pRingBuffer->slices[i]->buffer_dma_addr));


		if (!pRingBuffer->slices[i]->buffer)
		{
			OmniTekDebug(RINGBUFFER,KERN_ERR,"Ringbuffer %d allocation failed !!!\n",i);
			printk("Ringbuffer %d allocation failed !!!\n",i);
			return -EINVAL;
		}
		else
		{

			OmniTekDebug(RINGBUFFER,KERN_INFO,"Allocated buffer %d at %p - %p\n",i,pRingBuffer->slices[i]->buffer,(void *)pRingBuffer->slices[i]->buffer_dma_addr);
			pRingBuffer->slices[i]->size = slice_size;
			pRingBuffer->slices[i]->next = NULL;
			pRingBuffer->slices[i]->slice_no = i;
			pRingBuffer->slices[i]->pTransaction = NULL;
			pRingBuffer->slices[i]->pRingBuffer = pRingBuffer;

		}
	}

	OmniTekDebug(RINGBUFFER,KERN_INFO,"Setting up ring buffer\n");

	pRingBuffer->pChannel = pChannel;
	pRingBuffer->nslices = nslices;
	pRingBuffer->transmit_buffer = transmit;

	pRingBuffer->process_func = process_func;
	pRingBuffer->wake_func = wake_func;

	return ringbuffer_slice_init (pRingBuffer, slice_size);

}

int ringbuffer_slice_init (struct ringbuffer * pRingBuffer, ssize_t slice_size )
{
	unsigned int i = 0;

	//Setup buffer pointers
	pRingBuffer->dma_pointer.slice = pRingBuffer->slices[0];
	pRingBuffer->dma_pointer.offset = 0;
	pRingBuffer->dma_pointer.pRingBuffer = pRingBuffer;
	pRingBuffer->user_pointer.slice = pRingBuffer->slices[0];
	pRingBuffer->user_pointer.offset = 0;
	pRingBuffer->user_pointer.pRingBuffer = pRingBuffer;
	pRingBuffer->failed = false;

	for (i = 0 ; i < pRingBuffer->nslices; i++)
	{
		pRingBuffer->slices[i]->size = slice_size;
		pRingBuffer->slices[i]->next = NULL;
		pRingBuffer->slices[i]->slice_no = i;
		pRingBuffer->slices[i]->pTransaction = NULL;
		pRingBuffer->slices[i]->pRingBuffer = pRingBuffer;
		if (ringbuffer_queue_slice(pRingBuffer->slices[i]))
		{
			OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"Couldn't queue ringbuffer slice\n");
			pRingBuffer->failed = true;
			break;
		}
	}

	if (i != pRingBuffer->nslices)
	{
		return -EINVAL;
	}

	return 0;	
}


void ringbuffer_cancel(struct ringbuffer * pRingBuffer)
{
	unsigned int i = 0;
	OmniTekDMAChannelCancel(pRingBuffer->pChannel);

	for (i = pRingBuffer->nslices - 1; i != -1 ; i--)
	{
		if ((pRingBuffer->slices[i]->pTransaction->state  != OMNITEK_TRANSACTION_COMPLETED) && (pRingBuffer->slices[i]->pTransaction->state != OMNITEK_TRANSACTION_CANCELLED))
		{
		  OmniTekDebug(RINGBUFFER, KERN_ERR, "Transaction %p no. %d is not cancelled/completed in ringbuffer delete!\n",pRingBuffer->slices[i]->pTransaction, pRingBuffer->slices[i]->pTransaction->index);
		}
		pRingBuffer->slices[i]->size = 0;
		//TODO: should properly de-queue the slices when shutting down...
		pRingBuffer->slices[i]->next = NULL;
	}
}



/*! \brief Delete and release a ring buffer
	\param [in]   pRingBuffer pointer to ringbuffer struct to release
*/
void ringbuffer_delete(struct ringbuffer * pRingBuffer)
{
	 unsigned int i = 0;

	 // Cancel all channels
     // PSCW -- commented the following out it's done in the for loop below and
     // it makes __dma_free_remap print freeing wrong coherent size (65536 != 0)
	 //ringbuffer_cancel(pRingBuffer);

	 for (i = pRingBuffer->nslices - 1; i != -1 ; i--)
	 {
		 //Free all memory for the buffer:
		 pci_free_consistent(&(pRingBuffer->pChannel->pExt->Device->dev),
								 pRingBuffer->slices[i]->size,
								 pRingBuffer->slices[i]->buffer,
								 pRingBuffer->slices[i]->buffer_dma_addr);

		pRingBuffer->slices[i]->size = 0;

		//TODO: should properly de-queue the slices when shutting down...
		pRingBuffer->slices[i]->next = NULL;
	 }
	 kfree(pRingBuffer->slices); pRingBuffer->slices = NULL;
}




/*! \brief Ringbuffer slices available
    \param [in]   pPointer  struct ringbuffer_pointer to calculate available slices from
    \return       number of slices remaining in ringbuffer
*/
int ringbuffer_slices_remaining(struct ringbuffer_pointer * pPointer)
{
  struct ringbuffer_slice * cur_slice = pPointer->slice;
  int count = 0;
  
  while (cur_slice->next != NULL)
  {
    cur_slice = cur_slice->next;
    count++;
  }
  return count;

}

/*! \brief Ringbuffer space available
    \param [in]   pRingBuffer  struct ringbuffer to calculate available space from
    \return       number of bytes remaining in ringbuffer from provided pointer
*/
ssize_t ringbuffer_space(struct ringbuffer * pRingBuffer)
{
  //Ringbuffer space is from the current user pointer to the end of the buffer
  struct ringbuffer_slice * cur_slice = pRingBuffer->user_pointer.slice;
  ssize_t space;

  if (pRingBuffer->failed) {
	OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"%s -  ringbuffer in failed state!\n", __func__);
    return -ERESTARTSYS;
  }

  space = cur_slice->size - pRingBuffer->user_pointer.offset;

  for (cur_slice = cur_slice->next; cur_slice != NULL; cur_slice = cur_slice->next)
    space += cur_slice->size;

  return space;
}

/*! \brief      Return the space available in transmit ringbuffer
    \details    Returns space and also if there is a sufficient number of slices remaining
    \param [in] pRingbuffer Pointer to struct ringbuffer to check
    \return     0 if there is insufficient space/slices, or size available, or error
*/
ssize_t ringbuffer_transmit_space(struct ringbuffer * pRingBuffer)
{
  if (ringbuffer_slices_remaining(&(pRingBuffer->user_pointer)) > TX_RING_MIN_SLICES)
    return ringbuffer_space(pRingBuffer) ;
  else
    return 0;
}

//Ringbuffer transmit function
//TODO: Could refactor this and other packet-related functions into OmniTekEFDMA.c ?  The ringbuffers are mostly ignorant of packets,
//      so moving this code out leaves the structure a bit cleaner.
/*! \brief   Transmit Packet
    \details This function performs the complete packet transmission.  Given a pointer to a data buffer and size it will automatically 
             create the required headers and padding to transmit the packet.  Total length should be a multiple of 64bits/8bytes and 
             zero-padding will be added accordingly.  In addition the payload can be zero padded to a minimum length where required
             - e.g. 60 bytes for ethernet.

    \param [in]   pRingBuffer   Pointer to the struct ringbuffer to transmit from
    \param [in]   size          Size of payload (in bytes) to transmit
    \param [in]   data          Pointer to data buffer to take transmit data from
    \param [in]   key           Key value to embed in header (N.B. Currently unused - key values other than 0 will cause an error)
    \param [in]   min_length    Minimum length for packet.  If the size parameter is less than this
                                the payload will be zero padded to at least this length.
    \return                     Requested payload size or negative error code
*/
ssize_t ringbuffer_transmit_packet(struct ringbuffer * pRingBuffer, ssize_t size, char * data, unsigned char key, size_t min_length)
{

  //Transmit a packet with header:
  ssize_t transferred = 0;
  ssize_t amt_to_transfer = 0;
  ssize_t packet_padding = 0;
  ssize_t payload_padding = 0;
  ssize_t packet_size = size;
  ssize_t amt_padding = 0;
  ssize_t amt_padded = 0;
  ssize_t space = 0;
  u32 reg_offset = 0;
  u64 header = 0;  
  char padding[8] = {0,0,0,0,0,0,0,0};

  //First let's get the actual packet size we will transfer
  if (packet_size < min_length)
  {
    packet_size = min_length;
    payload_padding = min_length - size;
  }
  //Now work out if we will need to pad this to match a 64 bit boundary
  packet_padding = (8-((packet_size + sizeof(header)) % 8)) % 8;

  //So now we have a total amount of padding
  amt_padding = packet_padding + payload_padding;

  //And our amount to transfer is
  amt_to_transfer = size + amt_padding;

  //Set up the header.
  header = ((packet_size + sizeof(header)) & 0xFFFF) << 16 | (key & 0x3F) << 10;
  

  if (!pRingBuffer->transmit_buffer)
  {
		OmniTekDebug(RINGBUFFER,KERN_ERR,"No Transmit Buffer!\n");
		return -EINVAL;
  }
  
  if (pRingBuffer->failed)
  {
		OmniTekDebug(EFDMA_ERROR,KERN_ERR,"%s - ringbuffer in failed state!\n", __func__);
		return -EINVAL;
  }

  OmniTekDebug(RINGBUFFER,KERN_INFO,"Transmitting packet: header size: %zd bytes, data size: %zd bytes, padding size: %zd bytes, total %zx - 0x%zx bytes\n",
                                   sizeof(header),size,amt_padding,sizeof(header) + size + amt_padding,sizeof(header) + size + amt_padding);

  space = ringbuffer_tx_avail(pRingBuffer);
  OmniTekDebug(RINGBUFFER,KERN_INFO,"Ringbuffer has %d / 0x%08x bytes space\n",(int)space,(unsigned int)space);

  if (space < 0)
  {
	OmniTekDebug(RINGBUFFER,KERN_ERR,"Transmit Ringbuffer error!\n");
    return -ERESTARTSYS;
  }

  if ((space < (amt_to_transfer + sizeof(header))) || (ringbuffer_slices_remaining(&(pRingBuffer->user_pointer)) <= TX_RING_MIN_SLICES))
  {
	  //If there isn't enough space for this transfer return busy error.  The calling function can block or give up or whatever...
	  OmniTekDebug(RINGBUFFER,KERN_INFO,"Buffer is full, returning busy!\n");
	  return -EBUSY;
  }

  // Check if the txlegacy locked flag is set
  reg_offset = ReadHWValue(pRingBuffer->pChannel->pExt,1,24);
  if (reg_offset)
  {
	  if (ReadHWValue(pRingBuffer->pChannel->pExt,1,reg_offset + 0))
	  {
		  //If the legacy TX channel is blocked, return busy error.  The calling function can block or give up or whatever...
		  OmniTekDebug(RINGBUFFER,KERN_INFO,"TX legacy deadlocked, returning busy!\n");
		  printk("TX legacy deadlocked, returning busy!\n");
		  return -EBUSY;
	  }
  }

  //Copy header to ringbuffer
  transferred = ringbuffer_transmit(pRingBuffer,sizeof(header),(char *)&header);
  if (transferred != sizeof(header))
  {
    OmniTekDebug(RINGBUFFER,KERN_ERR,"Couldn't transmit packet header!\n");
    return transferred;
  }

  //Copy data to ringbuffer
  transferred = ringbuffer_transmit(pRingBuffer,size,data);
  if (transferred != size)
  {
    OmniTekDebug(RINGBUFFER,KERN_ERR,"Couldn't transmit packet data!\n");
    return transferred;
  }
  
  //If necessary we should add some padding to the buffer
  while (amt_padded < amt_padding)
  {
    ssize_t amt_to_pad = (amt_padding - amt_padded > 8) ? 8 : amt_padding - amt_padded;
    transferred = ringbuffer_transmit(pRingBuffer,amt_to_pad,padding);
    if (transferred != amt_to_pad)
    {
      OmniTekDebug(RINGBUFFER,KERN_ERR,"Couldn't transmit packet padding!\n");
      return transferred;
    }
    amt_padded += transferred;
  }
 
  OmniTekDebug(RINGBUFFER,KERN_INFO,"Transmitted packet!\n");

  //Let hardware know data is available.  If we have failed anywhere above then we are completely out of sync...and the ringbuffer should be reset
  if (pRingBuffer->process_func)
    pRingBuffer->process_func(pRingBuffer,amt_to_transfer + sizeof(header));

  return size;
}

/*! \brief  Ringbuffer Transmit
    \details Copies data into a transmit ringbuffer, crossing slices as required.  This does not call the process_func() so it is necessary for the
             calling function to indicate to hardware that data is available once the complete packet has been copied to the ringbuffer - see
             ringbuffer_transmit_packet().  Since data may cross slice boundaries this function will make repeated calls to ringbuffer_transmit_part()
             until the total requested has been transmitted or an error occurs
    \param [in]   pRingBuffer   pointer to a struct ringbuffer that will perform the transmission
    \param [in]   size          size of required transfer
    \param [in]   data          pointer to data buffer for transmission
    \return                     amount transferred (will be == size parameter) or negative error code.
*/
ssize_t ringbuffer_transmit(struct ringbuffer * pRingBuffer, ssize_t size, char * data)
{
	//Copy data into the buffer
	//Request partial transfers until the entire buffer is transferred
	
	//TODO: we should also support a vectorized version of this for ethernet - just transmit each part
	//separately, but hold the ringbuffer lock to control the ordering

	ssize_t 	amt_copied = 0;
	ssize_t		amt_copy;

	//OmniTekDebug(RINGBUFFER,KERN_INFO,"ringbuffer_transmit: transmitting packet size %d\n",size);
	while (amt_copied < size)
	{
		amt_copy = ringbuffer_transmit_part(pRingBuffer,size-amt_copied,data+amt_copied);
		if (amt_copy < 0)
		{
			OmniTekDebug(RINGBUFFER,KERN_INFO,"ringbuffer_transmit: underflow\n");
			return amt_copy;
		}
		amt_copied += amt_copy;	

	}


	return size;
}

/*! \brief    Ringbuffer Partial Transmit
    \details  This will transmit a provide buffer up to the lesser of the specified size or the space in the current slice.
              If the transmitted data reaches the end of the slice the slice pointers are automatically updated.  Hence
              in order to transmit a buffer that will not fit into the current slice this function is repeatedly called.
    \param [in]   pRingBuffer   pointer to the struct ringbuffer to use
    \param [in]   size          maximum amount to transfer
    \param [in]   data          pointer to the data to transmit
    \return                     amount that was copied.  This may be less than or equal to the size paramater.  Repeated 
                                calls to ringbuffer_transmit_part() should be made, with the size parameter and data
                                pointer updated accordingly, until the complete buffer has been transmitted.  
                                See ringbuffer_transmit() for an example.  If an error occurs a negative error code will be
                                returned instead.
*/
ssize_t ringbuffer_transmit_part(struct ringbuffer * pRingBuffer, ssize_t size, char * data)
{
	//Transmit a smaller buffer.  Note that this may not necessarily transmit the requested size if
	//there is insufficient space in the current slice.
	ssize_t 	amt_to_copy = 0;
	char 	* dest_copy = 0;	

	if (!pRingBuffer->transmit_buffer)
		return -EINVAL;

	amt_to_copy = ringbuffer_increment_slice(&(pRingBuffer->user_pointer),size,&dest_copy);
	if (amt_to_copy <= 0)
	{
		OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"%s -  buffer overflow!\n", __func__);
		pRingBuffer->failed = true;   //We've lost sync here
		return amt_to_copy;
	}
	//Copy the amount available to the buffer
	memcpy(dest_copy, data,amt_to_copy);

	//Let hardware know data is available
	//Moved to transmit function
	//if (pRingBuffer->process_func)
	//	pRingBuffer->process_func(pRingBuffer,amt_to_copy);

	return amt_to_copy;
}

//Ringbuffer receive functions
/*! \brief        Return amount of data available in a receive ringbuffer
    \details      This function determines the amount of data that is awaiting receive processing in the ringbuffer.
                  It calculates the difference between the user pointer and the DMA pointer to determine the amount available.
    \param [in]   pRingBuffer   pointer to struct ringbuffer to determine amount of data available in.
    \return                     Amount of data available in receive buffer or negative error code.
*/
ssize_t ringbuffer_avail(struct ringbuffer * pRingBuffer)
{
  //Returns the amount of data available for a ringbuffer read.
  ssize_t avail;
  struct ringbuffer_slice * cur_slice;

  if (pRingBuffer->failed == true)
  {
	OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"%s -  ringbuffer in failed state!\n", __func__);
	return -ERESTARTSYS;
  }

  avail = pRingBuffer->user_pointer.slice->size - pRingBuffer->user_pointer.offset;
  cur_slice = pRingBuffer->user_pointer.slice;
  while (cur_slice != pRingBuffer->dma_pointer.slice)
  {
    cur_slice = cur_slice->next;
    if (cur_slice == NULL)
    {
      OmniTekDebug(RINGBUFFER,KERN_INFO,"ringbuffer_avail - reached end of chain before dma pointer slice!\n");
      OmniTekDebug(RINGBUFFER,KERN_INFO,"Dma Pointer Slice: %d, user pointer slice %d\n",
			  pRingBuffer->dma_pointer.slice->slice_no,
			  pRingBuffer->user_pointer.slice->slice_no);

      return -EINVAL; //Something broke...
    }
    
    avail += cur_slice->size;
  }
  
  avail -= pRingBuffer->dma_pointer.slice->size - pRingBuffer->dma_pointer.offset;
  return avail;
}

ssize_t ringbuffer_tx_avail(struct ringbuffer * pRingBuffer)
{
  //Returns the amount of data available for a ringbuffer read.
  ssize_t avail;

  if (pRingBuffer->failed == true)
  {
	OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"%s -  ringbuffer in failed state!\n", __func__);
    return -ERESTARTSYS;
  }

  if (pRingBuffer->user_pointer.slice->slice_no == pRingBuffer->dma_pointer.slice->slice_no)
  {
	  if (pRingBuffer->user_pointer.offset < pRingBuffer->dma_pointer.offset)
	  {
			avail = pRingBuffer->dma_pointer.offset - pRingBuffer->user_pointer.offset;
	  }
	  else
	  {
		  avail = (pRingBuffer->nslices - 1) * pRingBuffer->user_pointer.slice->size;
	  }
  }
  else if (pRingBuffer->user_pointer.slice->slice_no > pRingBuffer->dma_pointer.slice->slice_no)
  {
	  avail = (pRingBuffer->nslices - (pRingBuffer->user_pointer.slice->slice_no - pRingBuffer->dma_pointer.slice->slice_no) -1 ) * pRingBuffer->user_pointer.slice->size;
  }
  else
  {
	  avail = ((pRingBuffer->dma_pointer.slice->slice_no - pRingBuffer->user_pointer.slice->slice_no) - 1) * pRingBuffer->user_pointer.slice->size;
  }

  return avail;
}




/*!
 \brief returns size of next packed in ringbuffer by inspecting header at current pointer - doesn't delete the packet header
 \param [in] pRingBuffer ringbuffer to return data available from
 \return size of next packet including packet header
*/
ssize_t ringbuffer_next_packet_size_save(struct ringbuffer * pRingBuffer)
{  
  ssize_t avail;
  u64 header;

  avail = pRingBuffer->user_pointer.slice->size - pRingBuffer->user_pointer.offset;

  //Need to copy data from potentially two slices - copy as much as possible from the first slice
  memcpy(&header,pRingBuffer->user_pointer.slice->buffer + pRingBuffer->user_pointer.offset, (avail >= 8 ? 8 : avail));
  //And copy some more from the next slice if required
  if (avail < 8)
    memcpy(&header+avail,pRingBuffer->user_pointer.slice->next->buffer,8-avail);
  
  OmniTekDebug(RINGBUFFER,KERN_INFO,"Read next packet size in buffer as 0x%08llx bytes %s\n",header&0xFFFFFFFF,(avail < 8) ?  "- Covered two slices" : "");

  return (header >> 16) & 0xFFFF;
}

/*!
 \brief returns size of data payload in next packet.  This will increment the pointer so that the header is not returned during a
        subsequent call to ringbuffer_receive.
 \param [in] pRingBuffer ringbuffer to return data available from
 \return size of next packet payload - e.g. without header
*/
ssize_t ringbuffer_next_packet_size(struct ringbuffer * pRingBuffer)
{
  u64 header;
  ssize_t result = 0;
  OmniTekDebug(RINGBUFFER_SLICE, KERN_INFO, "Next packet in slice %d at %p - start address %p\n",
    pRingBuffer->user_pointer.slice->slice_no,
    (void *)pRingBuffer->user_pointer.slice->buffer_dma_addr,
    (void* )pRingBuffer->user_pointer.slice->buffer_dma_addr+pRingBuffer->user_pointer.offset);

  result = ringbuffer_receive(pRingBuffer, sizeof(header), (char *)&header);

  if (result == -EINVAL) 
  {
    OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error getting packet header - ringbuffer receive returned -EINVAL\n");
    return result;
  }
  else if (result != sizeof(header))
  {
    OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error getting packet header - ringbuffer receive returned %zd\n",result);
    return result;
  }
  else
  {
    OmniTekDebug(RINGBUFFER,KERN_INFO,"Header is: 0x%016llx Next packet size  %llu\n",header,((header >> 16) & 0xFFFF) - sizeof(header));
	if (header & 0xFFFFFFFF0000FFFFll)
	{
	  OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error getting packet header - header appears to be corrupt: %016llx\n",header);
	  return -ERESTARTSYS;
	}
	else if (((header >> 16) & 0xFFFF) == 0)
    {
      OmniTekDebug(EFDMA_ERROR,KERN_ERR,"Error getting packet header - size read was 0.  Header read was 0x%016llx\n",header);
      return 0;
    }
    return ((header >> 16) & 0xFFFF) - sizeof(header);
  }
  

}
/*! \brief    Ringbuffer Receive
    \details  Receive requested amount of data from ringbuffer into supplied buffer.
    \param [in]   pRingBuffer     struct ringbuffer pointer to receive data from
    \param [in]   size            Amount of data to receive
    \param [in]   data            pointer to buffer to store received data in.
    \return                       Amount of data received - same as size parameter - or negative error code
*/
ssize_t ringbuffer_receive(struct ringbuffer * pRingBuffer, ssize_t size, char * data)
{
	ssize_t amt_copied = 0;
	ssize_t amt_avail = 0;
	//TODO should check for errors

  if (pRingBuffer->failed)
  {
	OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_ERR,"%s -  ringbuffer in failed state!\n", __func__);
    return -EINVAL;
  }

	while (amt_copied < size)
	{
		amt_avail = ringbuffer_receive_part(pRingBuffer, size-amt_copied, data + amt_copied);
		if (amt_avail < 0)
		{
			OmniTekDebug((RINGBUFFER|EFDMA_ERROR),KERN_INFO,"Underflow in ringbuffer_receive\n");
			pRingBuffer->failed = true;
			return amt_avail;
		}
		amt_copied += amt_avail; 
	}
	return size;
}

/*! \brief    Ringbuffer Partial Receive
    \details  This function attempts to copy the requested data amount to the supplied buffer.
              If the amount of data in the current slice is less than the amount requested then
              it will copy the data from that slice and return the amount copied.  The slice
              pointers will be automatically updated so the function can be called repeatedly - 
              with the size and output buffer pointers updated accordingly - until the required 
              data has been received.  See ringbuffer_receive() for example.
    \param [in]   pRingBuffer     struct ringbuffer pointer to receive data from
    \param [in]   size            Amount requested
    \param [in]   data            Pointer to buffer to copy data to
    \return                       Amount copied - less than or equal to amount requested with size parameter
                                  or negative error code.
*/
ssize_t ringbuffer_receive_part(struct ringbuffer * pRingBuffer, ssize_t size, char * data)
{
	//Copy data from ring buffer to the provided buffer.  If the data is across multiple slices this
	//will return an amount from the current slice and move the pointer.
	ssize_t amt_to_copy = 0;
	char * src_copy = 0;
	struct ringbuffer_slice * slice = pRingBuffer->user_pointer.slice;
	amt_to_copy = ringbuffer_increment_slice(&(pRingBuffer->user_pointer),size,&src_copy);
	if (amt_to_copy <= 0)
	{
		//OmniTekDebug(RINGBUFFER,KERN_ERR,"ringbuffer_receive_part underflow\n");
		OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"ringbuffer_receive_part underflow\n");
		return amt_to_copy;
	}

	memcpy(data,src_copy,amt_to_copy);

	//TODO: Make this a function in the dmachannel code...
	atomic_sub(amt_to_copy, &(pRingBuffer->pChannel->u.DmaChannel.nBytesUnhandled));
	
	//If we've incremented the slice then requeue it

	if (slice != pRingBuffer->user_pointer.slice)
	{    
		//Set a processed flag if we can't re-queue - when the dma completes it will be re-queued there instead
		if (slice->pTransaction)  //If the slice still has a transaction then mark it as finished
		{
			OmniTekDebug(RINGBUFFER_DMA, KERN_ERR, "Slice %d transaction outstanding, setting processed\n", slice->slice_no);
			slice->processed = true;
		}
		else
		{
			OmniTekDebug(RINGBUFFER_DMA, KERN_ERR, "Slice %d transaction complete, re-queuing\n", slice->slice_no);
			ringbuffer_queue_slice(slice);
		}
	}
	
	//OmniTekDebug(RINGBUFFER,KERN_INFO,"ringbuffer_receive_part copied %d\n",amt_to_copy);
	return amt_to_copy;
}

/*! \brief    RingBuffer dump function
    \details  Called to dump/discard data from a receive ringbuffer - used to attempt to 
              resynchronize the ringbuffer.  E.g. if the header looks to be invalid
              all the outstanding data in the ringbuffer can be dumped in the hope that
              subsequent data will be correctly aligned.  Could also be used to discard
              packets e.g. if multiple types are sent to the same EFDMA but not all are
              required.  This depends on the key value in the header being used and
              implemented.
    \param [in]   pRingBuffer     struct ringbuffer pointer to receive data from
    \param [in]   size            amount of data to dump.
    \return                       amount dumped (size) or negative error code
*/
ssize_t ringbuffer_dump(struct ringbuffer * pRingBuffer, ssize_t size)
{
  ssize_t amt_dumped  = 0;
  ssize_t amt_dump = 0;

  char * src_copy = 0;

  OmniTekDebug(EFDMA_ERROR, KERN_ERR, "Ringbuffer dumping %zd bytes\n",size);

  while (amt_dumped < size)
  {
    amt_dump = ringbuffer_increment_slice(&(pRingBuffer->user_pointer),size-amt_dumped,&src_copy);
    if (amt_dump < 0)
    {
      OmniTekDebug(EFDMA_ERROR, KERN_ERR, "Ringbuffer couldn't dump, increment_slice returned %zd\n",amt_dump);
      return amt_dump;
    }
    amt_dumped += amt_dump;
  }

  //TODO: Make this a function in the dmachannel code...
  atomic_sub(amt_dumped, &(pRingBuffer->pChannel->u.DmaChannel.nBytesUnhandled));

  OmniTekDebug(RINGBUFFER, KERN_ERR, "Ringbuffer dumped %zd bytes\n",amt_dumped);
  return amt_dumped;

}

/*! \brief    Ringbuffer Transferred
    \details  This function is called in response to an event or SG interrupt and updates the
              ringbuffer with the amount of data transferred into it.  For receive channels this
              occurs with each event and increments by the amount of bytes transferred as read from hardware.  
              For transmit channels it is called when the DMA transaction for the slice completes and increments
              by a whole slice at a time.
    \param [in]   pRingBuffer     struct ringbuffer pointer to receive data from
    \param [in]   size            amount of data that has been copied to ringbuffer.
    \return                       0 on success or error code if the ringbuffer has underflowed - this indicates
                                  a significant problem where the ringbuffer is out of sync with the hardware!
*/
int ringbuffer_xferred(struct ringbuffer * pRingBuffer, ssize_t size)
{

	//On XFer we update the DMA pointer.  We also wake up any waiting threads if necessary (receive buffer)
	ssize_t amt = 0;
	struct ringbuffer_slice * pSlice;

	pSlice = pRingBuffer->dma_pointer.slice;

	if (size == 0)
		return 0;

	while (amt < size)
	{
		ssize_t amt_avail = ringbuffer_increment_slice(&(pRingBuffer->dma_pointer), size - amt,NULL);
		if (amt_avail <= 0)
		{
			OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"ringbuffer_xferred underflow: requested %d, returned %d\n", size-amt, amt_avail);
			return amt_avail;
		}
		OmniTekDebug(RINGBUFFER,KERN_INFO,"ringbuffer_xferred chunk size: %zd, slice %d\n",amt_avail,pRingBuffer->dma_pointer.slice->slice_no);
		amt += amt_avail;
	}

//  OmniTekDebug(RINGBUFFER,KERN_INFO,"RingBuffer %p transferred %zd bytes\n",pRingBuffer,size);

	if (!pRingBuffer->transmit_buffer)
		OmniTekDebug(RINGBUFFER_SLICE,KERN_INFO,"RingBuffer %p transferred %zd bytes from slice %d\n",pRingBuffer,size,pSlice->slice_no);

	//Receive ringbuffer calls wake function to signal that data is available if we are not using a polling channel...
	if (!(pRingBuffer->transmit_buffer))
	{
		if (pRingBuffer->pChannel->u.DmaChannel.EFDMA_Polling == false)
		{
			if (pRingBuffer->wake_func)
			{
				pRingBuffer->wake_func(pRingBuffer);
			}
		}
	}
	return 0;
}

/*! \brief    Ringbuffer Increment Slice
    \details  This function increments the slice pointers in response to transmit/receive/hardware calls.
              Each ringbuffer maintains two struct ringbuffer_pointers - one for DMA and one for user.
          	  This function increments the provided pointer.  If it reaches the end of a slice
	            the pointer is updated to the start of the next slice and the amount that was available
	            in the previous slice is returned.  Thus if it returns less than the requested amt it
	            is necessary to repeat the call until the total amount needed has been returned.  This is
              normally achieved through repeated calls to ringbuffer_receive_part() or ringbuffer_transmit_part()
              or for the DMA side by ringbuffer_xferred().

              A char ** parameter is provided that will be set to the memory address of the data before the pointer
              was updated.  Hence by calling this function the combination of the returned pointer and returned size
              gives a location and amount of space available within a ringbuffer slice that data can be copied 
              to/from as required.

              If the requested amount causes the pointer to reach the end of a slice then the slice pointer is
              automatically moved to the beginning of the next slice.  See calling functions for example usage.

    \param [in]   pPointer    pointer to a struct ringbuffer_pointer to increment
    \param [in]   requested   requested amount to move by.
    \param [in]   start       If provided this pointer wil be set to the location in the ringbuffer before the
                              offset was changed.
    \return                   amount of data requested if there is sufficient space in slice, otherwise amount
                              available in current slice.  If an error occurs a negative error code will be 
                              returned.
*/
ssize_t ringbuffer_increment_slice(
		struct ringbuffer_pointer * pPointer, 
		ssize_t requested, 
		char ** start
	)
{
	
  ssize_t avail = 0;
  char * type = pPointer->pRingBuffer->transmit_buffer ? "TX: " : "RX: ";

  if (pPointer->slice == NULL)    //Run out of slices?
  {
    OmniTekDebug(EFDMA_ERROR,KERN_INFO,"%s Request to increment slice with NULL slice!\n",type);
    return -EINVAL;
  }

  if (requested < 0)
  {
    OmniTekDebug(EFDMA_ERROR,KERN_INFO,"%s Request to increment slice by negative amount %zd!\n",type,requested);
    return -EINVAL;
  }

  avail = pPointer->slice->size - pPointer->offset;

	//Transmit will want to know where to copy data to.  Receive will want to know where from.
	if (start)
		*start = pPointer->slice->buffer + pPointer->offset;
	
	if (avail <= requested)
	{
		//If we completely use or need more than the current buffer then update pointer to the next
		struct ringbuffer_slice * cur_slice = pPointer->slice;
		OmniTekDebug(RINGBUFFER,KERN_INFO,"%s Slice full, incrementing\n", type);
		pPointer->slice = pPointer->slice->next;

		if (pPointer->slice == NULL)
		{
			pPointer->pRingBuffer->failed = true;
			OmniTekDebug(EFDMA_ERROR,KERN_INFO,"%s Buffer overflow - no more slices!\n",type );
			return -EINVAL;
		}

		if (pPointer == &(pPointer->pRingBuffer->dma_pointer))
			OmniTekDebug(RINGBUFFER,KERN_INFO,"%s DMA Pointer slice now %d\n",type,pPointer->slice->slice_no);
		else
			OmniTekDebug(RINGBUFFER,KERN_INFO,"%s User Pointer slice now %d\n",type,pPointer->slice->slice_no);

		OmniTekDebug(RINGBUFFER,KERN_INFO,"%s Returning %zd bytes from slice %d at %p+%p, new offset %p\n",type,
                 avail,cur_slice->slice_no,(void*)cur_slice->buffer_dma_addr,(void*)pPointer->offset,(void*)0);
		pPointer->offset = 0;		

		return avail;
	}
	else
	{
		OmniTekDebug(RINGBUFFER,KERN_INFO,"%s Returning %zd bytes from slice %d at %p+%p, new offset %p\n", type,
                 requested,pPointer->slice->slice_no, (void*)pPointer->slice->buffer_dma_addr , (void*)pPointer->offset,(void*)(pPointer->offset + requested));
		pPointer->offset += requested;
		return requested;		
	}
}

DECLARE_WAIT_QUEUE_HEAD(slice_dma_wq);

#if 0
/*! \brief    Schedule re-queueing of slices once completed
    \details  When a slice no longer has current data (e.g. it has been received or transmitted) it can be re-queued.
              This function is called in to schedule the re-queueing of slices.  It may be necessary to wait 
              for the DMA completion to occur on the slice, hence the need for a queued function.
    \param [in]   work    automatically provided pointer to the work struct.

*/
void ringbuffer_slice_queue_work(
  struct work_struct * work
)
{
  int result = 0;
  struct ringbuffer_slice * pSlice = container_of(work,struct ringbuffer_slice,queue_work);

  if ((result = ringbuffer_slice_is_complete(pSlice)))
  {
    OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"Slice %d hasn't completed properly\n",pSlice->slice_no);
    return ;
  }     

	OmniTekDebug(RINGBUFFER_DMA,KERN_INFO,"Re-queueing slice %d\n",pSlice->slice_no);
	if (ringbuffer_queue_slice(pSlice))
  {
		OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"Failure Re-queueing slice %d\n",pSlice->slice_no);
    dump_stack();
  }
}

#endif

/*! \brief  Ringbuffer Slice DMA Completion
    \details  This function is called to indicate that the DMA transaction for a slice has completed.  It is
              provided as a callback to the underlying DMA driver.

    \param  [in]    pTransaction  pointer to the struct _OmniTekDmaTransactionContext that has completed.
    \param  [in]    status        status of completed transaction
*/
void ringbuffer_slice_dma_complete(struct _OmniTekDmaTransactionContext * pTransaction, int status)
{
  //This routine will be called automatically when the slice completes.  We should finish the transaction, and clear the transaction
  //pointer in the slice, it will be re-queued once the data has been processed from it.
  
  //TODO: Is there a race condition here where increment_slice or something might try and re-queue the slice before we've run slice_dma_complete
  //      thus causing a fault as the slice is already seen as active?
  //      Apparently, yes, there is.

  struct ringbuffer_slice * pSlice = (struct ringbuffer_slice *)(pTransaction->typeInfo.generic);  
//  struct _Resource * pChannel = pTransaction->pChannel;

  OmniTekDebug(RINGBUFFER_DMA, KERN_INFO,"Ringbuffer %p slice %d completed\n",pSlice->pRingBuffer,pSlice->slice_no);

  //Sanity check:
  if (pSlice->pTransaction != pTransaction)
  {
    OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Transactions don't match for slice\n");
  }

  if ((status) | (pTransaction->state != OMNITEK_TRANSACTION_COMPLETED))
  {
    //TODO: Need to handle this properly - it's either an error or the channel is being cancelled.
    OmniTekDebug((RINGBUFFER_DMA|EFDMA_ERROR), KERN_ERR,"Slice DMA Transaction was unsuccesful!\n");
    pSlice->pRingBuffer->failed = true;

    //TODO: If we get a slice completing that we weren't expecting to complete...then we need to pull it from the list?
    //      There is some work here to do with cancellation/closing the channel.  Shutdown etc. needs to be managed
    //      correctly
    //      We're only expecting slices to fail in some unususal circumstance or on shutdown... but still needs to be handled correctly.
    return; 

  }
  
  OmniTekDMAReleaseRequest(pTransaction);

  pSlice->pTransaction = NULL;

  if (pSlice->pRingBuffer->transmit_buffer)
  {
    struct efdma_channel * pEFDMA_Channel = container_of(pSlice->pRingBuffer,struct efdma_channel, transmit.ringbuffer);

    //Increment amount we know has been transferred - this should release this slice
    ringbuffer_xferred(pSlice->pRingBuffer,pSlice->size);   

    if (ringbuffer_queue_slice(pSlice))
    {
      OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Ringbuffer %p Couldn't re-queue slice %d!\n",pSlice->pRingBuffer, pSlice->slice_no);      
    }

    //Call resume function to re-enable queue after sleep
    if ((pEFDMA_Channel->tx_func) && (ringbuffer_slices_remaining(&(pSlice->pRingBuffer->user_pointer)) > 2))
    {
      OmniTekDebug(EFDMA, KERN_INFO, "Calling tx wake function\n");
      (*pEFDMA_Channel->tx_func)(pEFDMA_Channel->tx_priv);
    }
    else
	{
	  OmniTekDebug(EFDMA, KERN_ERR, "No TX Wake function provided\n");
	}
  }
  else
  {
    //There may be a receive buffer waiting for this completion to re-queue a slice, so wake up the queue here.
    //Wake up waiting work...
    OmniTekDebug(RINGBUFFER_DMA, KERN_INFO,"Waking up queue. Current slice: %d\n", pSlice->slice_no);
	OmniTekDebug(RINGBUFFER_DMA, KERN_INFO,"RingBuffer State: User pointer in slice %d, offset %d\n",pSlice->pRingBuffer->user_pointer.slice->slice_no,  (int)(pSlice->pRingBuffer->user_pointer.offset));
	OmniTekDebug(RINGBUFFER_DMA, KERN_INFO,"RingBuffer State: DMA pointer in slice %d, offset %d\n", pSlice->pRingBuffer->dma_pointer.slice->slice_no,  (int)(pSlice->pRingBuffer->dma_pointer.offset));

    if (pSlice->processed == true)
    {
      ringbuffer_queue_slice(pSlice);
      OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Slice %d processed, re-queueing\n",pSlice->slice_no);
    }
    else
    {
      OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Slice %d not yet processed\n",pSlice->slice_no);
    }   
  }
}

OmniTekTransactionCompleteCb ringbuffer_slice_dma_complete_func = ringbuffer_slice_dma_complete;

//For the RX functions we need to wait for the dma complete to occur before attempting to re-queue a slice
/*! \brief  Ringbuffer Slice Completion Check
    \details  Before we can re-queue a receive ringbuffer slice we must ensure the DMA transaction is complete.
             This function will sleep waiting for this to occur.
    \param [in]   pSlice pointer to a struct ringbuffer_slice that is expected to be complete or complete soon.
    \return       0 on success or error code if a timeout or other error occurs.
*/

#if 0
int ringbuffer_slice_is_complete(struct ringbuffer_slice * pSlice)
{
  int result;
  //If the slice still has a transaction wait for the DMA to complete
  if (pSlice->pTransaction)
  {
    OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"Waiting for slice %d DMA transaction %08x to complete\n", pSlice->slice_no, pSlice->pTransaction->index);

    result = wait_event_timeout(slice_dma_wq, 
                                pSlice->pTransaction == NULL,
                                msecs_to_jiffies(1000));

    if (result >= 0)
    {
      if (pSlice->pTransaction)
      {
        OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Queuing Slice %d transaction %08x hasn't finished, timeout\n",pSlice->slice_no, pSlice->pTransaction->index);
        return -ERESTARTSYS; 
      }
    }
    else
    {
      OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Queuing Slice %d transaction %08x wait returned %d\n",pSlice->slice_no, pSlice->pTransaction->index,result);
      if (pSlice->pTransaction == NULL)
        return 0;
      
      return -ERESTARTSYS;
    }
  }

  //Resume queue if there are still enough entries...
#ifdef REMOVE_ALL_UNTHROTTLES
  if (pSlice->pRingBuffer->pChannel->u.DmaChannel.FDMA_Throttling == true)
  {
    bool result = DMAChannelUnThrottle(pSlice->pRingBuffer->pChannel,4);
    if (result)
      ; //OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel unthrottled\n");
    else 
      OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel still throttled\n");
  }
#endif


  return 0;

}
#endif
//TODO: we could move the slices onto a standard list_head structure
/*! \brief    Queue Ringbuffer Slice
    \details  This function creates a DMA transaction for the slice and adds it to the ringbuffer's linked list of slices.
    \param  [in]  pSlice    Pointer to the slice to queue.
    \return   0 on success or negative error code.
*/
int ringbuffer_queue_slice(struct ringbuffer_slice * pSlice)
{
	struct ringbuffer_slice * cur_slice;
  struct ringbuffer * pRingBuffer = pSlice->pRingBuffer;
  int result= 0;
	int count = 1;
  
  pSlice->next = NULL;

	if (pRingBuffer->transmit_buffer)
		cur_slice = pRingBuffer->user_pointer.slice;
	else
		cur_slice = pRingBuffer->dma_pointer.slice;	

	if (cur_slice != pSlice)
	{
		while ((cur_slice->next != 0) && (count < 256))
		{
			cur_slice = cur_slice->next;

			if (cur_slice == pSlice)
			{
				OmniTekDebug(RINGBUFFER_DMA,KERN_INFO,"Error: Loop detected in slices linked list!\n");
				return -ERESTARTSYS;
			}

			count++;
		}

		if (count == 256)
		{
			OmniTekDebug(RINGBUFFER_DMA,KERN_INFO,"Error: Slices linked list broken!\n");
			return -ERESTARTSYS;
		}
	}

	if (pSlice->pTransaction)
	{
		OmniTekDebug(RINGBUFFER_DMA,KERN_ERR,"Error: Slice has outstanding transaction!\n");
		return -ERESTARTSYS;
	}

	OmniTekDebug(RINGBUFFER_DMA,KERN_INFO,"Creating DMA Request for Ringbuffer Device address: 0x%08llx, Kernel Address: %p,\
                              DMA Addr %p, length: 0x%08zx, direction: %s\n",
                              (unsigned long long)0,
                              pSlice->buffer,
                              (void *)pSlice->buffer_dma_addr,
                              pSlice->size,
                              pRingBuffer->transmit_buffer ? "Write" : "Read");

	result = OmniTekDMACreateRequest(
                                    pRingBuffer->pChannel,
                                    pSlice->buffer,
                                    pSlice->buffer_dma_addr,
                                    pSlice->size,
                                    0,
                                    pRingBuffer->transmit_buffer,    
                                    OMNITEK_TRANSACTION_RING_SLICE,
                                    pSlice,
                                    &ringbuffer_slice_dma_complete_func,
                                    &(pSlice->pTransaction)
                                  );

	if (result)
	{  
		OmniTekDebug(RINGBUFFER_DMA, KERN_ERR,"Couldn't create DMA transaction for slice!\n");
		return -ERESTARTSYS;
	}

	if (cur_slice != pSlice)
		cur_slice->next = pSlice;

	OmniTekDebug(RINGBUFFER_DMA,KERN_INFO,"Channel %p no. %d Queued Slice %d from %d.  Slices in buffer: %d\n", 
	pRingBuffer->pChannel,
	pRingBuffer->pChannel->u.DmaChannel.Index, 
	pSlice->slice_no,
	cur_slice->slice_no,
	count);
    
	pSlice->processed = false;

	//Once we've re-queued a slice we should attempt to resume the channel?

	if (pSlice->pRingBuffer->pChannel->u.DmaChannel.FDMA_Throttling == true)
	{
		bool result = DMAChannelUnThrottle(pSlice->pRingBuffer->pChannel, 4);
		if (result)
			; //OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel unthrottled\n");
		else 
			OmniTekDebug(EFDMA_THROTTLE, KERN_INFO, "DMA Channel still throttled\n");
	}

	return 0;
}

