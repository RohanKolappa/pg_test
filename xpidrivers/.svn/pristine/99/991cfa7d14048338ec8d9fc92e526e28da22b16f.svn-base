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

//#include "OmniTekTypes_linux.h"
#include "OmniTek_Exports.h"

extern int OmniTekGetNumExts(void);
extern struct _OMNITEK_INTERFACE_EXTENSION * OmniTekGetExtension(int);

extern struct _Resource * DmaChannelFind(
	  struct _OMNITEK_INTERFACE_EXTENSION * pExt,
	  u32 Identifier
	);

extern int DmaChannelSetEventFunc
(
    struct _Resource * pResource,
    void (*event_func)(struct _Resource * pResource, void * arg),
    void * arg
  );

extern void DMAChanEventInterruptEnable
(

    struct _Resource * pChannel,
    bool               Enable,
    bool               Write
  );

#ifdef USE_POLLED_EFDMA
extern int DmaChannelSetPollFunc
(
    struct _Resource * pResource,
    void (*poll_func)(struct _Resource * pResource, void * arg),
    void * arg
  );
#endif
/// DmaChannelGetBytesAvail gets the number of bytes stored up for this channel - this only works
/// when _not_ in polling mode as the bytes will be read for each interrupt and stored
/// for polling applications DmaChannelReadBytesAvail must be used which will directly 
/// read the amount available from hardware
extern size_t DmaChannelGetBytesAvail(
  struct _Resource * pChannel
  );

/// Read (and decrement) the bytes available register
extern size_t DmaChannelReadBytesAvail(
  struct _Resource * pChannel
  );

extern void DmaChannelWriteBytesAvail(
  struct _Resource * pChannel,
  size_t amt
  );

extern void DmaChannelSetAutoStart(
  struct _Resource * pChannel, 
  bool set
  );

extern void DmaChannelSetThrottling(
  struct _Resource * pChannel, 
  bool set
  );
  

extern void DMAChannelStop(
  struct _Resource * pChannel
  );

extern int DMAChannelStart(
  struct _Resource * pChannel
  );

extern void DMAChannelPause(
  struct _Resource * pChannel,
  bool pause
  );

extern bool DMAChannelThrottle(
  struct _Resource * pChannel
  );
extern bool DMAChannelUnThrottle(
  struct _Resource * pChannel,
  int threshold
  );

extern int OmniTekDMAChannelCancel(
  struct _Resource * pChannel
  );
extern int OmniTekDMARequestCancel(
  struct _OmniTekDmaTransactionContext * pTransaction
  );

extern int OmniTekDMACreateRequest(
  struct _Resource * pChannel,
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



extern int OmniTekDMAReleaseRequest(
  struct _OmniTekDmaTransactionContext * pTransaction  
  );

extern void OmniTekDMAChannelComplete(
  struct _Resource * pChannel
  );

extern int
ReadRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32			   *pVal
	);

extern int 
WriteRegValue(
	POMNITEK_INTERFACE_EXTENSION	pExt,
	u32				nBar,
	u32				nReg,
	u32			  nVal
	);

extern int OmniTekRegisterExtInterrupt(
   struct _OMNITEK_INTERFACE_EXTENSION * pExt,
   int                 irq_number,
   ExtInterruptISR     ISR,
   ExtInterruptHandler Handler
  );

/* no mask entries - only multicast addresses to allow more entries */
// #define NO_MASK_ENTRIES 1
#define MAC_FILTER_ENTRIES      16
//#define MAC_FILTER_ENTRIES      8

