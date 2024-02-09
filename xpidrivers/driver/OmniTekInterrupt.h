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

//static inline void OmniTekDMAChannelInterrupt(PResource *pChannel);

//static inline void OmniTekDMAInterrupt(struct _OMNITEK_INTERFACE_EXTENSION pExt);

irqreturn_t OmniTekInterrupt(int irq, void *dev_id);

//static inline int OmniTekDMAChannelInterruptHandler(PResource pChannel);

//static inline void OmniTekDMAInterruptHandler(PResource pDmaCtrl);

#ifdef USE_IRQ_THREAD
/*!
 \brief       Bottom half of interrupt handler
 \param [in]  irq IRQ number that has been triggered
 \param [in]  dev_id Pointer to device extension that has interrupted
 \return      Result depending on whether interrupt has been handled
*/
irqreturn_t OmniTekInterruptHandler(
  int irq,
  void * dev_id
  );
#else
/*!
 \brief       Bottom half of interrupt handler
 \param [in]  irq_pExt pointer to device extension stored by top half handler
*/
void OmniTekInterruptHandler(
  unsigned long irq_pExt
);
#endif

/************* ADDITIONAL DOCUMENTATION **************/
/*!
 \page Driver_External_Interrupt_API NetViz Driver External Interrupt API Documentation
 \section Introduction
 The following details the API provided for receiving external interrupts from the core DMA driver

 \section Overview
 Interrupts are handled in the driver in two stages.  When the interrupt is received an ISR will run.  
 This gathers hardware status and stores it for later processing.  The ISR then schedules a tasklet 
 that runs immediately after its completion to perform any necessary further processing.  In the case
 where multiple very fast interrupts occur the ISR routine may operate several times before the tasklet
 routine is run.

 \section Implmentation
 An external module can register for either or both the ISR and handler depending on the design
 requirements.

 Note that in both cases the callback will occur in an IRQ (hard for ISR, soft for handler) context
 and as such neither routine may sleep, allocate memory in a non-atomic way or perform any other
 operations that are precluded by being in interrupt context.  The handlers will be called with the
 pExt->irqLock held, with interrupts disabled (spin_lock_irqsave()) so care must be taken not to call
 functions that take this lock - DmaChannelStart and DmaChannelStop are two.  If you need to perform
 these actions then you should schedule a workitem in the handler to perform them.

 The callback routines should be as fast as possible, performing the minimum amount of processing it can.

*/

 

