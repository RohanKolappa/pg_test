#pragma once

/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictl
y prohibited. 
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
#include <linux/types.h>

typedef struct _OmniTekBARIoctlBuffer
{
  __u32 Offset;
  __u32 Value;
  __u32 Size;
  __u8 * Buffer;
} OmniTekBARIoctlBuffer;

typedef struct _OmniTekMDMAIoctlBuffer
{
  const char          *   buf;
  size_t                count;
  loff_t                f_pos;

} OmniTekMDMAIoctlBuffer;


/* IOCTL Definitions */
#define OMNITEK_IOC_MAGIC 'O'

#define OMNITEK_BAR_IOCRESET	            _IO(OMNITEK_IOC_MAGIC, 0)

#define OMNITEK_BAR_IOCQREADREGISTER	    _IOWR(OMNITEK_IOC_MAGIC,1,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQWRITEREGISTER	    _IOWR(OMNITEK_IOC_MAGIC,2,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQGETREGISTER	      _IOWR(OMNITEK_IOC_MAGIC,3,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQRAWREADREGISTER   _IOWR(OMNITEK_IOC_MAGIC,4,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQRAWWRITEREGISTER  _IOWR(OMNITEK_IOC_MAGIC,5,OmniTekBARIoctlBuffer *)

#define OMNITEK_BAR_IOCQRAWREADBYTE    	  _IOWR(OMNITEK_IOC_MAGIC,6,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQRAWWRITEBYTE   	  _IOWR(OMNITEK_IOC_MAGIC,7,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQRAWREADBYTERANGE  _IOWR(OMNITEK_IOC_MAGIC,8,OmniTekBARIoctlBuffer *)
#define OMNITEK_BAR_IOCQRAWWRITEBYTERANGE _IOWR(OMNITEK_IOC_MAGIC,9,OmniTekBARIoctlBuffer *)

#define OMNITEK_MDMA_READ           	    _IOWR(OMNITEK_IOC_MAGIC,10,OmniTekMDMAIoctlBuffer *)
#define OMNITEK_MDMA_WRITE	              _IOWR(OMNITEK_IOC_MAGIC,11,OmniTekMDMAIoctlBuffer *)

//Kernel 'bounce-buffered' type IO
#define OMNITEK_MDMA_READ_KERNEL     	    _IOWR(OMNITEK_IOC_MAGIC,12,OmniTekMDMAIoctlBuffer *)
#define OMNITEK_MDMA_WRITE_KERNEL         _IOWR(OMNITEK_IOC_MAGIC,13,OmniTekMDMAIoctlBuffer *)

#define OMNITEK_FDMA_START	              _IOWR(OMNITEK_IOC_MAGIC,14,unsigned long)
#define OMNITEK_FDMA_SET_AUTO             _IOWR(OMNITEK_IOC_MAGIC,15,unsigned long)

#define OMNITEK_BAR_MAXNR 2
#define OMNITEK_MAX_MDMA_ADDR (0x1000)
#define OMNITEK_MAX_MDMA_LENGTH (0x1000)


