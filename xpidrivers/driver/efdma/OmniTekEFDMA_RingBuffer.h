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

#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"

struct ringbuffer;


/*!
 \struct ringbuffer_slice
 \brief Data structure for a ringbuffer slice
*/
struct ringbuffer_slice	{
	ssize_t										size;                       /*! < Total size of this slice  */
	char 										*	buffer;                     /*! < Pointer to data buffer  */
  dma_addr_t                buffer_dma_addr;            /*! < Device (DMA) address for buffer  */
	int												slice_no;                   /*! < Slice index number  */
	struct ringbuffer_slice * next;                       /*! < Pointer to next slice  */
  struct _OmniTekDmaTransactionContext * pTransaction;  /*! < DMA Transaction for this slice  */
  struct ringbuffer *       pRingBuffer;                /*! < Pointer to owning ringbuffer */
  bool                      processed;                  /*! < Has this slice been processed (data received) */

};


/*!
 \struct ringbuffer_pointer
 \brief data structure to store slice and position in ringbuffer
*/
struct ringbuffer_pointer {
	struct ringbuffer_slice	*	slice;                      /*! < Current slice this pointer is in  */
	struct ringbuffer 			* pRingBuffer;                /*! < Pointer back to owning ringbuffer struct  */
	off_t											offset;                     /*! < Offset into current slice */
};

/*! 
 \struct ringbuffer
 \brief EFDMA Ringbuffer data structure
*/
struct ringbuffer	{
	struct ringbuffer_pointer	dma_pointer;		            /*! < Pointer for DMA engine side  */
	struct ringbuffer_pointer user_pointer;		            /*! < Pointer for user/kernel side */

	struct ringbuffer_slice ** slices;                    /*! < Array of slices for this ringbuffer */
	int 											 nslices;                   /*! < Number of slices for ringbuffer */

	bool	 transmit_buffer;                               /*! < Direction of buffer */


	void (*process_func)(struct ringbuffer * pRingBuffer, ssize_t amt);                         /*! < Function called to indicate to hardware that data is available (transmit only) */
	void (*wake_func)(struct ringbuffer * pRingBuffer);                                         /*! < Function called when data is available to user (receive only) */
	void (*hardware_queue_slice)(struct ringbuffer * pRingBuffer, struct ringbuffer_slice *);   /*! < Function to queue DMA requests for slice */
                
  struct _Resource *pChannel;                                                                 /*! < DMA Channel resource for this ringbuffer */

  spinlock_t        SpinLock;                                                                 /*! < SpinLock for this ringbuffer */
  
  bool  failed;                                                                               /*! < Set if for some reason a transfer fails and we 
                                                                                                    are no longer in sync with HW */
  struct workqueue_struct * work_queue;                                                       /*! < Slice re-queueing work queue */

};

int ringbuffer_slice_init (struct ringbuffer * pRingBuffer, ssize_t slice_size );

/*! 
 \brief Allocate and initialise a ring buffer structure
 \param [in] pRingBuffer Pointer to the ringbuffer struct to initialise
 \param [in] nslices number of slices to allocate
 \param [in] slice_size size of each slice
 \param [in] transmit true if this is a transmit buffer, false for receive
 \param [in] pChannel pointer to DMA channel resource for this ringbuffer
 \param [in] process_func function that is called to transmit data (transmit only)
 \param [in] wake_func function that is called when a packet is received (receive only)
 \return 0 on success or error code
*/
int  ringbuffer_init(
		struct ringbuffer * pRingBuffer, 
		unsigned int nslices, 
		ssize_t slice_size, 
		bool transmit,
    struct _Resource * pChannel,
		void (*process_func)(struct ringbuffer * pRingBuffer, ssize_t amt),
		void (*wake_func)(struct ringbuffer * pRingBuffer)
	);

/*! 
 \brief Delete and release a ring buffer
 \param [in] pRingBuffer pointer to ringbuffer structure to delete
*/
void ringbuffer_cancel(struct ringbuffer * pRingBuffer);
void ringbuffer_delete(struct ringbuffer * pRingBuffer);

