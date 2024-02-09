/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        common.h
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


#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef _WINDOWS
	#define _WIN_PCI
#else
        /* Linux */
// Turn off avsync at driver level - output it simply based on timestamps
// Need to figure out alternative output of stats
// #define __AVSYNC__ 

#endif

#include "Struct.h"
#include "StreamBuffer.h"
#include "AudioStreamBuffer.h"
#include "Board.h"
#include "osd.h"
#include "PacGen_7007SB.h"

#ifdef _WINDOWS
	#include "configuration.h"
#else 
	#include "config_simple.h"
#endif

#ifdef _WIN_PCI
	#include "pci_7008.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"

 
#define COMP_TYPE_DIVX_MPEG4			0
#define COMP_TYPE_MICROSOFT_MPEG4		1
#define COMP_TYPE_MPEG2				2
#define COMP_TYPE_MPEG1				3
#define COMP_TYPE_H263					4
#define COMP_TYPE_MJPG					5
#define COMP_TYPE_FCC_MPEG4			6
 

typedef enum 
{
	DS_IDLE,			// after StartDevice
	DS_STARTING,
	DS_STOPPING,
	DS_RUNNING,			// after initialize
}DEVICE_STATE;

typedef enum 
{
	SpongeBob,			// after StartDevice
	Gomer,
}CHIP_TYPE;

typedef enum 
{
	WisUsb,			    // after StartDevice
	WisPci,
}BUS_TYPE;

#ifdef _WIN_PCI
 
typedef enum 
{
	DmaMode,			// after StartDevice
	PioMode,
}WIS_SBPCI_MODE;

//*************************************************************************
// DMA Stuff.
//*************************************************************************
#define MAX_DMA_DESCRIPTORS   (PAGE_SIZE/(sizeof(_WIS_SBPCI_DMA_DESCRIPTOR)))
#define AUDIO_DESCRIPTORS     m_dma_wc0_descriptors
#define VIDEO_DESCRIPTORS     m_dma_wc1_descriptors
#define RAW_DESCRIPTORS       m_dma_rc0_descriptors
#define MISC_DESCRIPTORS      m_dma_rc1_descriptors
#define AUDIO_SG_LIST         m_dma_wc0_scattergather_list
#define VIDEO_SG_LIST         m_dma_wc1_scattergather_list
#define RAW_SG_LIST           m_dma_rc0_scattergather_list
#define MISC_SG_LIST          m_dma_rc1_scattergather_list
#define AUDIO_SG_LIST_FREE    m_dma_wc0_scattergather_list_free
#define VIDEO_SG_LIST_FREE    m_dma_wc1_scattergather_list_free
#define RAW_SG_LIST_FREE      m_dma_rc0_scattergather_list_free
#define MISC_SG_LIST_FREE     m_dma_rc1_scattergather_list_free
#define AUDIO_ADAPTER_DEVOBJ  m_wc0_adapter_device_object
#define VIDEO_ADAPTER_DEVOBJ  m_wc1_adapter_device_object
#define RAW_ADAPTER_DEVOBJ    m_rc0_adapter_device_object
#define MISC_ADAPTER_DEVOBJ   m_rc1_adapter_device_object
#define AUDIO_XFER_BYTES      m_wc0_transfer_bytes
#define VIDEO_XFER_BYTES      m_wc1_transfer_bytes
#define RAW_XFER_BYTES        m_rc0_transfer_bytes
#define MISC_XFER_BYTES       m_rc1_transfer_bytes
#define AUDIO_MDL             m_wc0_dma_mdl
#define VIDEO_MDL             m_wc1_dma_mdl
#define RAW_MDL               m_rc0_dma_mdl
#define MISC_MDL              m_rc1_dma_mdl
#define AUDIO_NOTIFIER        m_wc0_dma_notifier->dma_notifier_mapping
#define VIDEO_NOTIFIER        m_wc1_dma_notifier->dma_notifier_mapping
#define RAW_NOTIFIER          m_rc0_dma_notifier->dma_notifier_mapping
#define MISC_NOTIFIER         m_rc1_dma_notifier->dma_notifier_mapping
#define DMA_READ              FALSE
#define DMA_WRITE             TRUE
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_VIDEO_DMA_PAGE_BUFFER_WRITE_COMPLETE \
        WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_PAGE_BUFFER_WRITE_COMPLETE  
