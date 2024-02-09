/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        function.c
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
/////////////////////////////////////////////////////////
//
// function.c
//
// Created by Jun Ding 9/19/2001

#include "platform.h"
#include "function.h"
#include "I2C.h"
#include "gpio.h"
#include "usb_base.h"
#include "Board.h"
#include "CfgFileParser.h"
#include "osd.h"

#if ( defined(_WINDOWS) && defined(DRIVER) )
	_CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);	   // <wchar.h>
#endif

ACCESS_FUNCTIONS AccessFunc = 
{
	(I2C_WRITEREGISTER)		I2C_WriteRegister, 
	(I2C_READREGISTER)		I2C_ReadRegister, 

	(I2C_BURSTWRITEREGISTER)I2C_BurstWriteRegister,
	(I2C_BURSTREADREGISTER)	I2C_BurstReadRegister,
	
	(SPI_WRITEREGISTER)		SPI_WriteRegister, 
	(SPI_READREGISTER)		SPI_ReadRegister, 

	(GPIO_WRITEPINS)		GPIO_WritePins,
	(GPIO_READPINS)			GPIO_ReadPins,

	(WRITECBUSREGFW)		WriteCBusRegFW,
	(READCBUSREGFW)			ReadCBusRegFW,

	(GETDWORDVALUEFROMREGISTRY)	GetDWORDValueFromRegistry,

	(BOARD_CALLBACK)		BoardCallBack_OnVSyncLost,
};

#ifdef _WINDOWS

	#define INIT_LOCK_OFFSET KeInitializeSpinLock(&context->OffsetLock)
	#define	LOCK_OFFSET	 KeAcquireSpinLock(&context->OffsetLock,&context->irqlOffsetLock)
	#define UNLOCK_OFFSET	 KeReleaseSpinLock(&context->OffsetLock,context->irqlOffsetLock)

#elif defined(_LINUX)

	#define INIT_LOCK_OFFSET spin_lock_init(&(context->OffsetLock))
	#define LOCK_OFFSET	 spin_lock_irqsave(&(context->OffsetLock), context->OffsetLockFlags)
	#define UNLOCK_OFFSET	 spin_unlock_irqrestore(&(context->OffsetLock), context->OffsetLockFlags)

#endif
typedef struct nvpair
{
	SINT8 nv_name[128];
	SINT8 nv_val[128];
} nvpair_t;

nvpair_t g_nvpair[300];
int 	 g_nvlen=0;


int GenInitialPacket(PDEVICE_EXTENSION_COMMON pdxc, 
		     TCFGVIDEOEX *video_config,
		     TVIDEOCFGFIX *fix_config,
		     UINT8 *pPack, SINT32 *PackLen,
		     UINT8 *pFontBitmaps, SINT32 lFontBitmapSize,
		     int reset_type);
int GetFixConfig(PDEVICE_EXTENSION_COMMON pdxc, TVIDEOCFGFIX *fix_config);
void FormatSysFileName(SINT8 *FileName);


// input parameter : InterruptData<0 -- don't compare IntRetData
//
// return 
// >=0 -- success, the value is IntRetData when InterruptData<0
// -1 -- timeout
// -2 -- error interrupt type

int CheckInterruptPipe(PDEVICE_EXTENSION_COMMON context, int InterruptType, int InterruptData)
{
	int r;
	USHORT IntRetVal, IntRetData;

    DEBUG_FUNC_ENTER("CheckInterruptPipe")
    DEBUG_INFO(("CheckInterruptPipe InterruptType=%08x InterruptData=%08x",InterruptType, InterruptData))

 	if(context->DeviceConnected == 0){
		DEBUG_FUNC_EXIT("CheckInterruptPipe0")
		return -1;
	}

	if(context->ReadInterruptState == 0)
	{
		r = (context->hw_funcs.ReadInterrupt)(context, &IntRetVal, &IntRetData);
		IntRetVal &= 0xfffe;

		PrintMsg(DEBUG_HARDWARE_MISC, ("ReadInterrupt return %04x %04x",IntRetVal,IntRetData));

		if (r == 0)
		{
			if( (IntRetVal==InterruptType) && ((InterruptData<0)||(IntRetData==InterruptData)) )
			{
				if(InterruptData<0){
					DEBUG_FUNC_EXIT("CheckInterruptPipe1")
					return IntRetData;
				}
				else{
					DEBUG_FUNC_EXIT("CheckInterruptPipe2")
					return 0;
				}
			}
			else
			{
				DEBUG_FUNC_EXIT("CheckInterruptPipe3")
				return -2;
			}
		}
		else
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("CheckInterruptPipe : Timeout"));
			DEBUG_FUNC_EXIT("CheckInterruptPipe4")
			return -1;
		}
	}

	else   // run time
	{
		int r;

		context->CheckInterruptType = InterruptType;
		context->CheckInterruptData = InterruptData;
		PIResetEvent(&(context->CheckInterruptEvent));

		r = PIWaitEvent(&(context->CheckInterruptEvent), 1000);

		if (r != 0) 
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("CheckInterruptPipe : Timeout"));
			DEBUG_FUNC_EXIT("CheckInterruptPipe5")
			return -1;
		}
		else
		{
			int r;

			if(InterruptData<0)
				r = context->CheckInterruptData;
			else
				r = 0;

			context->CheckInterruptType = -1;
			context->CheckInterruptData = -1;

			DEBUG_FUNC_EXIT("CheckInterruptPipe6")
			return r;
		}
	}
}

int PrepCheckInterruptPipe(PDEVICE_EXTENSION_COMMON context, int InterruptType, int InterruptData)
{
	DEBUG_FUNC_ENTER("PrepCheckInterruptPipe")
	context->CheckInterruptType = InterruptType;
	context->CheckInterruptData = InterruptData;
	PIResetEvent(&(context->CheckInterruptEvent));
	DEBUG_FUNC_EXIT("PrepCheckInterruptPipe")
	return 0;
}

int CheckInterruptPipe2(PDEVICE_EXTENSION_COMMON context, int InterruptType, int InterruptData)
{
	int r;

	DEBUG_FUNC_ENTER("PrepCheckInterruptPipe2")
	//PrintMsg(DEBUG_HARDWARE_MISC, ("CheckInterruptPipe2 : Enter"));

	r = PIWaitEvent(&(context->CheckInterruptEvent), 1000);

	if (r != 0) 
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("CheckInterruptPipe2 : Timeout"));
		DEBUG_FUNC_EXIT("PrepCheckInterruptPipe2 0")
		return -1;
	}
	else
	{
		int r;

		if(InterruptData<0)
			r = context->CheckInterruptData;
		else
			r = 0;

		context->CheckInterruptType = -1;
		context->CheckInterruptData = -1;

		DEBUG_FUNC_EXIT("PrepCheckInterruptPipe2 1")
		return r;
	}
}

//////  return 0 -- success

