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


#include "OmniTekEFDMA_RingBuffer.h"

/*!
 \struct efdma_ringbuffer
 \brief EFDMA Channel Ringbuffer struct
*/
struct efdma_ringbuffer {
  struct ringbuffer       ringbuffer;     /*! < ringbuffer struct for this efdma channel */
  atomic_t                dma_waiting;    /*! < amount of data DMA'd but not accounted for */
  wait_queue_head_t       wait_queue;     /*! < Wait queue for blocking processes (e.g. transmit, waiting receive?) */
  struct work_struct      dma_work;       /*! < work struct for the dma_xferred call */
};


struct efdma_channel;
typedef int  (*efdma_receive_func)(struct efdma_channel * pEFDMA_Channel, void * priv, ssize_t size); /*! < External packet receive function */
typedef void (*efdma_tx_wake_func)(void * priv);                                                      /*! < TX Queue available function */
typedef int  (*efdma_schedule_poll_func)(struct efdma_channel * pEFDMA_Channel, void * priv);                      /*! < Schedule external function to poll for packets */


/*!
 \struct efdma_channel
 \brief EFDMA Channel struct
*/
struct efdma_channel {
  spinlock_t  SpinLock;                         /*! < SpinLock for EFDMA Channel Data Structure */
  struct efdma_ringbuffer transmit;             /*! < Transmit RingBuffer */
  struct efdma_ringbuffer receive;              /*! < Receive RingBuffer */
  
  struct _OMNITEK_INTERFACE_EXTENSION * pExt;   /*! < Pointer to OmniTek Device that this EFDMA channel is owned by */
  struct list_head  Object;                     /*! < Linked list entry for tracking channels */  
  efdma_receive_func *  rx_func;                /*! < Pointer to external packet receive function */
  efdma_tx_wake_func *  tx_func;                /*! < Pointer to tx queue wake function */
  efdma_schedule_poll_func * poll_func;         /*! < Pointer to RX polling function */
  bool  rx_throttle;                            /*! < This is set to enable throttling */
  bool  rx_throttled;                           /*! < This is set when the channel is being throttled */
  bool  rx_poll;                                /*! < Set if the channel is to use polling (e.g. NAPI) */
  void * rx_priv;                               /*! < Private data pointer for rx callback function */                               
  void * tx_priv;                               /*! < Private data pointer for tx callback function */                               
  void * poll_priv;                             /*! < Private data pointer for poll callback function */                               

  struct _Resource *pTransmit_Channel;          /*! < Transmit DMA Channel resource for this ringbuffer */
  struct _Resource *pReceive_Channel;           /*! < Receive DMA Channel resource for this ringbuffer */



};

/*!
 \struct efdma_module_data
 \brief EFDMA Module data structure
*/
struct efdma_module_data {
  struct list_head efdma_channels;              /*! < Linked list of efdma channels */
  struct workqueue_struct * work_queue;         /*! < Module queue for EFDMA work */
};