#define WIS_SBPCI_GRPB_INTERRUPT_STATUS_AUDIO_DMA_PAGE_BUFFER_WRITE_COMPLETE \
        WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_PAGE_BUFFER_WRITE_COMPLETE  
        
// Waiting for status on these bits. Hack it for now.
#define VIDEO_DMA_PAGE_BUFFER_WRITE_COMPLETE \
        WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC1_DMA_PAGE_SKIP  
#define AUDIO_DMA_PAGE_BUFFER_WRITE_COMPLETE \
        WIS_SBPCI_GRPB_INTERRUPT_STATUS_WC0_DMA_FRAME_SKIP
        
#define VIDEO_PBI_COMMAND     wis_sbpci_wc1_pbi_command_register
#define AUDIO_PBI_COMMAND     wis_sbpci_wc0_pbi_command_register
#define VIDEO_PBI_DESC_HEAD   wis_sbpci_wc1_head_buffer_descriptor_address_register
#define AUDIO_PBI_DESC_HEAD   wis_sbpci_wc0_head_buffer_descriptor_address_register
#define CHANNEL_RC0    0
#define CHANNEL_WC0    1
#define CHANNEL_WC1    2
#define CHANNEL_RC1    3
#define CHANNEL_VIDEO  CHANNEL_WC1
#define CHANNEL_AUDIO  CHANNEL_WC0

typedef struct _WIS_DMA_NOTIFY
{
    PVOID dma_notifier; 
    ULONG dma_notifier_mapping; 
}WIS_DMA_NOTIFY, *PWIS_DMA_NOTIFY;

#define AUDIO_RAW_BUFFER      m_raw_wc0_capture_buffer
#define VIDEO_RAW_BUFFER      m_raw_wc1_capture_buffer

typedef struct _DEBUG_CAPTURE_BUFFER
{
	PUCHAR	       _buffer;
	UINT32	       _buffer_size;
	UINT32         _read_byte_index;
	UINT32         _write_byte_index;
    WCHAR          _capture_filename[80];
	UNICODE_STRING _unicode_file_name;
    HANDLE         _capture_filehandle;
    BOOLEAN        _is_capturing;
} DEBUG_CAPTURE_BUFFER, *PDEBUG_CAPTURE_BUFFER;

#endif

//*************************************************************************


#if defined(_WINDOWS)

#pragma warning( disable: 4200 )

	#include <wdm.h>
	#include "usbdi.h"
	#include "usbdlib.h"

#elif defined(_LINUX)

	#include <linux/usb.h>
#endif

