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

#include "OmniTekRequestQueue.h"

#define DMA_CHANNEL_OFFSET				16

//Register positions
#define DMA_CTRL_CAP_HEADER				0
#define DMA_CTRL_CAP_REG				1
#define DMA_CTRL_INTERRUPT_STATUS		2
#define DMA_CTRL_ENH_INTERRUPT_CTRL     4
#define DMA_CTRL_ENH_INTERRUPT_STATUS     5


#define DMA_CHANNEL_PADR				0
#define DMA_CHANNEL_PADR_HIGH			1
#define DMA_CHANNEL_LADR				2
#define DMA_CHANNEL_LADR_HIGH			3
#define DMA_CHANNEL_DPR					4
#define DMA_CHANNEL_DPR_HIGH			5
#define DMA_CHANNEL_SIZE				6
#define DMA_CHANNEL_SIZE_HIGH			7
#define DMA_CHANNEL_CSR					8
#define DMA_CHANNEL_BYTES_XFER			10
#define NUM_REGS_PER_DMA_CHANNEL		11

#define DMA_DPR_BIT_PAUSE           0x01
#define DMA_DPR_BIT_END_OF_CHAIN		0x02
#define DMA_DPR_BIT_INTERRUPT			  0x04
#define DMA_DPR_BIT_DIRECTION_TO_PC	0x08

// Size of SGL list in bytes
#define	SGL_ITEM_SIZE					(sizeof(u64)*4)	

// In order to DMA Transfer bytes, how big does the SGL memory need to be?
// We need SGL_ITEM_SIZE per SGL element - each element can describe 1 Page

#define DMA_SGL_SIZE(Transfer)			(((Transfer) / (PAGE_SIZE)) * SGL_ITEM_SIZE)

//Encode
#define DMA_FDMA_CHANNEL(type, w, x)	(DMA_CHANNEL_FDMA | \
											((w) ? DMA_CHANNEL_WRITE : DMA_CHANNEL_READ) | \
											(((type) & 0xffff) << 8) | ((x) & 0xff))
#define DMA_MDMA_CHANNEL(x)			((x) & 0x7f)

//Decode
#define DMA_FDMA_TYPE(x)			(((x) >> 8) & 0xffff)	// FDMA Type - user defined
#define	DMA_CHANNEL(x)				((x) & 0xff)			// Bottom 8 bits are channel label

//Max single transfer size - determines amount of SG buffer space allocated for each channel.
//#define MAXTRANSFER (1<<20)
#ifdef CONFIG_MACH_NETVIZ
#define MAXTRANSFER (1<<24)
#else
#define MAXTRANSFER (1<<28)
#endif




/*!
 \brief					Setup DMA Device nodes
 \param [in]		pExt Pointer to device extension to set up DMA devices for
*/
void
OmniTekDMASetupDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);

/*!
 \brief					Release DMA Device nodes
 \param [in]		pExt Pointer to device extension to release DMA devices for
*/
void 
OmniTekDMAReleaseDev(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);

/*!
 \brief					Stop a DMA Channel (this is a hard abort)
 \param	[in] 		pChannel pointer to channel resource to stop.
*/
void  
DmaChannelStop(
	PResource pChannel
	);

/*!
 \brief					Has this channel got any outstanding transactions?
 \param [in]		pChannel pointer to channel resource
 \result				True if there are transactions in the channel's active queue
*/
bool
DmaChannelBusy(
	PResource pChannel
	);


/*!
 \brief					Initialise DMA channel resource
 \param [in]		pResource pointer to DMA channel resource to initialize
*/
void  
DmaResourceInit(
	Resource *pResource
	);

/*!
 \brief					Initialize DMA Controller
 \param [in]		pExt pointer to device extension to initialize
 \return				success if DMA Controller initialized
*/
int DmaInit(
  struct _OMNITEK_INTERFACE_EXTENSION * pExt
);

#ifdef USE_FIXED_SGL_BUFFER
unsigned int
DmaGetSglBufferOffset(
	PResource pChannel,
	unsigned int NumElements
	);

void
DmaReleaseSglBuffer(
	POmniTekDmaTransactionContext pContext
);
#endif

/*!
 \brief				Delete a DMA Channel resource
 \param [in]	pChannel pointer to channel resource to delete
 \details			Will stop channel and cancel any outstanding requests
*/
void
DmaChannelDelete(
	PResource pChannel
	);

/*!
 \brief				Delete a DMA Controller resource
 \param [in]  pCtrl pointer to controller resource to remove
 \details			Will delete all channels associated with this controller
*/
void
DmaCtrlRemove(
	PResource pCtrl
	);

/*!
 \brief				Return the current value of the bytes_avail reg
 \param [in]  pChannel pointer to the channel to read
 \return      amount of bytes available
*/
size_t DmaChannelReadBytesAvail(
  PResource pChannel
  );

/*!
 \brief				Return the number of bytes available in the channel
 \param [in]  pChannel pointer to the channel to read
 \return      amount of bytes available
*/
size_t DmaChannelGetBytesAvail(
  PResource pChannel
  );


/*!
 \brief				Set the auto start mode of the dma channel
 \param [in]  pChannel pointer to the channel 
 \param [in]  set the auto start mode
*/
void DmaChannelSetAutoStart(
  struct _Resource * pChannel, 
  bool set
  );



/************** MDMA FILE OPERATIONS *******************/
#ifdef MKERNEL
int OmniTek_MDMA_dev_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#else
long OmniTek_MDMA_dev_ioctl(struct file *, unsigned int, unsigned long);
#endif

int OmniTek_MDMA_dev_open(struct inode *, struct file *);

int OmniTek_MDMA_dev_release(struct inode * , struct file *);

ssize_t OmniTek_MDMA_dev_read(struct file * filp, char __user * buf, size_t count, loff_t *f_pos);

ssize_t OmniTek_MDMA_dev_write(struct file * filp, const char __user * buf, size_t count, loff_t *f_pos);

ssize_t OmniTek_MDMA_dev_transfer(struct file * filp, const char __user * buf, dma_addr_t dma_addr, size_t count, loff_t * f_pos, bool write, bool kernel_buffer);

/************** FDMA FILE OPERATIONS *******************/
ssize_t OmniTek_MDMA_dev_aio_read(struct kiocb * iocb, const struct iovec * vec, unsigned long niov, loff_t pos);

ssize_t OmniTek_MDMA_dev_aio_write(struct kiocb * iocb, const struct iovec * vec, unsigned long niov, loff_t pos);