int BootupChip(PDEVICE_EXTENSION_COMMON context)
{
  // use a static buffer for firmware read - to prevent actual disk file read later
  static int SFirmlen = 0;
  static UINT8 SFirmware[64*1024];

	char FirmwareFileName[256] = "go7007fw.bin";
	char FirmwareFileName2[256] = "go7007fw_pf.bin";
	UINT8 *pFirmware;		
	int Firmlen;
	int j2,j1;
	j1=jiffies;
	DEBUG_FUNC_ENTER("BootupChip")
	PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : BusType=%d,BusMode=%d",
		context->BusType,context->BoardInfo.BusMode));
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

 	if(context->DeviceConnected == 0)
		return 1;

	Firmlen = 64*1024;
	pFirmware = PIAllocMem(Firmlen);

	if(pFirmware==NULL)
	{
	        PrintMsg(DEBUG_HARDWARE_MISC, ("Bootup : Alloc Memory Fail"));
	        return 2;
	}
	CHECK_INT_LOCK(context);

	if(context->BoardInfo.Capability & BOARD_PROP_DUALFIRMWARE)
	{
		FormatSysFileName(FirmwareFileName2);
		if(ReadFileToBuffer(FirmwareFileName2, pFirmware, &Firmlen)>0)
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("Bootup : Read firmware 2 file fail"));
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);
			return 10;
		} 

		(context->hw_funcs.WriteInterrupt)(context,0x0001,0x0001);
		
		PISleep(100);

		if(CheckInterruptPipe(context, 0x55aa, -1)<0)
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : Bootup Fail 0"));
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);			
			return 11;
		}

		if( (context->hw_funcs.DownloadBuffer)(context, pFirmware+16, Firmlen-16) != 0){
			DEBUG_ERROR(("BootupChip - Download firmware 0 Failed !"))
			DEBUG_FUNC_EXIT("BootupChip")
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);
			return 12;
		}

 		if(CheckInterruptPipe(context, 0x55aa, -1)<0)
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : Bootup Fail 0"));
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);			
			return 13;
		}
	}
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

	Firmlen = 64*1024;
	FormatSysFileName(FirmwareFileName);

	// do actual read from file only once to static buffer
	if(! SFirmlen ) {
	  if(ReadFileToBuffer(FirmwareFileName, SFirmware, &Firmlen)>0)
	    {
	      PrintMsg(DEBUG_HARDWARE_MISC, ("Bootup : Read firmware file fail"));
	      PIFreeMem(pFirmware);
	      CHECK_INT_UNLOCK(context);
	      return 3;
	    }
	  SFirmlen = Firmlen;
	}
	// all other times just copy from static to dynamic buffer
	Firmlen = SFirmlen;
	memcpy(pFirmware, SFirmware, Firmlen);
	
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

    if(context->BusType == WisUsb){
	    if(context->BoardInfo.BusMode==UM_USB_EZUSB)
		{
			(context->hw_funcs.WriteInterrupt)(context,0x0001,0x0001);
		
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
			PISleep(100);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
		
			// reset buffer in EzUsb, we should do it twice here
			(context->hw_funcs.DoVendorRequest)(context, 0x10, 0, 0, NULL, 0, DIR_OUT);
			(context->hw_funcs.DoVendorRequest)(context, 0x10, 0, 0, NULL, 0, DIR_OUT);
		}
		
		(context->hw_funcs.WriteInterrupt)(context,0x0001,0x0001);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
		
		PISleep(100);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

		if(CheckInterruptPipe(context, 0x55aa, -1)<0)
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : Bootup Fail 1"));
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);
			return 4;
		}
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
	}else{
        if(((context->hw_funcs.SoftwareReset)(context))!=STATUS_SUCCESS){
			DEBUG_ERROR(("BootupChip - Software Reset Failed !"))
			PIFreeMem(pFirmware);
			DEBUG_FUNC_EXIT("BootupChip")
			CHECK_INT_UNLOCK(context);			    
			return 5;
		}
	}

	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

    //	DownloadFirmware
	if( (context->hw_funcs.DownloadBuffer)(context, pFirmware+16, Firmlen-16) != 0){
		DEBUG_ERROR(("BootupChip - Download firmware Failed !"))
		PIFreeMem(pFirmware);
		CHECK_INT_UNLOCK(context);
		DEBUG_FUNC_EXIT("BootupChip")
		return 6;
	}
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));

    if(context->BusType == WisUsb){
		if(CheckInterruptPipe(context, 0x5a5a, -1)<0)
		{
			PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : Bootup Fail 2"));
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);
			return 7;
		}
	}else{
		USHORT int_ret_val, int_ret_data;
		int timeout = 0x10;

		(context->hw_funcs.ReadInterrupt)(context, &int_ret_val, &int_ret_data);

		while ((int_ret_val != 0x5a5a)&&(timeout)) {
			PISleep(100);
			(context->hw_funcs.ReadInterrupt)(context, &int_ret_val, &int_ret_data);
			timeout --;
		}
		if(timeout!=0){
			DEBUG_INFO(("BootupChip - Firmware download Successful"))
		}else{
			DEBUG_ERROR(("BootupChip - Firmware download failed !"))
			PIFreeMem(pFirmware);
			CHECK_INT_UNLOCK(context);			
			DEBUG_FUNC_EXIT("BootupChip")
			return 5;
		}
	}

    CHECK_INT_UNLOCK(context);
	
	PIFreeMem(pFirmware);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
    if (context->BoardInfo.Capability & BOARD_CAP_AUDIO)
    {
        unsigned long caps = 0;
        caps = context->BoardInterface->pIBoardAudio->GetAudioCapabilities(context,0);
        if (caps & CAP_AUDIO_I2S_MASTER_MODE)
        {
            if (WriteCBusRegFW(context,0x1000,0x0811) != 0)
            {
                PrintMsg(DEBUG_HARDWARE_MISC, ("WriteAudio 0x0811: Fail"));
                return 6;
            }
            if (WriteCBusRegFW(context,0x1000,0x0c11) != 0)
            {
                PrintMsg(DEBUG_HARDWARE_MISC, ("WriteAudio 0x0c11: Fail"));
                return 7;
            }
        }
    }

	context->BootupSuccess = 1;
	context->ChipState = DS_IDLE;
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("%s %d %d",
		__FUNCTION__, __LINE__, j2-j1));
	PrintMsg(DEBUG_HARDWARE_MISC, ("BootupChip : Success"));
    DEBUG_FUNC_EXIT("BootupChip")
	return 0;
}


int AllocateBuffers(PDEVICE_EXTENSION_COMMON context)
{

	int round;

#if defined(_WINDOWS) 	
	unsigned char AudioTryBufferSize[] = {22, 21, 20};
	int VideoTryBufferSize[] = {4096000, 2048000, 1024000};
#elif defined(_LINUX)
	unsigned char AudioTryBufferSize[] = {19, 18, 17};
	int VideoTryBufferSize[] = {512*1024, 256*1024, 128*1024};
#endif

	if(context->BufferAllocated == 1)
	{
		PrintMsg(DEBUG_ERROR_LEVEL|DEBUG_MEMORY, ("AllocateBuffers : Buffers already allocated"));
		return 0;
	}

	context->BufferAllocated = 1;


	for(round=0; round<3; round++)
	{
		if(context->DoAudio)
		{
			context->AudioStreamBuffer = CAudioStreamBuffer_Init(AudioTryBufferSize[round]);
			if(context->AudioStreamBuffer == NULL)
				continue;
		}

		if(context->DoVideo)
		{
			NTSTATUS ntStatus;

			ntStatus = StreamBuffer_Init(&(context->StreamBuffer),VideoTryBufferSize[round], 
										 Config_GetCurrentVideoConfig(&context->config_manager),
#ifdef NO_SJ_MODS  // Removed VBI_PRESENT to avoid false VBI data matches in video stream
										 VBI_PRESENT|FINGERPRINT_PRESENT|MAP_PRESENT);
#else
										 FINGERPRINT_PRESENT|MAP_PRESENT);
#endif

			if(ntStatus != STATUS_SUCCESS)
			{
				if(context->DoAudio)
					CAudioStreamBuffer_Uninit(context->AudioStreamBuffer);

				continue;
			}
		}

		break;
	}

	PrintMsg(DEBUG_AUDIO_STREAMBUFFER|DEBUG_VIDEO_STREAMBUFFER, ("Allocate buffer round : %d", round));

	if(round == 3)
	{
		context->BufferAllocated = 0;
		return 2;
	}

	context->pBlankPFrame = PIAllocMem(4096);
	context->pVBIBuffer = PIAllocMem(4096);
        context->pSeqHeader = PIAllocMem(64);

	return 0;
}

void ReleaseBuffers(PDEVICE_EXTENSION_COMMON context)
{
	if(context->BufferAllocated == 0)
		return;

	context->BufferAllocated = 0;

	if(context->DoAudio)
	{
		CAudioStreamBuffer* pSave = context->AudioStreamBuffer;
		context->AudioStreamBuffer = NULL;
		CAudioStreamBuffer_Uninit(pSave);
	}

	if(context->DoVideo)
		StreamBuffer_Uninit(&(context->StreamBuffer));

	PIFreeMem(context->pBlankPFrame);
	PIFreeMem(context->pVBIBuffer);
	PIFreeMem(context->pSeqHeader);
}

#ifdef _WIN_PCI

int FUNCTION_stop_raw_capture(PDEVICE_EXTENSION_COMMON context, ULONG channel)
{
    int status;

    DEBUG_FUNC_ENTER("FUNCTION_stop_raw_capture")

    switch(channel){
        case CHANNEL_AUDIO:
            if(context->AUDIO_RAW_BUFFER->_is_capturing){
                if(context->AUDIO_RAW_BUFFER->_write_byte_index > 0){
                    IO_STATUS_BLOCK      iosb;

                    DEBUG_INFO(("FUNCTION_stop_raw_capture Audio capture data size = %08x", context->AUDIO_RAW_BUFFER->_write_byte_index));

                    if (!NT_SUCCESS (status = ZwWriteFile(context->AUDIO_RAW_BUFFER->_capture_filehandle,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          &iosb,
                                                          context->AUDIO_RAW_BUFFER->_buffer,
                                                          context->AUDIO_RAW_BUFFER->_write_byte_index,
                                                          NULL,
                                                          NULL))){
                        DEBUG_ERROR(("FUNCTION_stop_raw_capture - Error Writing the AUDIO file !"))
                    }
                }
                if (!NT_SUCCESS (status = ZwClose (context->AUDIO_RAW_BUFFER->_capture_filehandle))){
                    DEBUG_ERROR(("FUNCTION_stop_raw_capture - Error Closing the AUDIO file !"))
                }
                PIFreeMem(context->AUDIO_RAW_BUFFER->_buffer);
                context->AUDIO_RAW_BUFFER->_is_capturing = FALSE;
            }
            break;
        case CHANNEL_VIDEO:
            if(context->VIDEO_RAW_BUFFER->_is_capturing){
                if(context->VIDEO_RAW_BUFFER->_write_byte_index > 0){
                    IO_STATUS_BLOCK      iosb;

                    DEBUG_INFO(("FUNCTION_stop_raw_capture Video capture data size = %08x", context->VIDEO_RAW_BUFFER->_write_byte_index));

                    if (!NT_SUCCESS (status = ZwWriteFile(context->VIDEO_RAW_BUFFER->_capture_filehandle,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          &iosb,
                                                          context->VIDEO_RAW_BUFFER->_buffer,
                                                          context->VIDEO_RAW_BUFFER->_write_byte_index,
                                                          NULL,
                                                          NULL))){
                        DEBUG_ERROR(("FUNCTION_stop_raw_capture - Error Writing the VIDEO file !"))
                    }
                }
                if (!NT_SUCCESS (status = ZwClose (context->VIDEO_RAW_BUFFER->_capture_filehandle))){
                    DEBUG_ERROR(("FUNCTION_stop_raw_capture - Error Closing the VIDEO file !"))
                }
                PIFreeMem(context->VIDEO_RAW_BUFFER->_buffer);
                context->VIDEO_RAW_BUFFER->_is_capturing = FALSE;
            }
            break;
        default:
            break;
    }

    DEBUG_FUNC_EXIT("FUNCTION_stop_raw_capture")

    return status;

}