//Keep XBar's statuss
typedef struct _XBAR_STATUS
{
	SINT32 Caps;
	UINT8 SupportMute;		
}XBAR_STATUS;
#if defined(_WINDOWS)
typedef void (* NOTIFICATION_CALLBACK)(void* context, USHORT type, USHORT data);
#endif
typedef struct _DEVICE_EXTENSION_COMMON 
{
	// open handle count
	SINT32			OpenHandleCount;	
	SINT32			DeviceId;	
  int videoretries;
  int audioretries;
  int lastvideoretries;
  int lastaudioretries;
	STREAM_BUFFER	StreamBuffer;
	CAudioStreamBuffer* AudioStreamBuffer;
	int				BufferAllocated;

	//For each board, we keep its context into the dxc
	BOARD_CONTEXT 	BoardContext;

	STREAMBUFFER_CALLBACKFUNC VideoSBCallBack;
	AUDIOSTREAMBUFFERCALLBACK AudioSBCallBack;
	void*			SBCallBackContext;
	int             TotalVideoChannel;
#if defined(_WINDOWS)
	NOTIFICATION_CALLBACK	NotificationCallBack;
	void*			NotificationCallBackContext;
#endif
    int				IFrameNum;	// for drop all data before first I frame
	int				PFrameNum;	// for dropping B frame before first P frame or first 2 I frames

	int				TotalDelay;		// current encoded - next frame to be fetched from stream buffer
	int				ChipDelay;		// current encoded - current added to stream buffer
	int             LastChipFrameNum;	// reported through interrupt
	int             RollOverTimes;
	UINT32          ChipFrameNum;  // TimeStamp after adjusting
	UINT32          LastChipTick;	// reported through interrupt

	int				BootupSuccess;
	DEVICE_STATE    ChipState;

	int				DeviceConnected;

	int				BlockSize;
	int				AudioBlockSize;

    BOARD_INFO		BoardInfo;

	int             DoAudio;
	int             DoVideo;

	int				CheckInterruptType;
	int				CheckInterruptData;

	UINT8           *pBlankPFrame;		// hold a blank P frame for interpolation
										// byte[0] = low 8-bit of dup_mem size in byte
										// byte[1] = high 8-bit of dup_mem size in byte
	UINT8           *pVBIBuffer;
        UINT8           *pSeqHeader;       // save seq header during initial package gen

	BOARD_INTERFACE *BoardInterface;
	UINT32			BoardVendorId;
	UINT32			BoardProductId;
	UINT32			BoardRevisionNum;

	XBAR_STATUS		XBarStatus;

	CONFIG_MANAGER	config_manager;
	TVIDEOCFGFIX	fix_config;

	int				MDInterruptFlag;

#if defined(_WINDOWS) 
	KMUTEX			I2CLock; 
	KMUTEX			CheckIntLock;
#elif defined(_LINUX)
#endif

	UINT8		    *pReadStreamBlock;
	int             ReadStreamState;

	UINT8		    *pReadAudioStreamBlock;
	int             ReadAudioStreamState;

	UINT8		    *pReadInterruptBlock;
	int             ReadInterruptState;

#if defined(__AVSYNC__)
	int             AVSynchState;
	int             TimerCount;
	SINT32		        AVOffset[128];
	int		        InitAVOffset;
	int		        AVSyncCorrectionTime;
	int		        AVSyncCorrectionDelayTime;
	int		        AudioCorrectionTime;
	int		        AudioCorrectionDelayTime;
	int		        AudioCorrectionAVSyncLeadTime;
  UINT32    LastAVSyncTime;	
  UINT64    lastTotalVideoBytes;
  UINT32    lastTotalAudioTime;
#endif

	int AudioReadJitterCnt[4];
	int VideoReadJitterCnt[4];
	int lastAudioReadJitterCnt[4];
	int lastVideoReadJitterCnt[4];

  UINT32    StartTime;	//SJ used in units of jiffies to avoid overflow

  SINT32    AudioAdjBlkCnt;
  UINT32    AudioAddBlkCnt;
  UINT32    AudioDropBlkCnt;
  UINT32    VideoAddCnt;
  UINT32    VideoDropCnt;

	//KEVENT          WatchStopEvent;

	CHIP_TYPE       ChipType;  
	BUS_TYPE        BusType;  

	struct _WISDRIVER_HW_FUNCTION_TABLE
	{
		int       (*DoVendorRequest       )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT8 Request, UINT16 Index, UINT16 Value, PVOID TransferBuffer,UINT32 TransferBufferLength, int Dir);
		int       (*DownloadBuffer        )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT8 *pBuffer, SINT32 Size);
		int       (*WriteInterrupt        )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT16 Addr, UINT16 Data);
		int       (*ReadInterrupt         )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT16 *pIntRetVal, UINT16 *pIntRetData);
		int       (*ReadStream            )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT8 *pBuf, SINT32 Size);
		NTSTATUS  (*ResetPipe             )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT32 PipeNum);
		NTSTATUS  (*AbortPipe             )(struct _DEVICE_EXTENSION_COMMON *pdxc, UINT32 PipeNum);
		NTSTATUS  (*InitializeBus         )(struct _DEVICE_EXTENSION_COMMON *pdxc);
		NTSTATUS  (*UninitializeBus       )(struct _DEVICE_EXTENSION_COMMON *pdxc);
		int       (*StartReadVideoStream  )(struct _DEVICE_EXTENSION_COMMON *pdxc);
		int       (*StartReadAudioStream  )(struct _DEVICE_EXTENSION_COMMON *pdxc);
		int       (*StartReadInterrupt    )(struct _DEVICE_EXTENSION_COMMON *pdxc);
		void      (*StopReadAllPipes      )(struct _DEVICE_EXTENSION_COMMON *pdxc);
 		void      (*ResetBusDevice        )(struct _DEVICE_EXTENSION_COMMON *pdxc);
        NTSTATUS  (*SoftwareReset         )(struct _DEVICE_EXTENSION_COMMON *pdxc);
	} hw_funcs;
	
