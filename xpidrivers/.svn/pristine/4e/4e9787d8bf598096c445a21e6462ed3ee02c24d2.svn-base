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

/************ DEBUG STUFF ***********/
#define EFDMA             1
#define RINGBUFFER        2
#define RINGBUFFER_SLICE  4
#define EFDMA_THROTTLE    8
#define RINGBUFFER_DMA    16
#define EFDMA_ERROR       32

//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (-1)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA | RINGBUFFER_SLICE | RINGBUFFER | EFDMA_THROTTLE)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (RINGBUFFER_SLICE )
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA | RINGBUFFER_SLICE )
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA | RINGBUFFER)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA_THROTTLE | RINGBUFFER_DMA | RINGBUFFER | EFDMA)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA | EFDMA_THROTTLE | RINGBUFFER_DMA)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA_THROTTLE | RINGBUFFER_DMA | EFDMA_ERROR)
//#define OMNITEK_EFDMA_DEBUG_CATEGORIES (EFDMA_THROTTLE)
#define OMNITEK_EFDMA_DEBUG_CATEGORIES ( EFDMA_ERROR )

#define OmniTekDebug(category, level, format, Args...) \
    ({if (category & OMNITEK_EFDMA_DEBUG_CATEGORIES) printk(level "%-35s: " format ,__func__,##Args);})