int FUNCTION_setup_raw_capture(PDEVICE_EXTENSION_COMMON context, ULONG channel)
{
    NTSTATUS Status;
   	ULONG Length;
    ULONG BufferSize;
	UNICODE_STRING Str;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE DiskKeyHandle;
    union {PVOID buffer; PKEY_VALUE_PARTIAL_INFORMATION valueKeyPartialInf;} u;
    
    DEBUG_FUNC_ENTER("FUNCTION_setup_raw_capture")

    // Start out with one page of buffer
    BufferSize = PAGE_SIZE;

    // Get the registry entries for the capture settings. 
    while(1){
        u.buffer = PIAllocMem(BufferSize);
        if (u.buffer == NULL)
        {
            DEBUG_ERROR(("Error allocating u.buffer !"))
            context->VIDEO_RAW_BUFFER->_buffer_size = 0;
            context->AUDIO_RAW_BUFFER->_buffer_size = 0;
            break;
        }

        // Format Registry path
        swprintf(u.buffer,L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\WISTechVIDCAP\\RAW_CAPTURE");
        RtlInitUnicodeString(&Str, u.buffer);
        InitializeObjectAttributes(&ObjectAttributes, &Str, OBJ_CASE_INSENSITIVE, NULL, NULL);
        Status = ZwOpenKey(&DiskKeyHandle, KEY_READ, &ObjectAttributes);
        if (!NT_SUCCESS(Status)){
            DEBUG_ERROR(("Error opening registry key !"));
            context->VIDEO_RAW_BUFFER->_buffer_size = 0;
            context->AUDIO_RAW_BUFFER->_buffer_size = 0;
            PIFreeMem(u.buffer);
            break;
        }

        RtlInitUnicodeString(&Str, L"VideoSize");
        Status = ZwQueryValueKey(DiskKeyHandle, &Str, KeyValuePartialInformation,
                                    u.valueKeyPartialInf, BufferSize, &Length);
        if (!NT_SUCCESS(Status) || (u.valueKeyPartialInf->Type != REG_DWORD)){
            DEBUG_ERROR(("Error finding the VideoSize registry entry !"))
            context->VIDEO_RAW_BUFFER->_buffer_size = 0;
        }else{
            context->VIDEO_RAW_BUFFER->_buffer_size = *(ULONG*)&(u.valueKeyPartialInf->Data);
        }
        RtlInitUnicodeString(&Str, L"AudioSize");
        Status = ZwQueryValueKey(DiskKeyHandle, &Str, KeyValuePartialInformation,
                                    u.valueKeyPartialInf, BufferSize, &Length);
        if (!NT_SUCCESS(Status) || (u.valueKeyPartialInf->Type != REG_DWORD)){
            DEBUG_ERROR(("Error finding the AudioSize registry entry !"))
            context->AUDIO_RAW_BUFFER->_buffer_size = 0;
        }else{
            context->AUDIO_RAW_BUFFER->_buffer_size = *(ULONG*)&(u.valueKeyPartialInf->Data);
        }

        if(context->VIDEO_RAW_BUFFER->_buffer_size){
            RtlInitUnicodeString(&Str, L"VideoFilename");
            Status = ZwQueryValueKey(DiskKeyHandle, &Str, KeyValuePartialInformation,
                                        u.valueKeyPartialInf, BufferSize, &Length);
            if (!NT_SUCCESS(Status) || (u.valueKeyPartialInf->Type != REG_SZ)){
                DEBUG_ERROR(("Error finding the VideoFilename registry entry !"))
                context->VIDEO_RAW_BUFFER->_buffer_size = 0;
            }
            // Retrieve the value
            if ((((PWSTR)u.valueKeyPartialInf->Data)[0] == L'\\') && 
                (((PWSTR)u.valueKeyPartialInf->Data)[1] == L'?')){
                // Filename in the Reqistry already consists "\??\" prefix
                wcscpy(context->VIDEO_RAW_BUFFER->_capture_filename, (PWSTR)&(u.valueKeyPartialInf->Data));
            }else{
                // Insert prefix
                wcscpy(context->VIDEO_RAW_BUFFER->_capture_filename, L"\\??\\");
                wcscat(context->VIDEO_RAW_BUFFER->_capture_filename, (PWSTR)&(u.valueKeyPartialInf->Data));
            }
            RtlInitUnicodeString(&context->VIDEO_RAW_BUFFER->_unicode_file_name, (PCWSTR) context->VIDEO_RAW_BUFFER->_capture_filename);
        }

        if(context->AUDIO_RAW_BUFFER->_buffer_size){
            RtlInitUnicodeString(&Str, L"AudioFilename");
            Status = ZwQueryValueKey(DiskKeyHandle, &Str, KeyValuePartialInformation,
                                        u.valueKeyPartialInf, BufferSize, &Length);
            if (!NT_SUCCESS(Status) || (u.valueKeyPartialInf->Type != REG_SZ)){
                DEBUG_ERROR(("Error finding the AudioFilename registry entry !"))
                context->AUDIO_RAW_BUFFER->_buffer_size = 0;
            }
            // Retrieve the value
            if ((((PWSTR)u.valueKeyPartialInf->Data)[0] == L'\\') && 
                (((PWSTR)u.valueKeyPartialInf->Data)[1] == L'?')){
                // Filename in the Reqistry already consists "\??\" prefix
                wcscpy(context->AUDIO_RAW_BUFFER->_capture_filename, (PWSTR)&(u.valueKeyPartialInf->Data));
            }else{
                // Insert prefix
                wcscpy(context->AUDIO_RAW_BUFFER->_capture_filename, L"\\??\\");
                wcscat(context->AUDIO_RAW_BUFFER->_capture_filename, (PWSTR)&(u.valueKeyPartialInf->Data));
            }
            RtlInitUnicodeString(&context->AUDIO_RAW_BUFFER->_unicode_file_name, (PCWSTR) context->AUDIO_RAW_BUFFER->_capture_filename);
        }
        ZwClose(DiskKeyHandle);
        PIFreeMem(u.buffer);
        break;
    }

    switch(channel){
        case CHANNEL_AUDIO:
            if(context->AUDIO_RAW_BUFFER->_buffer_size != 0){
                // Allocate the capture buffer.
                context->AUDIO_RAW_BUFFER->_buffer = PIAllocMem(context->AUDIO_RAW_BUFFER->_buffer_size);
                if(context->AUDIO_RAW_BUFFER->_buffer==NULL){
                    DEBUG_ERROR(("Error allocating AUDIO_RAW_BUFFER !"))
                    break;
                }
                context->AUDIO_RAW_BUFFER->_read_byte_index = 0;
                context->AUDIO_RAW_BUFFER->_write_byte_index = 0;
                {
                    OBJECT_ATTRIBUTES    obj;
                    IO_STATUS_BLOCK      iosb;

                    InitializeObjectAttributes (&obj, &context->AUDIO_RAW_BUFFER->_unicode_file_name, OBJ_CASE_INSENSITIVE, NULL, NULL);

                    if (!NT_SUCCESS (Status = ZwCreateFile(&context->AUDIO_RAW_BUFFER->_capture_filehandle,
                                                          SYNCHRONIZE | FILE_READ_DATA | FILE_WRITE_DATA,
                                                          &obj,
                                                          &iosb,
                                                          0,
                                                          0,
                                                          FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                                          FILE_OVERWRITE_IF,
                                                          FILE_SYNCHRONOUS_IO_NONALERT,
                                                          NULL,
                                                          0))){
                        DEBUG_ERROR(("Error opening the AUDIO file !"));
                    }else{
                        context->AUDIO_RAW_BUFFER->_is_capturing = TRUE;
                    }
                }
            }else{
                context->AUDIO_RAW_BUFFER->_is_capturing = FALSE;
            }
            break;
        case CHANNEL_VIDEO:
            if(context->AUDIO_RAW_BUFFER->_buffer_size != 0){
                // Allocate the capture buffer.
                context->VIDEO_RAW_BUFFER->_buffer = PIAllocMem(context->VIDEO_RAW_BUFFER->_buffer_size);
                if(context->VIDEO_RAW_BUFFER->_buffer==NULL){
                    DEBUG_ERROR(("Error allocating VIDEO_RAW_BUFFER !"))
                    break;
                }
                context->VIDEO_RAW_BUFFER->_read_byte_index = 0;
                context->VIDEO_RAW_BUFFER->_write_byte_index = 0;
                {
                    OBJECT_ATTRIBUTES    obj;
                    IO_STATUS_BLOCK      iosb;

                    InitializeObjectAttributes (&obj, &context->VIDEO_RAW_BUFFER->_unicode_file_name, OBJ_CASE_INSENSITIVE, NULL, NULL);

                    if (!NT_SUCCESS (Status = ZwCreateFile(&context->VIDEO_RAW_BUFFER->_capture_filehandle,
                                                          SYNCHRONIZE | FILE_READ_DATA | FILE_WRITE_DATA,
                                                          &obj,
                                                          &iosb,
                                                          0,
                                                          0,
                                                          FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                                          FILE_OVERWRITE_IF,
                                                          FILE_SYNCHRONOUS_IO_NONALERT,
                                                          NULL,
                                                          0))){
                        DEBUG_ERROR(("Error opening the VIDEO file !"));
                    }else{
                        context->VIDEO_RAW_BUFFER->_is_capturing = TRUE;
                    }
                }
            }else{
                context->VIDEO_RAW_BUFFER->_is_capturing = FALSE;
            }
            break;
        default:
            break;
    }
    
    DEBUG_FUNC_EXIT("FUNCTION_setup_raw_capture")

    return STATUS_SUCCESS;
}

#endif /* _WIN_PCI */


#if defined(_WINDOWS) 
#define CLEAN_ON_ERROR								\
	{												\
		if(context->fix_config.osdcfg.DoOSD==1)		\
		{											\
			OSD_Uninit(context->hosd);				\
			context->hosd = 0;						\
		}											\
		if(pInitPack!=NULL)							\
			PIFreeMem(pInitPack);					\
	    ReleaseBuffers(context);					\
	    context->ChipState=DS_IDLE;					\
	}
#endif


// return 0: success
int InitializeDevice(PDEVICE_EXTENSION_COMMON context)
{
	UINT8 *pInitPack;		
	int PackLen;
	int r, i;
	int channel_id;
	unsigned long target_rate;
	int VideoConfigValid, AudioConfigValid;
	char *pFontBitmaps;
	long lFontBitmapSize;
	TCFGVIDEOEX *video_config, *video_config1;
	int j1,j2;

	VideoConfigValid = Config_IsVideoConfigValid(&context->config_manager);
	AudioConfigValid = Config_IsAudioConfigValid(&context->config_manager);
	j1=jiffies;
	
	j2=jiffies;

	DEBUG_FUNC_ENTER("InitializeDevice")
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : Enter %d", j2-j1));

	if(context->DeviceConnected == 0)
		return 0;

	if(context->BootupSuccess==0 || 
	   context->ChipState!=DS_IDLE || 
	   (context->DoVideo && VideoConfigValid==0) ||
	   (context->DoAudio && AudioConfigValid==0) ||	
	   context->BufferAllocated==0 ||
  	   context->BoardInterface == NULL)
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("%d %d %d %d %d %d %d %08x", 
			context->BootupSuccess, 
			context->ChipState, 
			context->DoVideo, VideoConfigValid,
			context->DoAudio, AudioConfigValid,
			context->BufferAllocated,
			(UINT32)(context->BoardInterface)));
		DEBUG_FUNC_EXIT("InitializeDevice 0")
		return 1;
	}

	context->ChipState = DS_STARTING;

	context->IFrameNum = 0;
	context->PFrameNum = 0;

	context->TotalDelay = 0;
	context->ChipDelay = 0;

	context->LastChipFrameNum = 0;
	context->RollOverTimes = 0;
	context->ChipFrameNum = 0;

	if(context->DoAudio && context->BoardInterface->pIBoardAudio!=NULL)
	{
  	        InitBoardInterface(context);
		context->BoardInterface->pIBoardAudio->InitializeAudio(
				context->BoardContext,
				(AVSTREAM_COOKIE)context,
	       		Config_GetCurrentAudioConfig(&context->config_manager)
			);
	}
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : %d %d", 
			__LINE__,j2-j1));

	GetFixConfig(context, &(context->fix_config));
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : %d %d", 
			__LINE__,j2-j1));

	if(context->fix_config.osdcfg.DoOSD==1)
	{
		TCFGVIDEOEX* video_config;
		char FileName[256]="fix_setting.txt";

		FormatSysFileName(FileName);
		video_config = Config_GetCurrentVideoConfig(&context->config_manager);

#if defined(_WINDOWS) 
		OSD_Init(&(context->hosd),
				FileName,
				&pFontBitmaps,
				&lFontBitmapSize,
				context,
				video_config->rescfg.width,
				video_config->rescfg.height);
#endif

	}
	else
	{

		pFontBitmaps = 0;
		lFontBitmapSize = 0;
	}

	PackLen = 96*1024;
	pInitPack = PIAllocMem(PackLen);
    
	if(pInitPack==NULL)
	{
	    PrintMsg(DEBUG_HARDWARE_MISC,
		     ("InitializeDevice : Alloc Memory Fail"));
#if defined(_WINDOWS)
	    CLEAN_ON_ERROR;
#endif
	    DEBUG_FUNC_EXIT("InitializeDevice 1")
	    return 2;
	}
    
	video_config = PIAllocMem(sizeof(TCFGVIDEOEX));
	video_config1 = Config_GetCurrentVideoConfig(&context->config_manager);

	*video_config = *video_config1;
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : %d %d", 
			__LINE__,j2-j1));

	context->BoardInterface->pIBoardVideo->OnStartCapture(
			context->BoardContext,
			(AVSTREAM_COOKIE)context,
			video_config, 
			context->config_manager._sensor_config
		);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : %d %d", 
			__LINE__,j2-j1));

	if(GenInitialPacket(context, video_config, &(context->fix_config),
			    pInitPack, &PackLen,
			    pFontBitmaps, lFontBitmapSize,0)>0)
	{
	    PrintMsg(DEBUG_HARDWARE_MISC,
		     ("Initialize : Generate packets fail"));
#if defined(_WINDOWS)
		CLEAN_ON_ERROR;
#endif
		PIFreeMem(video_config);
		DEBUG_FUNC_EXIT("InitializeDevice 2")
	    return 3;	
	}
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : %d %d", 
			__LINE__,j2-j1));

	//OSD Initialization
	init_osd(context,video_config->rescfg.width,video_config->rescfg.height);
		
	PIFreeMem(video_config);

	CHECK_INT_LOCK(context);
	
