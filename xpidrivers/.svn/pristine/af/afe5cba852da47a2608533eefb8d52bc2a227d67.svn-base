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
 * Image Processing Techniques makes no guarantee or representations regarding the use o  f, 
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

#define DMA             1
#define DMA_OPS         2
#define IRQ             4
#define GENERAL         8
#define RESOURCES       16
#define FOPS            32
#define DMA_CORE        64
#define DMA_REQUEST     128
#define REQUEST_QUEUE   256
#define DMA_PAGES       512
#define DMA_SGL         1024
#define SYSFS           2048
#define DMA_ISR         4096
#define EFDMA_CORE      8192
#define IRQ_HANDLER     16384
#define DMA_JOIN        32768
#define DMA_THROTTLE	  65536
#define DMA_REQUEST_ERR (1<<17)
//Everything is going to be -1
//#define OMNITEK_DEBUG_CATEGORIES (-1)

//Knock out some very noisy sections
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | REQUEST_QUEUE))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | IRQ_HANDLER))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | IRQ_HANDLER | DMA_ISR))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | IRQ_HANDLER | DMA_ISR | REQUEST_QUEUE))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_CORE | REQUEST_QUEUE | DMA_OPS))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | IRQ_HANDLER | DMA_CORE | REQUEST_QUEUE | DMA_OPS))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | DMA_ISR))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | IRQ | IRQ_HANDLER | DMA_ISR))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_CORE| IRQ | IRQ_HANDLER | DMA_ISR | DMA_REQUEST | REQUEST_QUEUE))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_CORE | IRQ | IRQ_HANDLER | DMA_ISR | DMA_REQUEST | REQUEST_QUEUE | DMA_CORE | DMA))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_CORE | IRQ | IRQ_HANDLER | DMA_ISR | DMA_REQUEST | REQUEST_QUEUE | DMA_CORE | DMA | DMA_OPS))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_REQUEST | REQUEST_QUEUE))
//#define OMNITEK_DEBUG_CATEGORIES (-1 & ~(DMA_PAGES | DMA_SGL | DMA_JOIN | DMA_CORE | DMA | DMA_OPS))

//Enable specific sections
//#define OMNITEK_DEBUG_CATEGORIES (DMA | GENERAL | RESOURCES | FOPS | DMA_REQUEST | REQUEST_QUEUE)
//#define OMNITEK_DEBUG_CATEGORIES (GENERAL | IRQ | DMA_REQUEST | DMA | DMA_CORE | RESOURCES | SYSFS)
//#define OMNITEK_DEBUG_CATEGORIES (GENERAL | DMA_THROTTLE)
//#define OMNITEK_DEBUG_CATEGORIES (GENERAL | IRQ | IRQ_HANDLER)
//#define OMNITEK_DEBUG_CATEGORIES (GENERAL | DMA_CORE)
//#define OMNITEK_DEBUG_CATEGORIES (GENERAL)

//Enable nothing (driver will be silent)
#define OMNITEK_DEBUG_CATEGORIES (0)

#ifndef OmniTekDebug
#define OmniTekDebug(category, level, format, Args...) \
    ({if (category & OMNITEK_DEBUG_CATEGORIES) printk(level "%-35s: " format ,__func__,##Args);})
#endif

#ifdef CONFIG_SMP
/*
#define OmniTekSpinLockCheck(spinlock, category, level, message, return) \
  ({ \
    if (!spin_is_locked(spinlock)) \
    { \
      OmniTekDebug(category, level, message); \
      dump_stack(); \
      if (return == true) return -EINVAL; \
    } \
  })
*/

/* A use for the comma operator: If the check fails then print the debug message, and dump the stack, 
   then provide a result (false).  Otherwise return true.  This way we can use this as a debug message or
   as something which evaluates to a result. */
#define OmniTekSpinLockCheck(spinlock, category, level, message, Args...) \
    ((!spin_is_locked(spinlock) ?                   \
    ({                                              \
      OmniTekDebug(category,level,message,##Args);  \
      dump_stack();                                 \
    }) , false : true )        \
  )

#else //Do nothing on non-smp systems - they don't have real spinlocks! - return true so the evaluations pass
#define OmniTekSpinLockCheck(spinlock, category, level, message, Args...) (true)
#endif

