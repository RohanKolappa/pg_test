/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        debug.h
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WINDOWS) && defined(DRIVER)
#include <wdm.h>
#endif

#define DEBUG_ALL_MESSAGES					0x0FFFFFFF
#define DEBUG_NO_MESSAGES					0x00000000

#define DEBUG_ALWAYS_DISPLAY				        0x80000000

#define DEBUG_ERROR_LEVEL					0x10000000
#define DEBUG_NOTICE_LEVEL					0x20000000
#define DEBUG_INFORMATION_LEVEL				        0x40000000

#define DEBUG_HARDWARE_USB_READ_WRITE		                0x00000001
#define DEBUG_HARDWARE_USB_CONFIGURATION	                0x00000002
#define DEBUG_AVSYNC						0x00000004
#define DEBUG_AUDIO_CAPTURE					0x00000008
#define DEBUG_CLASS_CAudioCapturePin        DEBUG_AUDIO_CAPTURE
#define DEBUG_VIDEO_CAPTURE					0x00000010
#define DEBUG_CLASS_CCapturePin             DEBUG_VIDEO_CAPTURE
#define DEBUG_VIDEO_PREVIEW					0x00000020
#define DEBUG_CLASS_CPreviewPin             DEBUG_VIDEO_PREVIEW
#define DEBUG_DEVICE						0x00000040
#define DEBUG_CLASS_CCaptureDevice		DEBUG_DEVICE
#define DEBUG_CAPTURE_FILTER				        0x00000080
#define DEBUG_CLASS_CCaptureFilter		DEBUG_CAPTURE_FILTER
#define DEBUG_PROPERTY						0x00000100
#define DEBUG_VIDEO_FORMAT					0x00000200
#define DEBUG_VIDEO_STREAMBUFFER			        0x00000400
#define DEBUG_AUDIO_STREAMBUFFER			        0x00000800
#define DEBUG_I2C						0x00001000
#define DEBUG_PACKGEN						0x00002000
#define DEBUG_HARDWARE_MISC					0x00004000
#define DEBUG_HARDWARE_USB_MISC				        0x00008000
#define DEBUG_MEMORY						0x00010000
#define DEBUG_CONFIGURATION					0x00020000
#define DEBUG_XBAR_FILTER					0x00040000
#define DEBUG_FUNC_TRACE                                        0x00080000
#define DEBUG_INFO_CODE                     DEBUG_INFORMATION_LEVEL
#define DEBUG_ERROR_CODE                    DEBUG_ERROR_LEVEL
  // changed to avoid conflict with debug level indicators
#define DEBUG_BREAK_ENABLE                                      0x00100000
#define DEBUG_PCI                                               0x00200000
#define DEBUG_START_STOP                                        0x00400000
#define DEBUG_REGISTRY						0x00800000
#define DEBUG_VBI						0x01000000
#define DEBUG_GPIO						0x02000000
#define DEBUG_OSD						0x04000000

#if defined(_WINDOWS)

#define PrintMsg(flags, _x_)					\
			if( (flags) & DebugFlags ) {		\
				DbgPrint ("<WISGOSTRM.SYS> ");	\
				DbgPrint _x_;					\
				DbgPrint ("\n");				\
			}

#elif defined(_LINUX)

#define PrintMsg(flags, _x_)					\
			if( (flags) & DebugFlags ) {		\
                            if( (flags) & DEBUG_ERROR_LEVEL ) printk (KERN_ERR);  \
                            else if( (flags) & DEBUG_NOTICE_LEVEL ) printk (KERN_NOTICE);  \
                            else if( (flags) & DEBUG_INFORMATION_LEVEL ) printk (KERN_INFO);  \
			    else printk (KERN_DEBUG); \
				printk ("<WIS Driver> ");		\
				printk _x_;						\
				printk ("\n");					\
			}

#endif

extern unsigned long DebugFlags;

#if DBG
#define DRIVERNAME                "<WISGOSTRM.SYS> "

#if defined(_WINDOWS)

#define DEBUG_BREAK_POINT         if(DebugFlags & DEBUG_BREAK_ENABLE) \
                                      DbgBreakPoint ();
#define DEBUG_PRINT(flags,x)      if((flags) & DebugFlags){  \
                                      DbgPrint x;          \
                                  }

#else

#define DEBUG_BREAK_POINT
#define DEBUG_PRINT(flags,x)

#endif

#define DEBUG_FUNC_CLASS_ENTER(classcode, x) if(DEBUG_FUNC_TRACE & DebugFlags){	                            \
												 DEBUG_PRINT(classcode, (DRIVERNAME x" Entered\n")); \
                                             }
#define DEBUG_FUNC_CLASS_EXIT(classcode, x)  if(DEBUG_FUNC_TRACE & DebugFlags){	                            \
												 DEBUG_PRINT(classcode, (DRIVERNAME x" Exiting\n")); \
                                             }
#define DEBUG_FUNC_ENTER(x)       DEBUG_PRINT(DEBUG_FUNC_TRACE, (DRIVERNAME x" Entered\n"));
#define DEBUG_FUNC_EXIT(x)        DEBUG_PRINT(DEBUG_FUNC_TRACE, (DRIVERNAME x" Exiting\n"));
#define DEBUG_INFO(x)             DEBUG_PRINT(DEBUG_INFO_CODE, (DRIVERNAME));   \
                                  DEBUG_PRINT(DEBUG_INFO_CODE, x);              \
                                  DEBUG_PRINT(DEBUG_INFO_CODE, ("\n"));
#define DEBUG_ERROR(x)            DEBUG_PRINT(DEBUG_ERROR_CODE, (DRIVERNAME "ERROR!"));   \
                                  DEBUG_PRINT(DEBUG_ERROR_CODE, x);                       \
                                  DEBUG_PRINT(DEBUG_ERROR_CODE, ("\n"));                  \
                                  DEBUG_BREAK_POINT
#define DEBUG_CLASS(classcode,x)  DEBUG_PRINT(classcode, (DRIVERNAME));   \
                                  DEBUG_PRINT(classcode, x);              \
                                  DEBUG_PRINT(classcode, ("\n"));
								  
#else
#define DEBUG_PRINT(flags,x)
#define DEBUG_FUNC_CLASS_ENTER(classcode, x)
#define DEBUG_FUNC_CLASS_EXIT(classcode, x)
#define DEBUG_FUNC_ENTER(x)
#define DEBUG_FUNC_EXIT(x)
#define DEBUG_INFO(x)
#define DEBUG_ERROR(x)
#define DEBUG_BREAK_POINT
#define DEBUG_CLASS(classcode,x)

#endif

void SuspendDebugOutput();
void ResumeDebugOutput();
void DumpBuffer(PVOID pvBuffer, UINT32 length);

#ifdef __cplusplus
}
#endif

#endif

/********************************** end of debug.h ***************************/