// initialize chip
    if(context->BusType == WisUsb)
	{
	    r = (context->hw_funcs.DownloadBuffer)(context, pInitPack, PackLen);
        
	    if (r != 0)
	    {
#if defined(_WINDOWS)
			CLEAN_ON_ERROR;
#endif
                DEBUG_FUNC_EXIT("InitializeDevice 3")
		CHECK_INT_UNLOCK(context);
	    	return 4;	
	    }
        
	    if(CheckInterruptPipe(context, 0x0000, -1)<0)
	    {
	    	PrintMsg(DEBUG_HARDWARE_MISC, ("Initialize : fail"));
#if defined(_WINDOWS)
			CLEAN_ON_ERROR;
#endif
		CHECK_INT_UNLOCK(context);		
		DEBUG_FUNC_EXIT("InitializeDevice 4")
	    	return 5;	
	    }
    }
        CHECK_INT_UNLOCK(context);		

	if(context->DoVideo)
	{
		for(i=0; i<context->TotalVideoChannel; i++) // last channel is only for getting frame info
		{
			StreamBuffer_OpenChannel(&(context->StreamBuffer), &channel_id);
			if(context->VideoSBCallBack)
				StreamBuffer_SetChannelCallBack(&(context->StreamBuffer), i, context->VideoSBCallBack, context->SBCallBackContext);
		}


		target_rate = Config_GetVideoTargetBitrate(&context->config_manager);

		if(target_rate==0)
			context->BlockSize = 4096;
		else if(target_rate<=512*1000)
			context->BlockSize = 512;
		else if(target_rate<=1024*1000)
			context->BlockSize = 1024;
		else if(target_rate<=2048*1000)
			context->BlockSize = 2048;
		else if(target_rate<=4096*1000)
			context->BlockSize = 4096;
		else
			context->BlockSize = 8192;

		if(context->BlockSize > ((int) context->ReadStreamMaxSize))
			context->BlockSize = (int) context->ReadStreamMaxSize;

	j2=jiffies;
		PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice: Start Video, BlockSize : %d %d", context->BlockSize,j2-j1));

		PIResetEvent(&(context->StreamReady));

        DEBUG_PRINT(DEBUG_START_STOP,("InitializeDevice - Allocating context->pReadStreamBlock\n")); 
        context->pReadStreamBlock = PIAllocMem(context->BlockSize);

		(context->hw_funcs.StartReadVideoStream)(context);

	}

	if(context->DoAudio)
	{
#ifdef _WINDOWS
		unsigned long lAudioBufferLen =0;
#endif

		CAudioStreamBuffer_SetCallback(context->AudioStreamBuffer,context->AudioSBCallBack, context->SBCallBackContext);

	j2=jiffies;
		PrintMsg(DEBUG_HARDWARE_MISC, ("Start Audio %d", j2-j1));

		context->AudioBlockSize = 1024;

		if(context->AudioBlockSize > ((int) context->ReadAudioStreamMaxSize))
			context->AudioBlockSize = (int) context->ReadAudioStreamMaxSize;

		PIResetEvent(&(context->AudioStreamReady));

#if defined(_WINDOWS)
		lAudioBufferLen = CAudioStreamBuffer_Alloc(context->AudioStreamBuffer,&context->pReadAudioStreamBlock,context->AudioBlockSize);

		//	PrintMsg(DEBUG_HARDWARE_MISC, ("***************Audio->FreeAreaHead:%d,DataHead:%d",
		//		context->AudioStreamBuffer->m_lFreeAreaHeadPtr,context->AudioStreamBuffer->m_lDataAreaHeadPtr));
		if( lAudioBufferLen == 0)
		{
			//TODO:Audio buffer is full
		}
#elif defined(_LINUX)
		context->pReadAudioStreamBlock = PIAllocMem(context->AudioBlockSize);
#endif

		(context->hw_funcs.StartReadAudioStream)(context);
	}

#ifdef _WIN_PCI

    if(context->BusType == WisPci){
        // Support for RAW captures.
        if(context->DoVideo){
            FUNCTION_setup_raw_capture(context, CHANNEL_VIDEO);
        }

        if(context->DoAudio){
            FUNCTION_setup_raw_capture(context, CHANNEL_AUDIO);
        }

        r = (context->hw_funcs.DownloadBuffer)(context, pInitPack, PackLen);
 
        if (r != 0)
        {
#if defined(_WINDOWS)
			CLEAN_ON_ERROR;
#endif
            DEBUG_FUNC_EXIT("InitializeDevice 3")
            return 4;	
        }

	if(CheckInterruptPipe(context, 0x0000, -1)<0)
        {
            PrintMsg(DEBUG_HARDWARE_MISC, ("Initialize : fail"));
#if defined(_WINDOWS)
			CLEAN_ON_ERROR;
#endif
            DEBUG_FUNC_EXIT("InitializeDevice 4")
            return 5;	
        }
    }

#endif /* _WIN_PCI */
    
	PIFreeMem(pInitPack);

	context->pReadInterruptBlock = PIAllocMem(4);
	(context->hw_funcs.StartReadInterrupt)(context);

	context->ChipState=DS_RUNNING;

	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("InitializeDevice : Completed %d", j2-j1));
    DEBUG_FUNC_EXIT("InitializeDevice 5")
#if 0
    //init OSD display
    osd_string_t osd_str[2] =
    {
	    {1,1,"San Jose, CA"},
	    {1,4,"Good"}
    };


    //Display string
    get_osd_frame(1,&osd_str);
    show_osd_frame(0);
