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
  \brief Start this DMA Channel
  \param [in] pChannel Pointer to DMA channel resource to start
*/
int DMAChannelStart(
  PResource pChannel
);


/*!
  \brief Stop this DMA Channel
  \param [in] pChannel Pointer to DMA channel resource to stop
*/
void DMAChannelStop(
  PResource pChannel
);

/*!
  \brief Pause or resume this channel
  \param [in] pChannel Pointer to DMA channel resource to pause/resume
  \param [in] pause true for pause, false for resume
*/
void DMAChannelPause(
  struct _Resource * pChannel,
  bool pause
);


#if 0
/*!
  \brief Throttle this channel
  \param [in] pChannel Pointer to DMA channel resource to throttle
  \return whether channel was throttled
  \details If necessary this FDMA channel will be throttled
*/
bool DMAChannelThrottle(
  PResource pChannel
);

#endif
/*!
  \brief UnThrottle this channel
  \param [in] pChannel Pointer to DMA channel resource to unthrottle
  \return whether channel was unthrottled
  \details If necessary this FDMA channel will be unthrottled
*/
bool DMAChannelUnThrottle(
  PResource pChannel,
  int threshold
);



/*! 
  \brief Prepare this transaction
  \param [in] pTransaction Pointer to transaction to start
*/
int DMAPrepareTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
);

/*! 
  \brief Start this transaction
  \param [in] pTransaction Pointer to transaction to start
*/
int DMAStartTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
);

/*!
 \brief Finish this transaction
  \param [in] pTransaction Pointer to transaction to finish
*/
void DMAFinishTransaction(
  struct _OmniTekDmaTransactionContext * pTransaction
);