#ifdef WIS_PCI

	PKINTERRUPT		               m_InterruptObject;
    PADAPTER_OBJECT                m_DmaAdapterObject;
    ULONG                          m_NumberOfMapRegisters;
	CM_PARTIAL_RESOURCE_DESCRIPTOR Memory_BARS[2];
	CM_PARTIAL_RESOURCE_DESCRIPTOR Interrupt;
	PWIS_SBPCI_DEVICE              m_WisSBPciDevice;
	PWIS_SBHPI_DEVICE              m_WisSBHpiDevice;
	ULONG			               OperatingMode;
    PWIS_SBPCI_DMA_DESCRIPTOR      m_dma_wc0_descriptors;
    PWIS_SBPCI_DMA_DESCRIPTOR      m_dma_wc1_descriptors;
    PWIS_SBPCI_DMA_DESCRIPTOR      m_dma_rc0_descriptors;
    PWIS_SBPCI_DMA_DESCRIPTOR      m_dma_rc1_descriptors;
    PSCATTER_GATHER_LIST           m_dma_wc0_scattergather_list;
    PSCATTER_GATHER_LIST           m_dma_wc1_scattergather_list;
    PSCATTER_GATHER_LIST           m_dma_rc0_scattergather_list;
    PSCATTER_GATHER_LIST           m_dma_rc1_scattergather_list;
    BOOLEAN                        m_dma_wc0_scattergather_list_free;
    BOOLEAN                        m_dma_wc1_scattergather_list_free;
    BOOLEAN                        m_dma_rc0_scattergather_list_free;
    BOOLEAN                        m_dma_rc1_scattergather_list_free;
    PDEVICE_OBJECT                 m_wc0_adapter_device_object;
    PDEVICE_OBJECT                 m_wc1_adapter_device_object;
    PDEVICE_OBJECT                 m_rc0_adapter_device_object;
    PDEVICE_OBJECT                 m_rc1_adapter_device_object;
    ULONG                          m_wc0_transfer_bytes;
    ULONG                          m_wc1_transfer_bytes;
    ULONG                          m_rc0_transfer_bytes;
    ULONG                          m_rc1_transfer_bytes;
    PMDL                           m_wc0_dma_mdl; 
    PMDL                           m_wc1_dma_mdl; 
    PMDL                           m_rc0_dma_mdl; 
    PMDL                           m_rc1_dma_mdl; 
    PWIS_DMA_NOTIFY                m_wc0_dma_notifier; 
    PWIS_DMA_NOTIFY                m_wc1_dma_notifier; 
    PWIS_DMA_NOTIFY                m_rc0_dma_notifier; 
    PWIS_DMA_NOTIFY                m_rc1_dma_notifier;
    KDPC                           m_dma_dpc;
    ULONG                          m_start_audio_dma;
    PDEBUG_CAPTURE_BUFFER          m_raw_wc0_capture_buffer;
    PDEBUG_CAPTURE_BUFFER          m_raw_wc1_capture_buffer;
#else
	UINT32		    ReadStreamUrbSize;	
	UINT32		    ReadAudioStreamUrbSize;
	UINT32		    ReadInterruptUrbSize;

#endif	/* _WIN_PCI */

	UINT32			ReadStreamMaxSize;
	UINT32			ReadAudioStreamMaxSize;

#if defined(_WINDOWS)

// only for USB
	// Device object we call when submitting Urbs
	PDEVICE_OBJECT TopOfStackDeviceObject;

	// we support up to one interface
	PUSBD_INTERFACE_INFORMATION Interface;

	USBD_PIPE_HANDLE	ReadStreamHandle;
	PURB            	ReadStreamUrb;

	USBD_PIPE_HANDLE	ReadInterruptHandle;
	PURB            	ReadInterruptUrb;

	USBD_PIPE_HANDLE	ReadAudioStreamHandle;
	PURB            	ReadAudioStreamUrb;