#endif    
    return 0;
}

int UninitializeDevice(PDEVICE_EXTENSION_COMMON context)
{
	int i;

	PrintMsg(DEBUG_HARDWARE_MISC, ("UninitializeDevice : Enter"));

	// to allow recovery from failed start - DS_STARTING - state
	//	if(context->ChipState!=DS_RUNNING)
	if(context->ChipState == DS_IDLE)
		return 0;

	if(context->DeviceConnected == 0)
		return 0;

	context->ChipState = DS_STOPPING;

	(context->hw_funcs.StopReadAllPipes)(context);

#ifdef _WIN_PCI

    if(context->BusType == WisPci){
        if(context->DoVideo){
            FUNCTION_stop_raw_capture(context, CHANNEL_VIDEO);
        }

        if(context->DoAudio){
            FUNCTION_stop_raw_capture(context, CHANNEL_AUDIO);
        }
    }

#endif

	if(context->DoVideo)
	{
		for(i=0; i<context->TotalVideoChannel; i++)
			StreamBuffer_CloseChannel(&(context->StreamBuffer), i);

        DEBUG_PRINT(DEBUG_START_STOP,("InitializeDevice - Freeing context->pReadStreamBlock\n")); 
		PIFreeMem(context->pReadStreamBlock);
	}

	if(context->DoAudio)
	{
#ifdef _LINUX
		PIFreeMem(context->pReadAudioStreamBlock);
#endif
	}
	PIFreeMem(context->pReadInterruptBlock);

	context->BoardInterface->pIBoardVideo->OnStopCapture(
		context->BoardContext,
		(AVSTREAM_COOKIE)context);

	if(context->fix_config.osdcfg.DoOSD==1)
	{
#if defined(_WINDOWS) 
		OSD_Uninit(context->hosd);
#endif
		context->hosd = 0;
	}

	BootupChip(context);

	PrintMsg(DEBUG_HARDWARE_MISC, ("UninitializeDevice : Left"));
	return 0;
}

void ReadVideoStreamCallBack(PDEVICE_EXTENSION_COMMON context)
{
	int r;

    //JJV DEBUG_FUNC_ENTER("ReadVideoStreamCallBack")
	//DumpBuffer(context->pReadStreamBlock, 16);

	r = StreamBuffer_AddBlock(&(context->StreamBuffer),context->pReadStreamBlock, context->BlockSize);
	
//	PrintMsg(DEBUG_HARDWARE_MISC, ("ReadStreamPipeCompletion %d", r));

	if(r<0)
		PrintMsg(DEBUG_HARDWARE_MISC, ("!!!!!!!!!!!!!!!!!! StreamBuffer_AddBlock Error"));

	// set the Stream ready event, a block reading is waiting for it
	r = StreamBuffer_GetChannelFrameCount(&(context->StreamBuffer),0); 
	if(r>0)
		PISetEvent(&(context->StreamReady));

    //JJV DEBUG_FUNC_EXIT("ReadVideoStreamCallBack")

}

void ReadAudioStreamCallBack(PDEVICE_EXTENSION_COMMON context)
{
	ULONG lAudioBufferLen;
	UINT8 *pAudioBlock;

	//DumpBuffer(context->pReadAudioStreamBlock, 16);

#if defined(_WINDOWS)
	CAudioStreamBuffer_Commit(context->AudioStreamBuffer,context->pReadAudioStreamBlock,context->AudioBlockSize);
#elif defined(_LINUX)
	lAudioBufferLen = CAudioStreamBuffer_Alloc(context->AudioStreamBuffer,&pAudioBlock,context->AudioBlockSize);

	PICopyMem(pAudioBlock, context->pReadAudioStreamBlock, context->AudioBlockSize);
	CAudioStreamBuffer_Commit(context->AudioStreamBuffer,pAudioBlock,context->AudioBlockSize);
#endif

//	PrintMsg(DEBUG_HARDWARE_MISC, ("**********Commit*****Audio->FreeAreaHead:%d,DataHead:%d,Size:%d",
//		context->AudioStreamBuffer->m_lFreeAreaHeadPtr,context->AudioStreamBuffer->m_lDataAreaHeadPtr,
//		context->AudioBlockSize));

	// issue another request for reading stream

#ifdef _WINDOWS
	lAudioBufferLen = CAudioStreamBuffer_Alloc(context->AudioStreamBuffer,&context->pReadAudioStreamBlock,context->AudioBlockSize);

//	PrintMsg(DEBUG_HARDWARE_MISC, ("*********Alloc******Audio->FreeAreaHead:%d,DataHead:%d",
//		context->AudioStreamBuffer->m_lFreeAreaHeadPtr,context->AudioStreamBuffer->m_lDataAreaHeadPtr));
	
	if( lAudioBufferLen == 0)
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("Buffer is full"));
	}
#endif
}

void ReadInterruptCallBack(PDEVICE_EXTENSION_COMMON context)
{
	USHORT	IntRetData, IntRetVal;					

	IntRetData = (((UINT16)(context->pReadInterruptBlock[1]))<<8) + context->pReadInterruptBlock[0];
	IntRetVal  = (((UINT16)(context->pReadInterruptBlock[3]))<<8) + context->pReadInterruptBlock[2];

	PrintMsg(DEBUG_HARDWARE_MISC, ("ReadInterruptPipeCompletion : IntRetVal %04x, IntRetData %04x", IntRetVal, IntRetData));

	IntRetVal &= 0xfffe;

	if ((context->MDInterruptFlag == 1)&&(IntRetVal==0x4d44))
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("Got Motion Detect Interrupt!!"));
//		PISetEvent(&(context->MDInterruptEvent));
	}
	// find out if it is the interrupt that we are waiting for
	if(context->CheckInterruptType != -1)
	{
		if (IntRetVal == context->CheckInterruptType)
		{
			if ((context->CheckInterruptData==-1 || (context->CheckInterruptData&0xfffe)==IntRetData) )
			{
				context->CheckInterruptData = IntRetData;
				PISetEvent(&(context->CheckInterruptEvent));
			}
		}
		else
			PrintMsg(DEBUG_HARDWARE_MISC, ("Completion : context->CheckInterruptType=%x,IntRetVal=%x",context->CheckInterruptType,IntRetVal));
	}
}


// return	0 -- success
//			1 -- download buffer error

int WriteCBusRegFW(PDEVICE_EXTENSION_COMMON context, UINT16 Addr, UINT16 Data)
{
	int r;

	DEBUG_FUNC_ENTER("WriteCBusRegFW")
	PrintMsg(DEBUG_HARDWARE_MISC,
		 ("WriteCBusRegFW : 0x%04x, 0x%04x", Addr, Data));
	CBUS_WRITE_LOCK(context);
	
	r = (context->hw_funcs.WriteInterrupt)(context, (UINT16)(Addr|0x8000), Data);
	if(r != 0)
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("WriteCBusRegFW WriteInterrupt error"));
		DEBUG_FUNC_EXIT("WriteCBusRegFW0")
		CBUS_WRITE_UNLOCK(context);
		return 1;
	}

	PrintMsg(DEBUG_HARDWARE_MISC, ("WriteCBusRegFW success"));
	DEBUG_FUNC_EXIT("WriteCBusRegFW1")
	CBUS_WRITE_UNLOCK(context);
	return 0;
}

// return	0 -- success
//			1 -- download buffer error
//			2 -- read interrupt error
//			3 -- error interrupt type

int ReadCBusRegFW(PDEVICE_EXTENSION_COMMON context, UINT16 Addr, UINT16 *pData)
{
	int r;
	int RetData;

	DEBUG_FUNC_ENTER("ReadCBusRegFW")
	PrintMsg(DEBUG_HARDWARE_MISC, ("ReadCBusRegFW, 0x%04x",Addr));

	CHECK_INT_LOCK(context);		

	if(context->ReadInterruptState == 1)	// run time
		PrepCheckInterruptPipe(context, 0xa000, -1);

	r = (context->hw_funcs.WriteInterrupt)(context, 0x0010, Addr);
	if(r != 0)
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("ReadCBusRegFW WriteInterrupt error"));
		CHECK_INT_UNLOCK(context);
		DEBUG_FUNC_EXIT("ReadCBusRegFW 0")
		return 1;
	}
		
	if(context->ReadInterruptState == 0)	// init time
		RetData = CheckInterruptPipe(context, 0xa000, -1);
	else
		RetData = CheckInterruptPipe2(context, 0xa000, -1);

	if(RetData >= 0)
	{
		*pData = (UINT16)RetData;
	}
	else
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("ReadCBusRegFW CheckInterruptPipe error"));
		CHECK_INT_UNLOCK(context);		
		DEBUG_FUNC_EXIT("ReadCBusRegFW 1")
		return 2;
	}

	CHECK_INT_UNLOCK(context);
	
	PrintMsg(DEBUG_HARDWARE_MISC, ("ReadCBusRegFW success, 0x%04x", *pData));
	DEBUG_FUNC_EXIT("ReadCBusRegFW 2")
	return 0;
}

int WriteHPIRegister(PDEVICE_EXTENSION_COMMON context, UINT16 Addr, UINT16 Data)
{
    int r;
	
    PrintMsg(DEBUG_HARDWARE_MISC, ("WriteHPIRegister : Enter (Addr %04x, Data %04x)", Addr, Data));    
	r = (context->hw_funcs.DoVendorRequest)(context, 0x12, Addr, Data, NULL, 0, DIR_OUT);
    PrintMsg(DEBUG_HARDWARE_MISC, ("WriteHPIRegister : Exit (Status %d)", r));

	return r;
}

