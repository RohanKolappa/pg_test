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

//Set to use one fixed SGL buffer for each channel, rather than allocating each one as required
#define USE_FIXED_SGL_BUFFER
#define EFDMA_RX_PAUSING
#define USE_POLLED_EFDMA

/*! 
  \struct     _HWCapability
  \brief      Details of a hardware capability
   \details   This stores the details of a hardware capability
 */
typedef struct _HWCapability
{
	void *	  Id;                   /*! Capability Pointer/ID */
  __u16		  Type;                 /*! Capability type */
	__u8		  Version;              /*! Capability version */
	__u8		  Major;                /*! Capability Version Major */
	__u8		  Minor;                /*! Capability Version Minor */
	__u32		  Size;                 /*! Capability Size*/
} HWCapability, *PHWCapability;

/*! 
  \struct     _CapabilityList
  \brief      Stores a list of available hardware capabilities 
   \details   
 */
typedef struct _CapabilityList
{
	unsigned int nCapabilities;     /*! Number of capabilities */
	unsigned int nMaxCapabilities;  /*! Max Number of capabilities */
	unsigned int nOffset;           /*! Offset (not used?) */
	PHWCapability pList;            /*! Pointer to list of hardware capabilities */
} CapabilityList;

// These are the defined types for the Firmware Capabilites
// Values below 0x100 MUST match the Capability ID in the Firmware
// Above 0x100 are "virtual" IDs used for sub-sections of Capabilites (I/O's, Individual Framesync Channels etc.)
typedef enum _ResourceType
{
	// Physical IDs
	OMNITEK_RESOURCE_UNKNOWN,
	OMNITEK_RESOURCE_FPGA = 1,
	OMNITEK_RESOURCE_VIDEO_CTRL,
	OMNITEK_RESOURCE_DMA_CTRL = 6,
	OMNITEK_RESOURCE_FLASH_PROGRAMMER = 0x45,
	// Virtual IDs
	OMNITEK_RESOURCE_DMA_CHANNEL = 0x100,
	OMNITEK_RESOURCE_VIDEO_INPUT,
	OMNITEK_RESOURCE_VIDEO_OUTPUT,
	// FPGA Extension IDs
	OMNITEK_RESOURCE_FPGA_EXTENSION = 0x200,
	OMNITEK_RESOURCE_CLOCKED_INPUT =	(OMNITEK_RESOURCE_FPGA_EXTENSION),
	OMNITEK_RESOURCE_CLOCKED_OUTPUT =	(OMNITEK_RESOURCE_FPGA_EXTENSION + 1),
	OMNITEK_RESOURCE_AUDIO_EXTRACT = 	(OMNITEK_RESOURCE_FPGA_EXTENSION + 2),
	OMNITEK_RESOURCE_AUDIO_EMBED	= (OMNITEK_RESOURCE_FPGA_EXTENSION + 3),
	OMNITEK_RESOURCE_ANCILLARY_EXTRACT =	(OMNITEK_RESOURCE_FPGA_EXTENSION + 4),
	OMNITEK_RESOURCE_ANCILLARY_EMBED	= (OMNITEK_RESOURCE_FPGA_EXTENSION + 5),
	OMNITEK_MAX_RESOURCE_TYPE
} ResourceType;

//DMA Types
typedef enum _OmniTekTransactionType
{
	OMNITEK_TRANSACTION_NONE = 0,
  OMNITEK_TRANSACTION_IOCTL,
  OMNITEK_TRANSACTION_SYNC_IO,
  OMNITEK_TRANSACTION_ASYNC_IO,
  OMNITEK_TRANSACTION_SYNC_KERNEL,
  OMNITEK_TRANSACTION_ASYNC_KERNEL,
  OMNITEK_TRANSACTION_RING_SLICE
} OmniTekTransactionType;

#define OMNITEK_TRANSACTION_BUFFER_KERNEL(type) \
  (((type) == OMNITEK_TRANSACTION_SYNC_KERNEL) || ((type) == OMNITEK_TRANSACTION_ASYNC_KERNEL) || ((type) == OMNITEK_TRANSACTION_RING_SLICE))

typedef enum _OmniTekTransactionState
{
	OMNITEK_TRANSACTION_NEW = 0,
  OMNITEK_TRANSACTION_PENDING,
  OMNITEK_TRANSACTION_PREPARED,
  OMNITEK_TRANSACTION_PROGRAMMED,
  OMNITEK_TRANSACTION_COMPLETED,
  OMNITEK_TRANSACTION_CANCELLED
} OmniTekTransactionState;

/*! \struct _OmniTekRequestQueueObject */
typedef struct _OmniTekRequestQueueObject {
  struct list_head Object;                                  /*!< Object for placing in queue */
  struct _OmniTekRequestQueue * pCurrentQueue;              /*!< Pointer to object's current queue */
} OmniTekRequestQueueObject, *POmniTekRequestQueueObject;


struct _OmniTekDmaTransactionContext;
typedef void (*OmniTekTransactionCompleteCb)(struct _OmniTekDmaTransactionContext *, int status);


typedef struct _OmniTekDmaTransactionContext {
  struct _Resource * pChannel;                              /*!< Channel for this struct */
//  struct _OmniTekKernelRequest * pRequest;
  
//  struct list_head  Object; //for placing in the workqueue?

  struct _XferInfo  {                                       /*!< Transfer information struct */
    const char *  Buffer;                                   /*!< User Buffer Pointer */
    dma_addr_t    DMA_Addr;          												/*!< DMA address for kernel (single, contiguous buffer) transfer */       
    loff_t        LocalAddr;                                /*!< Local address (to DMA controller - e.g. device memory address) */
    size_t        Size;                                     /*!< Size of transfer */
    bool          Write;                                    /*!< Direction of transfer (if true then transfer is write to device) */
  } Xfer;

  struct _SglInfo {                                         /*!< Scatter gather list information struct */
#ifdef USE_FIXED_SGL_BUFFER
      u32 Offset; 
      u32 NumElements;
#else
    void * CoherentMem;                                     /*!< Pointer to DMA coherent memory for this transfer */
    dma_addr_t DmaMem;                                      /*!< DMA Address for coherent memory (address for device) */
#endif
  } Sgl;

  OmniTekTransactionType type;                              /*!< Type of transaction */
	OmniTekTransactionState state;                            /*!< Transaction state */
  union {
    struct kiocb  * kiocb;                          				/*!< kiocb pointer for asynchronous callback */
    struct OmniTekRequest * request;  				              /*!< OmniTekRequest pointer for IOCtl based DMA */
    void *  generic;                                    		/*!< Pointer for assignment */

  } typeInfo;
  OmniTekTransactionCompleteCb * CompleteFunc;  						/*!< Complete function call back pointer */

  struct _DMACoreInfo {																			/*!< Data structure for DMA Core Info */
    struct page ** pages;																		/*!< Reserved memory pages data structure */
  	struct sg_table sgt;																		/*!< Scatter gather mappings table */       

    unsigned long first_page;																/*!< Address for first reserved page */
    unsigned long last_page;																/*!< Address for last reserved page */
    unsigned long num_pages;																/*!< Number of reserved pages */
    off_t         offset;																		/*!< Offset of data into first page */
    bool Chained;                                           /*!< Has another request been chained to this one? */
  } DMACoreInfo;

	struct _OmniTekRequestQueueObject QueueObject;						/*!< Request Queue object */

  int index;                                                /*!< Number for tracking */

} OmniTekDmaTransactionContext, *POmniTekDmaTransactionContext;



#define UNKNOWN_TYPE			0x0

