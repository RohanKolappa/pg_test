/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        usb_base.h
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


#ifndef _USB_BASE_H_
#define _USB_BASE_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

int USB_SB_DoVendorRequest(PDEVICE_EXTENSION_COMMON pdxc, UINT8 Request, UINT16 Index, UINT16 Value, PVOID TransferBuffer,UINT32 TransferBufferLength, int Dir);

int USB_SB_DownloadBuffer(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuffer, SINT32 Size);
int USB_SB_WriteInterrupt(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 Data);
int USB_SB_ReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc, UINT16 *pIntRetVal, UINT16 *pIntRetData);
int USB_SB_ReadStream(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuf, SINT32 Size);

NTSTATUS USB_SB_ResetPipe(PDEVICE_EXTENSION_COMMON pdxc, UINT32 PipeNum);
NTSTATUS USB_SB_AbortPipe(PDEVICE_EXTENSION_COMMON pdxc, UINT32 PipeNum);

NTSTATUS USB_SB_InitializeBus(PDEVICE_EXTENSION_COMMON pdxc);
NTSTATUS USB_SB_UninitializeBus(PDEVICE_EXTENSION_COMMON pdxc);

int USB_SB_StartReadVideoStream(PDEVICE_EXTENSION_COMMON pdxc);
int USB_SB_StartReadAudioStream(PDEVICE_EXTENSION_COMMON pdxc);
int USB_SB_StartReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc);

void USB_SB_StopReadAllPipes(PDEVICE_EXTENSION_COMMON pdxc);

void USB_SB_ResetBusDevice(PDEVICE_EXTENSION_COMMON pdxc);
NTSTATUS USB_SB_SoftwareReset(PDEVICE_EXTENSION_COMMON pdxc);

extern WISDRIVER_HW_FUNCTION_TABLE	wisdriver_usb_sb_function_table;

//////////////////////////////////////////////////////////////
// vender request direction
#define DIR_IN				0
#define DIR_OUT				1 

/////

#ifdef __cplusplus
}
#endif


#endif


/****************************** end of usb_base.h ****************************/