int ReadHPIRegister(PDEVICE_EXTENSION_COMMON context, UINT16 Addr, UINT16 *pData)
{
    int r;
	
    PrintMsg(DEBUG_HARDWARE_MISC, ("ReadHPIRegister : Enter (Addr %04x)", Addr));    
	r = (context->hw_funcs.DoVendorRequest)(context, 0x14, Addr, 0, pData, 2, DIR_IN);
    PrintMsg(DEBUG_HARDWARE_MISC, ("ReadHPIRegister : Exit (Status %d, Data %04x)", r, *pData));  

	return r;
}

void ClearOnClose(PDEVICE_EXTENSION_COMMON context)
{
	if(context->ChipState != DS_IDLE || context->OpenHandleCount < 1)
		return;

	PrintMsg(DEBUG_HARDWARE_MISC, ("ClearOnClose"));

#if defined(_WINDOWS)
//	PIReleaseNotificationEvent(context->NotifyEvent);
#endif

    context->OpenHandleCount --;
}


SINT32 OnCreate(PDEVICE_EXTENSION_COMMON context)
{
	if( /*context->OpenHandleCount>2  ||*/ 
		context->BootupSuccess==0   || 
		context->ChipState!=DS_IDLE ||
		context->BoardInterface == NULL)
		return 1;

    context->OpenHandleCount ++;

	return 0;
}

SINT32 OnClose(PDEVICE_EXTENSION_COMMON context)
{
    PrintMsg(DEBUG_HARDWARE_MISC, ("OnClose - begins, ChipState %d, OpenHandleCount %d",
		context->ChipState, context->OpenHandleCount ));

    //	if(context->ChipState == DS_RUNNING)
	if(context->ChipState != DS_IDLE)
		UninitializeDevice(context);

	if(context->ChipState != DS_IDLE || context->OpenHandleCount < 1)
		return 1;

	ClearOnClose(context);
	return 0;
}

SINT32 OnSurpriseRemoval(PDEVICE_EXTENSION_COMMON context)
{
	PrintMsg(DEBUG_HARDWARE_MISC, ("OnSurpriseRemoval"));
	context->DeviceConnected = 0;

#if defined(_WINDOWS)

//	if(context->NotifyEvent != NULL)
//	{
//		context->NotifyType = NT_DEVICEREMOVED;
//		context->NotifyData = 0;
//		PISetEvent(context->NotifyEvent);
//	}

#endif

    //	if(context->ChipState == DS_RUNNING)
	if(context->ChipState != DS_IDLE)
		UninitializeDevice(context);

	if(context->OpenHandleCount > 0)
		ClearOnClose(context);

	return 0;
}

SINT32 OnRemoveDevice(PDEVICE_EXTENSION_COMMON context)
{
	PrintMsg(DEBUG_HARDWARE_MISC, ("OnRemoveDevice"));
    //	if(context->ChipState == DS_RUNNING)
	if(context->ChipState != DS_IDLE)
		UninitializeDevice(context);

	if(context->OpenHandleCount > 0)
		ClearOnClose(context);

#if defined(_WINDOWS)
	PIFreeMem(context->Interface);
	Config_Uninitialize(&context->config_manager);
#endif

	return 0;
}

SINT32 OnAddDevice(PDEVICE_EXTENSION_COMMON context)
{
#if defined(_WINDOWS)
	context->Interface = NULL;
#endif
	// set when an interrupt for packet return occurs
	PIInitializeEvent(&(context->StreamReady));        

	// set when an interrupt for packet return occurs
	PIInitializeEvent(&(context->AudioStreamReady));        

#if defined(_WINDOWS)
//	context->NotifyEvent = NULL;
#endif
	
	context->BootupSuccess = 0;

    context->OpenHandleCount = 0;
	context->BoardContext = NULL;

	context->DeviceConnected = 1;

	context->ReadStreamState = 0;
	context->ReadInterruptState = 0;

	PIInitializeEvent(&(context->CheckInterruptEvent));
	context->CheckInterruptType = -1;
	context->CheckInterruptData = -1;

	context->VideoSBCallBack = NULL;
	context->AudioSBCallBack = NULL;

	context->BufferAllocated = 0;

	PIInitializeEvent(&(context->ReadStreamStopEvent));
	PIInitializeEvent(&(context->ReadAudioStreamStopEvent));
	PIInitializeEvent(&(context->ReadInterruptStopEvent));
#if defined(__AVSYNC__)
	PIInitializeEvent(&(context->AVSynchStopEvent));
#endif	
	context->hosd = 0;

#if defined(_LINUX)
	init_MUTEX(&context->i2c_lock);
	init_MUTEX(&context->check_int_lock);
	init_MUTEX(&context->cbus_write_lock);		
#endif	
	
	return 0;
}

void InitBoardInterface(PDEVICE_EXTENSION_COMMON context)
{
	BOARD_INTERFACE **BoardInterfaces;
	UINT32 BoardNum, i;

    DEBUG_FUNC_ENTER("InitBoardInterface")

	GetBoardModuleCaps((AVSTREAM_COOKIE)context,&AccessFunc, &BoardInterfaces, &BoardNum);

	PrintMsg(DEBUG_HARDWARE_MISC,("--------- Board Supported"));
	for(i=0;i<BoardNum;i++)
	{
		PrintMsg(DEBUG_HARDWARE_MISC, ("Vender Id : 0x%04x, Product Id : 0x%04x, Revision Number : 0x%04x", 
			BoardInterfaces[i]->BoardVendorId, BoardInterfaces[i]->BoardProductId, BoardInterfaces[i]->BoardRevisionNum));
	}

	for(i=0;i<BoardNum;i++)
	{
		if(	BoardInterfaces[i]->BoardVendorId == context->BoardVendorId &&
			BoardInterfaces[i]->BoardProductId == context->BoardProductId &&
			BoardInterfaces[i]->BoardRevisionNum == context->BoardRevisionNum)
			break;
	}

	if(i==BoardNum)
	{
		context->BoardInterface = NULL;
		PrintMsg(DEBUG_HARDWARE_MISC, ("!!!! Device %04x/%04x/%04x is not supported by current board module",
			context->BoardVendorId, context->BoardProductId, context->BoardRevisionNum));
		DEBUG_FUNC_EXIT("InitBoardInterface0")
		return;
	}

	context->BoardInterface = BoardInterfaces[i];

	context->BoardInterface->pIBoardBasic->InitBoard((AVSTREAM_COOKIE)context,&context->BoardContext,&(context->BoardInfo));

	PrintMsg(DEBUG_HARDWARE_MISC, ("--------- Current Board Information"));
	PrintMsg(DEBUG_HARDWARE_MISC, ("Name        : %s", context->BoardInfo.Name));
	PrintMsg(DEBUG_HARDWARE_MISC, ("RevisionNum : %04x", context->BoardInfo.RevisionNum));
	PrintMsg(DEBUG_HARDWARE_MISC, ("BusMode     : %d", context->BoardInfo.BusMode));
	PrintMsg(DEBUG_HARDWARE_MISC, ("Capability  : %d", context->BoardInfo.Capability));


    if(context->BusType == WisUsb){
#if defined(_WINDOWS)
	    if(context->BoardInfo.BusMode == UM_USB_EZUSB)
		{
			context->ReadStreamHandle = context->Interface->Pipes[2].PipeHandle;
			context->ReadStreamMaxSize = context->Interface->Pipes[2].MaximumTransferSize;

			context->ReadInterruptHandle = context->Interface->Pipes[1].PipeHandle;

			context->ReadAudioStreamHandle = context->Interface->Pipes[3].PipeHandle;
			context->ReadAudioStreamMaxSize = context->Interface->Pipes[3].MaximumTransferSize;
		}
		else
		{
			context->ReadStreamHandle = context->Interface->Pipes[0].PipeHandle;
			context->ReadStreamMaxSize = context->Interface->Pipes[0].MaximumTransferSize;

			context->ReadInterruptHandle = context->Interface->Pipes[3].PipeHandle;
		}
#elif defined(_LINUX)
	    if(context->BoardInfo.BusMode == UM_USB_EZUSB)
		{
			context->ReadStreamHandle = usb_rcvbulkpipe(context->usbdev, 6);
			context->ReadInterruptHandle = usb_rcvintpipe(context->usbdev, 4);
			context->ReadAudioStreamHandle = usb_rcvbulkpipe(context->usbdev, 8);
		}
		else
		{
			context->ReadStreamHandle = usb_rcvbulkpipe(context->usbdev, 1);
			context->ReadInterruptHandle = usb_rcvintpipe(context->usbdev, 4);
		}
//SJ reduced max block size to keep small frames from getting stuck during no/low-motion video
//		context->ReadStreamMaxSize = 65536;			// ??? not sure
//		context->ReadAudioStreamMaxSize = 65536;	// ??? not sure
		context->ReadStreamMaxSize = 8192;	
//		context->ReadStreamMaxSize = 2048;	
		context->ReadAudioStreamMaxSize = 2048;

#if defined(__AVSYNC__)		
//SJ increased sample count to avoid AVSync request based on just one sample
//		context->AVSyncCorrectionTime = 1;          //initial setting, check the av sync every 1 second
		context->AVSyncCorrectionTime = 3;          //sec: number of 1-sec samples to average AV offset

		context->AVSyncCorrectionDelayTime = 100;   //ms: max AV offset threshold for FW Sync request
//SJ added to keep card audio clock in sync with system audio clock
		context->AudioCorrectionTime = 5;             //sec: how often to check audio/system time sync
		context->AudioCorrectionDelayTime = 20;       //ms: max drift between audio time and system time
		context->AudioCorrectionAVSyncLeadTime = 100; //ms: max AV offset over AVSyncCorrDelayTime 
#endif
#endif
	}
    
    if(context->BusType == WisPci){
		context->ReadStreamMaxSize = 8192;
        context->ReadAudioStreamMaxSize = 1024;
	}

	DEBUG_FUNC_EXIT("InitBoardInterface1")
}