// 

	PKEVENT			NotifyEvent;			
	HANDLE			NotifyHandle;
	
	KEVENT			StreamReady;	// stream ready for read in stream buffer
	KEVENT			AudioStreamReady;	// stream ready for read in stream buffer

	KEVENT			CheckInterruptEvent;

	HANDLE			StreamThreadHandle;
	HANDLE			AudioStreamThreadHandle;
	HANDLE			InterruptThreadHandle;

	KEVENT			ReadStreamStopEvent;
	KEVENT			ReadAudioStreamStopEvent;
	KEVENT			ReadInterruptStopEvent;
	KEVENT			AVSynchStopEvent;
	KEVENT			MDInterruptEvent;

#ifdef _WATCHDOG_THREAD
	HANDLE			WatchDogThreadHandle;
	KEVENT			WatchDogStopEvent;
	KEVENT			WatchDogEvent;
	int             WatchDogEventIndex;		// 0: need reset chip
	int             WatchDogThreadState;
#endif

	KEVENT			NotifyInterruptEvent;
    
    PDEVICE_OBJECT	PhysicalDeviceObject;
	
#elif defined(_LINUX)

	struct usb_device	*usbdev;
        int                     unit; 
	wait_queue_head_t	wq_control_pipe;
	wait_queue_head_t	wq_download_pipe;
	wait_queue_head_t	wq_stream_pipe;
	wait_queue_head_t	wq_audio_stream_pipe;
	wait_queue_head_t	wq_interrupt_pipe;
	wait_queue_head_t	wq_interrupt_event;
	wait_queue_head_t	wq_audio_wait;

#if defined(__AVSYNC__)
	wait_queue_head_t	wq_avsynch_wait;
#endif

	int			ReadStreamHandle;
	int			ReadInterruptHandle;
	int			ReadAudioStreamHandle;
    
        struct semaphore        i2c_lock;
        struct semaphore        check_int_lock;
        struct semaphore        cbus_write_lock;
    
	// stream ready for read in stream buffer
	wait_queue_head_t   StreamReady;
    
        // stream ready for read in audio stream buffer
        wait_queue_head_t   AudioStreamReady;	

	wait_queue_head_t   CheckInterruptEvent;

	wait_queue_head_t	ReadStreamStopEvent;
	wait_queue_head_t	ReadAudioStreamStopEvent;
	wait_queue_head_t	ReadInterruptStopEvent;
	wait_queue_head_t	AVSynchStopEvent;
	unsigned short 		IntRetVal;
	unsigned short 		IntRetData;
	struct urb			*downloadbuffer_urb;
	struct urb			*video_urb;
	struct urb			*audio_urb;
	struct timer_list	audio_timer;
	int					testFlag;
#endif

//OSD
	HOSD hosd;
// For quick reset chip
	UINT8 *pQuickFirmware;		
	UINT8 *pQuickInitPack;
	int QuickInitPackLen;

} DEVICE_EXTENSION_COMMON, *PDEVICE_EXTENSION_COMMON;

typedef struct _WISDRIVER_HW_FUNCTION_TABLE WISDRIVER_HW_FUNCTION_TABLE;
typedef WISDRIVER_HW_FUNCTION_TABLE  *PWISDRIVER_HW_FUNCTION_TABLE;

#ifdef _LINUX
#define CHECK_INT_LOCK(context)\
        down (&(context)->check_int_lock)
#define CHECK_INT_UNLOCK(context)\
        up (&(context)->check_int_lock)
#define CBUS_WRITE_LOCK(context)\
        down (&(context)->cbus_write_lock)
#define CBUS_WRITE_UNLOCK(context)\
        up (&(context)->cbus_write_lock)
#else
/* WIN32 */
#define	CHECK_INT_LOCK(context)		KeWaitForMutexObject(&context->CheckIntLock,Executive,KernelMode,FALSE,0)
#define CHECK_INT_UNLOCK(context)	KeReleaseMutex(&context->CheckIntLock,FALSE)
#endif

#ifdef __cplusplus
}
#endif

#endif

/******************************** end of common.h ****************************/