//Ringbuffer transmit functions
/*!
 \brief returns space available in ringbuffer
 \param [in] pRingBuffer ringbuffer to return space from
 \return space available in this ringbuffer
*/
ssize_t ringbuffer_space(struct ringbuffer * pRingBuffer);

/*!
 \brief transmit packet with header
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \param [in] data pointer to buffer to transfer
 \param [in] key value for packet header
 \param [in] min_length of packet data (e.g. for ethernet ETH_ZLEN)
 \return amount transferred or error code
*/
ssize_t ringbuffer_transmit_packet(struct ringbuffer * pRingBuffer, ssize_t size, char * data, unsigned char key, size_t min_length);

/*! \brief      Return the space available in transmit ringbuffer
    \details    Returns space and also if there is a sufficient number of slices remaining
    \param [in] pRingbuffer Pointer to struct ringbuffer to check
    \return     0 if there is insufficient space/slices, or size available, or error
*/
ssize_t ringbuffer_transmit_space(struct ringbuffer * pRingBuffer);

/*!
 \brief transmit buffer
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \param [in] data pointer to buffer to transfer
 \return amount transferred or error code
*/
ssize_t ringbuffer_transmit(struct ringbuffer * pRingBuffer, ssize_t size, char * data);

/*!
 \brief transmit partial packet
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \param [in] data pointer to buffer to transfer
 \return amount transferred or error code
*/
ssize_t ringbuffer_transmit_part(struct ringbuffer * pRingBuffer, ssize_t size, char * data);

//Ringbuffer receive functions
/*!
 \brief returns data available in ringbuffer
 \param [in] pRingBuffer ringbuffer to return data available from
 \return data available in this ringbuffer
*/
ssize_t ringbuffer_avail(struct ringbuffer * pRingBuffer);
ssize_t ringbuffer_tx_avail(struct ringbuffer * pRingBuffer);


/*!
 \brief returns size of next packed in ringbuffer by inspecting header at current pointer
 \param [in] pRingBuffer ringbuffer to return data available from
 \return size of next packet 
*/
ssize_t ringbuffer_next_packet_size(struct ringbuffer * pRingBuffer);

/*!
 \brief receive packet
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \param [in] data pointer to buffer to transfer
 \return amount received or error code
*/
ssize_t ringbuffer_receive(struct ringbuffer * pRingBuffer, ssize_t size, char * data);

/*!
 \brief receive partial packet
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \param [in] data pointer to buffer to transfer
 \return amount transferred or error code
*/
ssize_t ringbuffer_receive_part(struct ringbuffer * pRingBuffer, ssize_t size, char * data);

/*!
 \brief Dump ringbuffer data
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of data to dump
 \return amount dumped or error
 \details Dump data from ringbuffer - in order to try and regain ringbuffer sync.
*/
ssize_t ringbuffer_dump(struct ringbuffer * pRingBuffer, ssize_t size);


//Ringbuffer processing (DMA Side)
/*!
 \brief used by hardware to indicate that data has been transferred
 \param [in] pRingBuffer pointer to ringbuffer struct
 \param [in] size size of transfer
 \return 0 on success
*/
int ringbuffer_xferred(struct ringbuffer * pRingBuffer, ssize_t size);

//internal

void ringbuffer_slice_queue_work(
  struct work_struct * work
);


int ringbuffer_slice_is_complete(struct ringbuffer_slice * pSlice);

/*!
 \brief queue a slice for its ringbuffer
 \param [in] pSlice pointer to struct ringbuffer_slice to queue
 \return 0 on success
*/
int ringbuffer_queue_slice(struct ringbuffer_slice * pSlice);

/*!
 \brief increment slice pointer
 \param [in] pPointer pointer to ringbuffer slice pointer struct
 \param [in] requested amount to move pointer by
 \param [in] start if specified this will be set to the position in the ringbuffer _before_ the pointer was incremented
 \return amount incremented - this may be less than the amount requested if the size > available space.  Multiple calls may be required.
*/
ssize_t ringbuffer_increment_slice(struct ringbuffer_pointer * pPointer, ssize_t requested, char ** start);

/*!
 \brief make throttle decision for ringbuffer
 \param [in] pRingBuffer pointer to struct ringbuffer 
*/
void ringbuffer_rx_throttle(struct ringbuffer * pRingBuffer);