void FormatSysFileName(SINT8 *FileName)
{
	char SysPath[256];
	GetConfigFilePath(SysPath);
	strcat(SysPath, FileName);
	strcpy(FileName, SysPath);
}

void ForceDropFrame(PDEVICE_EXTENSION_COMMON context)
{
	PrintMsg(DEBUG_HARDWARE_MISC, ("AVSYNC ForceDropFrame"));
	(context->hw_funcs.WriteInterrupt)(context, 0x0020, 0x0000);
}

void ForceDuplicateFrame(PDEVICE_EXTENSION_COMMON context)
{
	PrintMsg(DEBUG_HARDWARE_MISC, ("AVSYNC ForceDuplicateFrame"));

	if( Config_IsVideoDVDCompatibleStream(&context->config_manager)
		&& Config_IsVideoIVTCEnabled(&context->config_manager) )
		return;

	(context->hw_funcs.WriteInterrupt)(context, 0x0021, 0x0000);
}



SINT32 BoardCallBack_OnVSyncLost(void *context)
{
	PDEVICE_EXTENSION_COMMON pDeviceContext = (PDEVICE_EXTENSION_COMMON)context;
	SINT32 video_channel_count = pDeviceContext->TotalVideoChannel;

	PrintMsg(DEBUG_HARDWARE_MISC, ("On vertical-sync lost board callback!"));

	if ( pDeviceContext->ChipState == DS_RUNNING )
	{
		UninitializeDevice(pDeviceContext);
		pDeviceContext->TotalVideoChannel = video_channel_count;
		InitializeDevice(pDeviceContext);
	}

	return 1;
}

#define GET_FIX_CONFIG_INT(Struct,Field)		\
{												\
	SINT32 Value;								\
	if(GetNextConfigInt(fp, #Field, &Value)==0)	\
		Struct->Field = Value;					\
}

void DumpFixConfig(TVIDEOCFGFIX *fix_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- osdcfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DoOSD                   : %d",	fix_config->osdcfg.DoOSD));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDyc0                  : %d",	fix_config->osdcfg.OSDyc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDyc1                  : %d",	fix_config->osdcfg.OSDyc1));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDuc0                  : %d",	fix_config->osdcfg.OSDuc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDuc1                  : %d",	fix_config->osdcfg.OSDuc1));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDvc0                  : %d",	fix_config->osdcfg.OSDvc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDvc1                  : %d",	fix_config->osdcfg.OSDvc1));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- thcfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THACCoeffSet0           : %d",	fix_config->thcfg.THACCoeffSet0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THACCoeffStartpoint     : %d",	fix_config->thcfg.THACCoeffStartpoint));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THedge                  : %d",	fix_config->thcfg.THedge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THmotion                : %d",	fix_config->thcfg.THmotion));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THblending              : %d",	fix_config->thcfg.THblending));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THbigedge               : %d",	fix_config->thcfg.THbigedge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THsmalledge             : %d",	fix_config->thcfg.THsmalledge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THedgestatistics        : %d",	fix_config->thcfg.THedgestatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THmotionstatistics      : %d",	fix_config->thcfg.THmotionstatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THbigedgestatistics     : %d",	fix_config->thcfg.THbigedgestatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THsmalledgestatistics   : %d",	fix_config->thcfg.THsmalledgestatistics));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- deintercfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Motion   : %d",	fix_config->deintercfg.DeInter_Always_Motion));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Blending : %d",	fix_config->deintercfg.DeInter_Always_Blending));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Weave    : %d",	fix_config->deintercfg.DeInter_Always_Weave));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- elsecfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("HPIBufferCapacity       : %d",	fix_config->elsecfg.HPIBufferCapacity));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("clockrate               : %d",	fix_config->elsecfg.clockrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DRAM                    : %d",	fix_config->elsecfg.DRAM));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ivtc_holding_time       : %d",	fix_config->elsecfg.ivtc_holding_time));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("HasAudio                : %d",	fix_config->elsecfg.HasAudio));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("v_sync_bitplane         : %d",	fix_config->elsecfg.v_sync_bitplane));
}

#if 1 /* FIXUP: Dan - this is also in cmtools.cpp */
SINT32 GetConfigString(FILE *fp, const SINT8 *strItem, SINT8 *itemString)
{

	SINT8 strLine[128], *strTemp;
	UINT32 j;
	if (!fp) 
	  return 0;
	if (g_nvlen == 0) {
        	while (!feof(fp)) {
                	if (fgets(strLine, 128, fp) == NULL) break;
                	DeleteTabSpace(strLine);
                	strTemp = (SINT8*)strstr(strLine, "//");
                	j = strTemp - strLine + 1;
                	if (strTemp) strLine[j - 1] = 0;
                	if (strLine[0] == 0) continue;
                	if (strLine[0] == '\n') continue;
                	if (strLine[0] == ';') continue;
                	strTemp = (SINT8*)strtok(strLine, "=");
                	strcpy (g_nvpair[g_nvlen].nv_name, strTemp);
                	strTemp = (SINT8*)strtok(NULL, "=");
                	if (strTemp[strlen(strTemp) - 1] == '\n')
                        	strTemp[strlen(strTemp) - 1] = 0;
                	if (strTemp[strlen(strTemp) - 1] == ';')
                        	strTemp[strlen(strTemp) - 1] = 0;
	
                	strcpy (g_nvpair[g_nvlen].nv_val, strTemp);
                	g_nvlen++;
		}
        }
	for (j=0; j < g_nvlen; j++)
	{
		if (strTemp = strstr(g_nvpair[j].nv_name, strItem))
		{
			strcpy(itemString, g_nvpair[j].nv_val);
			return 1;
		}
	}
	return 0;
}


SINT32 GetConfigArray(FILE *fp, const SINT8 *strItem, SINT32 *itemValue, SINT32 s32ElementSize, SINT32 s32NumItems)
{
	SINT8 itemString[128];
	SINT8 *arrs8ItemValue;
	SINT16 *arrs16ItemValue;
	SINT32 *arrs32ItemValue;
	SINT32 s32ItemIndex;
	SINT32 s32StringIndex=0;
	
 	arrs32ItemValue = (SINT32 *)itemValue;
	arrs16ItemValue = (SINT16 *)itemValue;
	arrs8ItemValue = (SINT8 *)itemValue;
	
	if ( (s32ElementSize != 4)
		 && (s32ElementSize != 2)
		 && (s32ElementSize != 1) )
	{
		printk("Invalid GetConfigArray size argument");
		return 0;
	}
	
	if (GetConfigString(fp, strItem, itemString))
	{
		for (s32ItemIndex=0; s32ItemIndex < s32NumItems-1; s32ItemIndex++)
		{
			switch (s32ElementSize)
			{
				case 4:
					sscanf(&itemString[s32StringIndex], "%d,", &arrs32ItemValue[s32ItemIndex]);
					break;

				case 2:
					sscanf(itemString, "%d,", &arrs16ItemValue[s32ItemIndex]);
					break;

				case 1:
					sscanf(itemString, "%d,", &arrs16ItemValue[s32ItemIndex]);
					break;
			}
			
			/* skip over this item to the next one after the ',' */
			while (itemString[s32StringIndex++] != ',') { ; }
		}
		
		switch (s32ElementSize)
		{
			case 4:
				sscanf(&itemString[s32StringIndex], "%d", &arrs32ItemValue[s32ItemIndex]);
				break;

			case 2:
				sscanf(itemString, "%d", &arrs16ItemValue[s32ItemIndex]);
				break;

			case 1:
				sscanf(itemString, "%d", &arrs16ItemValue[s32ItemIndex]);
				break;
		}
		
		return s32ItemIndex;
	}
	
	return 0;
}

SINT32 GetConfigValue(FILE *fp, const SINT8 *strItem, SINT32 *itemValue)
{
	SINT8 itemString[128];
	if (GetConfigString(fp, strItem, itemString))
	{
		sscanf(itemString, "%d", itemValue);
		return 1;
	}
	return 0;
}
#endif

int GetFixConfig(PDEVICE_EXTENSION_COMMON pdxc, TVIDEOCFGFIX *fix_config)
{
    int j2,j1;

	// audio setting
	{
		AUDIO_CONFIG* audio_config;

		audio_config = Config_GetCurrentAudioConfig(&pdxc->config_manager);
		DumpAudioConfig(audio_config);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("GetFixConfig : %d %d", 
			__LINE__,j2-j1));

		if(pdxc->BoardInterface->pIBoardAudio != NULL)
			pdxc->BoardInterface->pIBoardAudio->GetAudioChipConfig(pdxc->BoardContext,(AVSTREAM_COOKIE)pdxc,&(fix_config->audiocfg), audio_config);
		else
			PIFillMem(&(fix_config->audiocfg), sizeof(TCFGAUDIO), 0);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("GetFixConfig : %d %d", 
			__LINE__,j2-j1));
		fix_config->audiocfg.reserved = audio_config->SampleRate;
	}

	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("GetFixConfig : %d %d", 
			__LINE__,j2-j1));

	if(pdxc->BoardInfo.BusMode == UM_USB_EZUSB)
		fix_config->elsecfg.HPIBufferCapacity = 7;
	else // UM_ONCHIP
		fix_config->elsecfg.HPIBufferCapacity = 0;

	fix_config->elsecfg.HasAudio = pdxc->DoAudio;
// this gets modified later based on video config so dont dump it just yet
//	DumpFixConfig(fix_config);
	j2=jiffies;
	PrintMsg(DEBUG_HARDWARE_MISC, ("GetFixConfig : %d %d", 
			__LINE__,j2-j1));

	return 0;
}

