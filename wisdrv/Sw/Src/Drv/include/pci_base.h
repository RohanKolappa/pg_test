/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        pci_base.h
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


#ifndef _PCI_BASE_H_
#define _PCI_BASE_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

int PCI_SB_DoVendorRequest(PDEVICE_EXTENSION_COMMON pdxc, UINT8 Request, UINT16 Index, UINT16 Value, PVOID TransferBuffer,UINT32 TransferBufferLength, int Dir);

int PCI_SB_DownloadBuffer(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuffer, SINT32 Size);
int PCI_SB_WriteInterrupt(PDEVICE_EXTENSION_COMMON pdxc, UINT16 Addr, UINT16 Data);
int PCI_SB_ReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc, UINT16 *pIntRetVal, UINT16 *pIntRetData);
int PCI_SB_ReadStream(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuf, SINT32 Size);

NTSTATUS PCI_SB_ResetPipe(PDEVICE_EXTENSION_COMMON pdxc, UINT32 PipeNum);
NTSTATUS PCI_SB_AbortPipe(PDEVICE_EXTENSION_COMMON pdxc, UINT32 PipeNum);

NTSTATUS PCI_SB_InitializeBus(PDEVICE_EXTENSION_COMMON pdxc);
NTSTATUS PCI_SB_UninitializeBus(PDEVICE_EXTENSION_COMMON pdxc);

int PCI_SB_StartReadVideoStream(PDEVICE_EXTENSION_COMMON pdxc);
int PCI_SB_StartReadAudioStream(PDEVICE_EXTENSION_COMMON pdxc);
int PCI_SB_StartReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc);

void PCI_SB_StopReadAllPipes(PDEVICE_EXTENSION_COMMON pdxc);

void PCI_SB_ResetBusDevice(PDEVICE_EXTENSION_COMMON pdxc);
NTSTATUS PCI_SB_SoftwareReset(PDEVICE_EXTENSION_COMMON pdxc);

void PCI_SB_Dma_Dpc(PKDPC Dpc, 
                    PVOID DeferredContext,
                    PVOID arg1,
                    PVOID arg2);

extern WISDRIVER_HW_FUNCTION_TABLE	wisdriver_pci_sb_function_table;

#ifdef __cplusplus
}
#endif


#endif // _PCI_BASE_H_

/********************************* end of pci_base.h *************************/
