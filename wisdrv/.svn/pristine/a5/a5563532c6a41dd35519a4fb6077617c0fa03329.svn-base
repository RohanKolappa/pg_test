/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        platform.h
*
*    DESCRIPTION:
*        functions that are platform dependent
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/


#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WINDOWS)
	
	#if defined(DRIVER)  

		#include <wdm.h>
		#include <string.h>

		_CRTIMP int __cdecl sprintf(char *, const char *, ...);

		int PIWaitEvent(KEVENT *pEvent, SINT32 Timeout);
		PKEVENT PICreateNotificationEvent(int InstanceNum, PHANDLE EventHandle);
		void PIReleaseNotificationEvent(PKEVENT pKEvent);
		void PIInitializeEvent(PKEVENT pKEvent);
		void PISetEvent(PKEVENT pKEvent);
		void PIResetEvent(PKEVENT pKEvent);

		void PIInitializeSpinLock(PKSPIN_LOCK pSpinLock);
		void PIAcquireSpinLock(PKSPIN_LOCK pSpinLock, PKIRQL pirql);
		void PIReleaseSpinLock(PKSPIN_LOCK pSpinLock, KIRQL irql);

	#else

		#include "windows.h"

	#endif

#elif defined(_LINUX)

	#include <linux/string.h>
	#include <linux/wait.h>

	typedef unsigned char UCHAR;
	typedef unsigned short USHORT;
	typedef unsigned long ULONG;
	typedef long LONG;

	#define STATUS_SUCCESS        ((NTSTATUS)0x00000000L) // ntsubauth
	#define STATUS_UNSUCCESSFUL   ((NTSTATUS)0xC0000001L)

	int PIWaitEvent(wait_queue_head_t *pEvent, SINT32 Timeout);
	void PIReleaseNotificationEvent(wait_queue_head_t *pKEvent);
	void PIInitializeEvent(wait_queue_head_t *pKEvent);
	void PISetEvent(wait_queue_head_t *pKEvent);
	void PIResetEvent(wait_queue_head_t *pKEvent);

#endif

void GetConfigFilePath(SINT8 *Path);
int ReadFileToBuffer(char *FileName, UINT8 *Buf, int *ReadLen);
int WriteBufferToFile(char *FileName, UINT8 *Buf, int WriteLen);

void PISleep(SINT32 ms);

void *PIAllocMem(SINT32 Size);
void PIFreeMem(void *p);
void *PIAllocVirtualMem(SINT32 Size);
void PIFreeVirtualMem(void *p);
void PICopyMem(void *pDest, void *pSrc, UINT32 Length);
int  PICmpMem(void *pDest, void *pSrc, UINT32 Length);
void PIFillMem(void *p, UINT32 Length, UINT8 Fill);
ULONG PIGetMapping(PVOID pMem, ULONG size);

UINT32 GetDWORDValueFromRegistry(char *Name, UINT32 DefaultValue);

#ifdef __cplusplus
}
#endif

#endif

/******************************** end of platform.h **************************/