void BitrateControlPatch(TCFGVIDEOEX* video_config, TVIDEOCFGFIX* fix_config)
{
	int tmp_fps;

	if( GET_Q_MIN(video_config->ctlcfg.reserved) == 0)
		SET_Q_MIN(video_config->ctlcfg.reserved, 2);

	if( GET_Q_MAX(video_config->ctlcfg.reserved) == 0)
		SET_Q_MAX(video_config->ctlcfg.reserved, 31);

	if( GET_BRC_WINDOW(video_config->ctlcfg.reserved) == 0)
		SET_BRC_WINDOW(video_config->ctlcfg.reserved, 1*video_config->fpscfg.frame_rate/1000);

	if(video_config->strcfg.compress_mode == 4)			// MPEG4
	{
		//SJ adjust deinterlace mode in fix config
		fix_config->deintercfg.DeInter_Always_Motion = 0;
		fix_config->deintercfg.DeInter_Always_Blending = 1;
		fix_config->deintercfg.DeInter_Always_Weave = 0;

		if(video_config->ctlcfg.peak_bitrate == 0)
		{
			video_config->ctlcfg.peak_bitrate = 
				video_config->ctlcfg.target_bitrate * 2;
		}
		if(video_config->ctlcfg.vbv_buffer == 0)
		{
			video_config->ctlcfg.vbv_buffer =
				video_config->ctlcfg.target_bitrate * 2;
		}
	}

	else if(video_config->strcfg.compress_mode == 1)	// MPEG2
	{
		//SJ adjust deinterlace mode in fix config
		//fix_config->deintercfg.DeInter_Always_Motion = 1;
		//fix_config->deintercfg.DeInter_Always_Blending = 0;
		//fix_config->deintercfg.DeInter_Always_Weave = 0;

		if(video_config->strcfg.DVD_compliant == 1) // DVD
        {
			if(video_config->ctlcfg.target_bitrate > 7000000)
				video_config->ctlcfg.target_bitrate = 7000000;

            video_config->ctlcfg.peak_bitrate = 8000000;
			video_config->ctlcfg.vbv_buffer =  900000;

			SET_BRC_DUP_ENB(video_config->ctlcfg.reserved, 1);
			SET_MB_BRC_ENB(video_config->ctlcfg.reserved, 1);

			video_config->strcfg.aspect_ratio = 2;	// 4:3
        }
		else
        {
			if(video_config->ctlcfg.peak_bitrate == 0)
			{
				video_config->ctlcfg.peak_bitrate = 
					video_config->ctlcfg.target_bitrate * 2;
			}
			if(video_config->ctlcfg.vbv_buffer == 0)
			{
				video_config->ctlcfg.vbv_buffer =
					video_config->ctlcfg.target_bitrate * 2;
			}
        }
	}

	else if(video_config->strcfg.compress_mode == 0) 	// MPEG1
	{
		if(video_config->strcfg.DVD_compliant == 1)	 // VCD complaint
		{
			if(video_config->ctlcfg.target_bitrate > 900000)
				video_config->ctlcfg.target_bitrate = 900000;

			video_config->ctlcfg.peak_bitrate = 1000000;
  			video_config->ctlcfg.vbv_buffer = 300000;

			SET_BRC_DUP_ENB(video_config->ctlcfg.reserved, 1);
			SET_MB_BRC_ENB(video_config->ctlcfg.reserved, 1);

			video_config->strcfg.aspect_ratio = 2;	// 4:3
		}
		else
        {
			if(video_config->ctlcfg.peak_bitrate == 0)
			{
				video_config->ctlcfg.peak_bitrate = 
					video_config->ctlcfg.target_bitrate * 2;
			}
			if(video_config->ctlcfg.vbv_buffer == 0)
			{
				video_config->ctlcfg.vbv_buffer =
					video_config->ctlcfg.target_bitrate * 2;
			}
        }
	}

	/* start of Config_CompleteVideoConfigurations() from configuration.c */
	/* because we don't use WINDOWS SDK's configuration.c in USB LINUX SDK
	 * but we still want to use some of its code, so we put the code here
	 */
/*SJ - why? */
	if(((UINT32)(video_config->rescfg.height*2) <= (UINT32)video_config->syscfg.sensor_v) && (video_config->syscfg.format==1) && (video_config->strcfg.deinterlace_mode==1))
		video_config->strcfg.deinterlace_mode = 0;

	if ( video_config->fpscfg.frame_rate != 0 ) {
		tmp_fps = (video_config->fpscfg.frame_rate/1000)*1000;
		video_config->fpscfg.drop_frame = video_config->syscfg.framerate / tmp_fps - 1;
		if (((UINT32)(video_config->rescfg.height*2) <= (UINT32)video_config->syscfg.sensor_v) && (video_config->syscfg.format==1) && (video_config->strcfg.deinterlace_mode==0))
			video_config->fpscfg.drop_frame = video_config->fpscfg.drop_frame * 2 + 1;
	}
	else 
		video_config->fpscfg.drop_frame = 0; // this should not happen.
/**/
	/* end of Config_CompleteVideoConfigurations() from configuration.c */

}

int GenInitialPacket(PDEVICE_EXTENSION_COMMON pdxc, 
		     TCFGVIDEOEX *video_config,
		     TVIDEOCFGFIX *fix_config,
		     UINT8 *pPack, SINT32 *PackLen,
		     UINT8 *pFontBitmaps, SINT32 lFontBitmapSize, int q_rst)
{
  int r=0;
    int i;

#if 1

	SINT8 error_info[256];
	UINT8 *iniBuffer; 
	int buflen;
	char FileName[256] = "go7007sb.bin";

	buflen = 256*1024;
	iniBuffer = PIAllocVirtualMem(buflen);

	FormatSysFileName(FileName);
	if(ReadFileToBuffer(FileName, iniBuffer, &buflen)>0)
	{
		PIFreeVirtualMem(iniBuffer);
		return 1;
	}

	BitrateControlPatch(video_config, fix_config);
	DumpVideoConfig(video_config);
	DumpFixConfig(fix_config);

	r = gen_init_packages_7007SB(iniBuffer,
				     buflen,
				     video_config,
				     fix_config,
				     error_info,
				     pPack,
				     PackLen,
				     pFontBitmaps,
				     lFontBitmapSize,
				     pdxc->pBlankPFrame, pdxc->pSeqHeader, q_rst);

	PIFreeVirtualMem(iniBuffer);

	PrintMsg(DEBUG_PACKGEN, ("Pac Gen: return: %d, len: %d, mesg: %s",r,*PackLen,error_info));
//	WriteBufferToFile("\\??\\c:\\init_pack_sdk.bin",pPack,*PackLen);

//#define DEBUG_INIT_PACK
#ifdef DEBUG_INIT_PACK
	for( i=0;i<*PackLen;i++)
    {
		printk("%02x ", pPack[i]);
		if ((!(i%32)) && (i!=0))
			printk("\n");
    }
#endif

#else

#if defined(_WINDOWS)
	char InitPackFileName[256] = "\\??\\c:\\init_pack_sdk.bin";
#elif defined(_LINUX)
	char InitPackFileName[256] = "/root/init_pack_sdk.bin";
#endif

	if(ReadFileToBuffer(InitPackFileName, pPack, PackLen)>0)
	{
		PrintMsg(DEBUG_PACKGEN, ("InitializeDevice : Read init pack file fail"));
		return 1;
	}

#endif

	return r;
}

// StartBitIndex should >= StopBitIndex
UINT32 GetBits(UINT32 Source, int StartBitIndex, int StopBitIndex)
{
	UINT32 Mask;

	if(StartBitIndex < StopBitIndex)
		return 0;

	Mask = (1<<(StartBitIndex-StopBitIndex+1))-1;

	return  (Mask & (Source>>StopBitIndex));
}
void SetBits(UINT32 *pSource, int StartBitIndex, int StopBitIndex, UINT32 Value){
    UINT32 Mask;
    
    if(StartBitIndex < StopBitIndex)
	return;
    
    Mask = (1<<(StartBitIndex-StopBitIndex+1))-1;
    
    *pSource = ((*pSource) & (~(Mask<<StopBitIndex))) |
	((Value&Mask)<<StopBitIndex);
}	


/*
	send the audio/video offset to firmware. Firmware will insert/drop video
	according this offset 

	Offset: offset of A/V in unit of audio samples
			>0, need drop video
			<0, need insert video
*/
#if defined(__AVSYNC__)
void NotifyAVOffset(PDEVICE_EXTENSION_COMMON pdxc, SINT32 Offset)
{
	int r;
	if(Offset == 0)
		return;

	//LOCK_SPINLOCK(pdxc, NotifyAVOffsetLock);	

	PrintMsg(DEBUG_HARDWARE_MISC, ("NotifyAVOffset : %d", Offset));

	down(&pdxc->i2c_lock);
	(pdxc->hw_funcs.WriteInterrupt)( pdxc, 0x0020, Offset&0xffff );
	(pdxc->hw_funcs.WriteInterrupt)( pdxc, 0x0021, (Offset>>16)&0xffff );
	up(&pdxc->i2c_lock);
	//UNLOCK_SPINLOCK(pdxc, NotifyAVOffsetLock);	
}

void
SetAVSyncPara(PDEVICE_EXTENSION_COMMON pdxc, int audio_delay_threshould, int av_sync_check_inteval)
{
    int i;
	pdxc->AVSyncCorrectionDelayTime = audio_delay_threshould;
	pdxc->AVSyncCorrectionTime = av_sync_check_inteval;
	//pdxc->TimerCount=0;
	PrintMsg(DEBUG_AVSYNC, ("A/V Sync threshould:%d(ms),Check time:%d(s),current time %d:%d:%d",
		audio_delay_threshould,av_sync_check_inteval,pdxc->TimerCount/3600,(pdxc->TimerCount%3600)/60,pdxc->TimerCount%60));
	for(i=0; i< pdxc->AVSyncCorrectionTime; i++)
			pdxc->AVOffset[i] = 0;

}

#endif

