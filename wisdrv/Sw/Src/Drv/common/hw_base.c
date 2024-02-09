/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        usb_base.c
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

#ifdef _WINDOWS
#include "wdm.h"
#endif

#include "platform.h"
#include "function.h"
#include "usb_base.h"

#ifdef _WIN_PCI
#include "pci_base.h"
#endif

#include "hw_base.h"

NTSTATUS
HW_BASE_SetupHwDispatch(
    PDEVICE_EXTENSION_COMMON pdxc
    )
/*++
 
Routine Description:

    This routine will setup the hw dispatch table for the WIS device.

Arguments:

	PDEVICE_EXTENSION_COMMON - pointer to a common extension.

Return Value:

    NTSTATUS - NT status value.

--*/
{

	NTSTATUS status = STATUS_SUCCESS;

    DEBUG_FUNC_ENTER("HW_BASE_SetupHwDispatch")

	if(pdxc->BusType == WisPci){
#ifdef _WIN_PCI
		if(pdxc->ChipType == SpongeBob){
			pdxc->hw_funcs.AbortPipe             = PCI_SB_AbortPipe           ; 
			pdxc->hw_funcs.DoVendorRequest       = PCI_SB_DoVendorRequest     ; 
			pdxc->hw_funcs.DownloadBuffer        = PCI_SB_DownloadBuffer      ; 
			pdxc->hw_funcs.InitializeBus         = PCI_SB_InitializeBus       ; 
			pdxc->hw_funcs.ReadInterrupt         = PCI_SB_ReadInterrupt       ; 
			pdxc->hw_funcs.ReadStream            = PCI_SB_ReadStream          ; 
			pdxc->hw_funcs.ResetBusDevice        = PCI_SB_ResetBusDevice      ; 
			pdxc->hw_funcs.ResetPipe             = PCI_SB_ResetPipe           ; 
			pdxc->hw_funcs.SoftwareReset         = PCI_SB_SoftwareReset       ;
			pdxc->hw_funcs.StartReadAudioStream  = PCI_SB_StartReadAudioStream; 
			pdxc->hw_funcs.StartReadInterrupt    = PCI_SB_StartReadInterrupt  ; 
			pdxc->hw_funcs.StartReadVideoStream  = PCI_SB_StartReadVideoStream; 
			pdxc->hw_funcs.StopReadAllPipes      = PCI_SB_StopReadAllPipes    ; 
			pdxc->hw_funcs.UninitializeBus       = PCI_SB_UninitializeBus     ; 
			pdxc->hw_funcs.WriteInterrupt        = PCI_SB_WriteInterrupt      ; 
		}else{
			if(pdxc->ChipType == Gomer){
				DEBUG_ERROR(("HW_BASE_SetupHwDispatch - Gomer not defined yet"));
				status = STATUS_UNSUCCESSFUL;
			}else{
				DEBUG_ERROR(("HW_BASE_SetupHwDispatch - Bad ChipType=%02x", pdxc->ChipType));
				status = STATUS_UNSUCCESSFUL;
			}
		}
#endif /* _WIN_PCI */
	}else{
		if(pdxc->BusType == WisUsb){
			if(pdxc->ChipType == SpongeBob){
				pdxc->hw_funcs.AbortPipe             = USB_SB_AbortPipe           ; 
				pdxc->hw_funcs.DoVendorRequest       = USB_SB_DoVendorRequest     ; 
				pdxc->hw_funcs.DownloadBuffer        = USB_SB_DownloadBuffer      ; 
				pdxc->hw_funcs.InitializeBus         = USB_SB_InitializeBus       ; 
				pdxc->hw_funcs.ReadInterrupt         = USB_SB_ReadInterrupt       ; 
				pdxc->hw_funcs.ReadStream            = USB_SB_ReadStream          ; 
				pdxc->hw_funcs.ResetBusDevice        = USB_SB_ResetBusDevice      ; 
				pdxc->hw_funcs.ResetPipe             = USB_SB_ResetPipe           ; 
				pdxc->hw_funcs.SoftwareReset         = USB_SB_SoftwareReset       ;
				pdxc->hw_funcs.StartReadAudioStream  = USB_SB_StartReadAudioStream; 
				pdxc->hw_funcs.StartReadInterrupt    = USB_SB_StartReadInterrupt  ; 
				pdxc->hw_funcs.StartReadVideoStream  = USB_SB_StartReadVideoStream; 
				pdxc->hw_funcs.StopReadAllPipes      = USB_SB_StopReadAllPipes    ; 
				pdxc->hw_funcs.UninitializeBus       = USB_SB_UninitializeBus     ; 
				pdxc->hw_funcs.WriteInterrupt        = USB_SB_WriteInterrupt      ; 
			}else{
				if(pdxc->ChipType == Gomer){
					DEBUG_ERROR(("HW_BASE_SetupHwDispatch - Gomer not defined yet"));
					status = STATUS_UNSUCCESSFUL;
				}else{
					DEBUG_ERROR(("HW_BASE_SetupHwDispatch - Bad ChipType=%02x", pdxc->ChipType));
					status = STATUS_UNSUCCESSFUL;
				}
			}
		}else{
			// Bad BusType
			DEBUG_ERROR(("HW_BASE_SetupHwDispatch - Bad BusType=%02x", pdxc->BusType));
			status = STATUS_UNSUCCESSFUL;
		}
	}

    DEBUG_FUNC_EXIT("HW_BASE_SetupHwDispatch")

    return status;
}

/****************************** end of hw_base.c *****************************/

