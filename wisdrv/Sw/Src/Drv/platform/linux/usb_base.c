//usb_base.c
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/delay.h>
#include <linux/usb.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>

#include <linux/signal.h>

#include "platform.h"
#include "usb_base.h"
#include "function.h"

#define MAXUSBPOLLING 100
#define MAXBLOCKSIZE 65536

#define ACTIVE		1
#define INACTIVE	0

/* basic usb pipe function */
//#define STREAM_PIPE 		1
//#define CONTROL_PIPE		2
//#define DOWNLOAD_PIPE 		3
//#define INTERRUPT_PIPE 		4

#define USB_TIMEOUT 	5   // 5 seconds for regular request
#define USB_MD_TIMEOUT 	15   // 1 second for MD request 

#define	HPI_STATUS_ADDR	0xFFF4
#define	INT_PARAM_ADDR	0xFFF6
#define	INT_INDEX_ADDR	0xFFF8

//NTSTATUS CallUSBD(PDEVICE_EXTENSION_COMMON pdxc, PURB Urb, PIO_COMPLETION_ROUTINE CompletionProc, int TimeoutMS, int CallerId);
#if defined(__AVSYNC__)
int AVSynchThreadProc(void *arg);
void WatchStatistic(PDEVICE_EXTENSION_COMMON pdxc);
SINT32 MediumValue(SINT32 Data[], int Size);
void Sort(SINT32 Data[], SINT32 Size);
#endif

NTSTATUS
ReadandSelectDescriptors(
    PDEVICE_EXTENSION_COMMON pdxc
    )
/*++
 
Routine Description:

    This routine configures the USB device.
    In this routines we get the device descriptor, 
    the configuration descriptor and select the
    configuration descriptor.

Arguments:

    DeviceObject - pointer to a device object

Return Value:

    NTSTATUS - NT status value.

--*/
{
    return 0;
}


NTSTATUS
DeconfigureDevice(
    PDEVICE_EXTENSION_COMMON pdxc
    )
/*++
 
Routine Description:

    This routine is invoked when the device is removed or stopped.
    This routine de-configures the usb device.

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
  	return 0;
}

NTSTATUS USB_SB_InitializeBus(PDEVICE_EXTENSION_COMMON pdxc)
{
  USB_SB_ResetBusDevice(pdxc);
	return 0;
}

NTSTATUS USB_SB_UninitializeBus(PDEVICE_EXTENSION_COMMON pdxc)
{
	return 0;
}

NTSTATUS USB_SB_AbortPipe(PDEVICE_EXTENSION_COMMON pdxc,UINT32 PipeNum)
{
	return 0;
}

/*++

Routine Description:

   Reset a given USB pipe.
   NOTE: this will reset the data toggle on the host as well

Arguments:

Return Value:

--*/

NTSTATUS USB_SB_ResetPipe(PDEVICE_EXTENSION_COMMON pdxc,UINT32 PipeNum)
{
	return 0;
}


int USB_SB_DoVendorRequest(PDEVICE_EXTENSION_COMMON pdxc, 
						UINT8 Request, 
						UINT16 Index,
						UINT16 Value,
						PVOID TransferBuffer,
						UINT32 TransferBufferLength,
						int Dir)
{
	int r;

	PrintMsg(DEBUG_HARDWARE_USB_MISC, 
		("<DRV> DoVenderRequest: %04x, %04x, %04x",Request,Index,Value));

	if(Dir==DIR_IN)
		r = usb_control_msg(pdxc->usbdev, usb_rcvctrlpipe(pdxc->usbdev,0),
							Request, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN,
							Value, Index, TransferBuffer,
							TransferBufferLength, HZ*5);
	else
		r = usb_control_msg(pdxc->usbdev, usb_sndctrlpipe(pdxc->usbdev,0),
							Request, USB_TYPE_VENDOR | USB_RECIP_DEVICE,
							Value, Index, TransferBuffer,
							TransferBufferLength, HZ*5);
	
	PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> DoVendorRequest return: %d",r));
	return 0;
}



void download_pipe_complete(struct urb *purb)
{
	PDEVICE_EXTENSION_COMMON s = (PDEVICE_EXTENSION_COMMON) purb->context;

//	dbg("Usb download_pipe_complete transfer complete");
	PISetEvent(&s->wq_download_pipe);
}

int USB_SB_DownloadBuffer(PDEVICE_EXTENSION_COMMON pdxc, UCHAR *pBuffer,int Size)
{
	int pipe;	
	struct urb *purb = pdxc->downloadbuffer_urb;
	int r;
	unsigned char *trans_buf;

	//PrintMsg(DEBUG_HARDWARE_USB_MISC, ("download pipe size: %d,urb=%lx",Size,purb));

	if(pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1)
		pipe = usb_sndbulkpipe(pdxc->usbdev, 3);
	else
		pipe = usb_sndbulkpipe(pdxc->usbdev, 2);
	
//	dbg("download pipe : %x",pipe);
//	dbg("bulk size / max bulk out size : %d / %d",
//		len, s->bulk_out_size);
	
	trans_buf	= kmalloc(Size, GFP_KERNEL | __GFP_DMA);

	memcpy(trans_buf, pBuffer, Size);

	FILL_BULK_URB_TO(purb, pdxc->usbdev, pipe, trans_buf, Size,
					 download_pipe_complete, pdxc, 500);
	purb->transfer_flags |= USB_QUEUE_BULK;

	r = usb_submit_urb(purb);
	if(r)
	{
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("usb_submit_urb error: %d", r));
		return 1;
	}
	
	r = PIWaitEvent(&pdxc->wq_download_pipe, USB_TIMEOUT);
	if(r!=0)
	{
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("download timeout : %d",r));
		return 2;
	}

	PrintMsg(DEBUG_HARDWARE_USB_MISC, ("download return: %d", r));
	kfree(trans_buf);

	return 0;
}

void control_pipe_complete(struct urb *purb)
{
	PDEVICE_EXTENSION_COMMON s = (PDEVICE_EXTENSION_COMMON) purb->context;

	PrintMsg(DEBUG_HARDWARE_USB_MISC, ("Usb control complete"));
	PISetEvent(&s->wq_control_pipe);
}


int USB_SB_WriteInterrupt(PDEVICE_EXTENSION_COMMON pdxc, USHORT Addr, USHORT Data)
{

	int r;
	int pipe;
	struct urb *purb = NULL;
	unsigned short *transbuf;
	unsigned char *setuppacket;	

	PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> WriteInterrupt: %04x %04x",Addr,Data));

	if(pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1)
	{
		transbuf=kmalloc(8,GFP_KERNEL);
		setuppacket=kmalloc(8,GFP_KERNEL);
			
		transbuf[0] = Data;	
		transbuf[1] = Addr;	
		transbuf[2] = 0;	
		transbuf[3] = 0;	
		
		setuppacket[0] = 0x42;
		setuppacket[1] = 0x00;
		setuppacket[2] = 0xaa;
		setuppacket[3] = 0x55;
		setuppacket[4] = 0xf0;
		setuppacket[5] = 0xf0;
		setuppacket[6] = 0x08;
		setuppacket[7] = 0x00;
		
		pipe = usb_sndctrlpipe(pdxc->usbdev, 2);

//		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("control pipe : %x",pipe));
        purb = usb_alloc_urb(0);

        FILL_CONTROL_URB_TO(purb, pdxc->usbdev, pipe, setuppacket, transbuf, 8,
                            control_pipe_complete, pdxc, 100);
        usb_submit_urb(purb);

        r = PIWaitEvent(&pdxc->wq_control_pipe, USB_TIMEOUT);

        kfree(transbuf);
        kfree(setuppacket);

        PrintMsg(DEBUG_HARDWARE_USB_MISC, ("control sleep return : %d",r));
        usb_unlink_urb(purb);
        PrintMsg(DEBUG_HARDWARE_USB_MISC, ("WriteInterrupt: free urb"));
        usb_free_urb(purb);
		return r;
	}

	else
	{
		UINT16 StatusReg;
		int i;

		for(i=0;i<MAXUSBPOLLING;i++)
		{
			r = USB_SB_DoVendorRequest(pdxc, 0x14, HPI_STATUS_ADDR, 0, &StatusReg, 2, DIR_IN);
			if ( (StatusReg & 0x0010) == 0 )
				break;
			PISleep(10);
		}
		if(i<MAXUSBPOLLING)
		{
			r = USB_SB_DoVendorRequest(pdxc, 0x12, INT_PARAM_ADDR, Data, NULL, 0, DIR_OUT);
			r = USB_SB_DoVendorRequest(pdxc, 0x12, INT_INDEX_ADDR, Addr, NULL, 0, DIR_OUT);
		}
		else
		{
			PrintMsg(DEBUG_HARDWARE_USB_MISC, 
			    ("<DRV> WriteInterrupt : Error (StatusReg : %08x)", StatusReg));
			r = i;
		}

		return r;
	}

	return 0;
}

void interrupt_pipe_complete(struct urb *purb)
{
	PDEVICE_EXTENSION_COMMON s;

	s = (PDEVICE_EXTENSION_COMMON) purb->context;

	PrintMsg(DEBUG_HARDWARE_USB_MISC, 
		("USB interrupt complete: status = %d,length=%d", purb->status,purb->actual_length));

	PISetEvent(&s->wq_interrupt_pipe);
}


int USB_SB_ReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc,
			 USHORT *pIntRetVal, USHORT *pIntRetData)
{
	struct urb			*irq_urb = NULL;
	char				*irq_buf;
	int r = 0;

	irq_urb = usb_alloc_urb(0);
	irq_buf = kmalloc(4, GFP_KERNEL);
	memset(irq_buf, 0, 4);

	FILL_INT_URB(irq_urb, pdxc->usbdev, pdxc->ReadInterruptHandle,
		     irq_buf, 4, interrupt_pipe_complete, pdxc , 0x20);

	if (irq_urb)
	{
	
		r = usb_submit_urb(irq_urb);

		if (!r)
		{

			r = PIWaitEvent(&pdxc->wq_interrupt_pipe, USB_TIMEOUT);
			
			if (!r)
			{
				PrintMsg(DEBUG_HARDWARE_USB_MISC,
					("usb_submit_urb in ReadInterrupt: %d\n", r));
				memcpy(pIntRetVal, irq_buf+2, 2);
				memcpy(pIntRetData, irq_buf, 2);
			}
			usb_unlink_urb(irq_urb);
			usb_free_urb(irq_urb);
			kfree(irq_buf);
		}
	}
	else
		PrintMsg(DEBUG_HARDWARE_USB_MISC, 
			("usb_alloc_urb failed "));

	return r;
}

void stream_pipe_complete(struct urb *purb)
{
	PDEVICE_EXTENSION_COMMON s = (PDEVICE_EXTENSION_COMMON) purb->context;

	// Simply return if urb was unlinked
	if ((purb->status == -ENOENT) || 
	    (purb->status == -ECONNRESET)) {
	  return;
	}

	PISetEvent(&s->wq_stream_pipe);
}

int USB_SB_ReadStream(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pBuf, SINT32 Size)
{
	struct urb *purb = pdxc->video_urb;
	int r;

	FILL_BULK_URB(purb, pdxc->usbdev, pdxc->ReadStreamHandle, 
				  pBuf, Size, stream_pipe_complete, pdxc);
	usb_submit_urb(purb);
	
	r = PIWaitEvent(&pdxc->wq_stream_pipe, USB_TIMEOUT);
	/*
	r = sleep_on_timeout (&pdxc->wq_stream_pipe, HZ/10);
	if (r > 0) 
		r = 0;
	else
		r=1;
	*/
	return r;
}  

#if defined(__AVSYNC__)
int 
AVSynchThreadProc(void *arg)
{
	UINT32 currTime = 0;

	// added to allow graceful handling of ungracefule user space death
	// this along with checking for pending signals allows kill without zombie
	daemonize();
	reparent_to_init();

	PDEVICE_EXTENSION_COMMON pdxc = (PDEVICE_EXTENSION_COMMON)arg;
    
	PIInitializeEvent(&pdxc->wq_avsynch_wait);
	currTime = jiffies;
	PrintMsg(DEBUG_AVSYNC, ("Port %d : Elapsed: %d AVSynchThreadProc : Enter", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));

	while(pdxc->AVSynchState == 1)
	{
	  PIWaitEvent(&pdxc->wq_avsynch_wait,1);/* Wait one second */
	  
	  // check for pending signals
	  if( pdxc->AVSynchState && signal_pending(current) ) {
	    pdxc->AVSynchState = 0;
	    currTime = jiffies;
	    PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_INFORMATION_LEVEL, 
		     ("Port %d : Elapsed: %d AVSynchThreadProc : Caught Signal - Exiting",
		      pdxc->DeviceId, (currTime - pdxc->StartTime)));
	    break;
	  }

	  //Do AV sync 
	  if ( pdxc->DoAudio )
	      WatchStatistic(pdxc);

	}

	//in case of the av sync is disable, reset the task.
	PISetEvent(&(pdxc->wq_avsynch_wait));
	currTime = jiffies;
	PrintMsg(DEBUG_AVSYNC, ("Port %d : Elapsed: %d AVSynchThreadProc : Exit", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
	PIResetEvent(&(pdxc->wq_avsynch_wait));

	PISetEvent(&pdxc->AVSynchStopEvent);
}

void
WatchStatistic(PDEVICE_EXTENSION_COMMON pdxc)
{


//	#define OFFSET_WINDOW 2//59
	int i,video_fps,SamepleCount;
	UINT32 currTime = 0;
	UINT64 temp1,temp2,temp3,AudioTime,VideoTime;
	UINT32 TotalVideoFrame, DroppedVideoFrames, DuplicatedVideoFrames;
	UINT64 TotalVideoBytes;
	unsigned long DroppedAudioBlocks, DuplicatedAudioBlocks;
	unsigned long long TotalAudioBlocks;
	UINT32 elapsed;
	SINT32 AudioLag, VideoLag, MedValue;
	int currAudioReadJitterCnt[4], currVideoReadJitterCnt[4];
	int curraudioretries, currvideoretries;
	int avgAudioBitrate, avgVideoBitrate;

	if (pdxc->AVSyncCorrectionTime <128)
		SamepleCount = pdxc->AVSyncCorrectionTime;
	else 
		SamepleCount =128;

	//tried to find a way not to use the double vairiable to calculate the vidio and audio time
	//also keep the accuracty, the following algrithm cann't get good accurency

/*
	TCFGVIDEOEX* video_config;
	video_config = Config_GetCurrentVideoConfig(&(pdxc->config_manager));
	if(video_config == NULL)
		return;
*/
	// calc VideoTime in jiffies (ms would cause overflow too soon)
	video_fps=pdxc->config_manager._video_config.fpscfg.frame_rate ;
    	StreamBuffer_GetStatistics(&(pdxc->StreamBuffer), 
		&TotalVideoFrame, &DroppedVideoFrames, &DuplicatedVideoFrames, &TotalVideoBytes);
	VideoTime = 0;
	if(video_fps != 0)
	{
		temp1 = TotalVideoFrame/video_fps;
		temp2 = TotalVideoFrame%video_fps;
		temp3 = (temp2 *1001)%video_fps ;
		VideoTime = (temp1 * 1001 * HZ) + 
		  ((temp2 * 1001 / video_fps) * HZ) + 
		  (temp3 * HZ / video_fps);
	}
 	// calc AudioTime in jiffies
	AUDIO_CONFIG *audio_config;
	audio_config = Config_GetCurrentAudioConfig(&(pdxc->config_manager));
	if(audio_config == NULL)
		return;
    	CAudioStreamBuffer_GetStatistics(pdxc->AudioStreamBuffer, &DroppedAudioBlocks, &DuplicatedAudioBlocks, &TotalAudioBlocks);
	AudioTime = 0;
	if(audio_config->AvgBytesPerSec != 0)
	{
		temp1 =  (audio_config->AvgBytesPerSec / HZ);
		temp2 = TotalAudioBlocks / temp1;
		temp3 = TotalAudioBlocks % temp1;
		AudioTime = temp2 * pdxc->AudioBlockSize 
			+ (temp3 * pdxc->AudioBlockSize )/temp1;
	}
	// calc and save AV Offset stats
	currTime = jiffies;
	if(pdxc->TimerCount == 0) {
	  // first time thru - use AudioTime to backtrack and set StartTime
	  pdxc->StartTime = currTime - AudioTime;
	  // init all AV Offset samples and MedValue
	  for(i=0; i< SamepleCount; i++)
	    pdxc->AVOffset[i] = (VideoTime - AudioTime);
	  MedValue = pdxc->AVOffset[0];
	  pdxc->VideoAddCnt = 0;
	  pdxc->VideoDropCnt = 0;
	  for(i=0; i<4; i++) {
	    pdxc->lastAudioReadJitterCnt[i] = 0;
	    pdxc->lastVideoReadJitterCnt[i] = 0;
	  }
	  pdxc->lastTotalVideoBytes = 0;
	  pdxc->lastTotalAudioTime = 0;
	}
	else {
	  // shift in current AV Offset sample
	  for( i = 0; i < (SamepleCount - 1); i++)
	    pdxc->AVOffset[i] = pdxc->AVOffset[i+1];
	  pdxc->AVOffset[pdxc->AVSyncCorrectionTime-1] = (VideoTime - AudioTime);
	  // calc MedValue of AV Offset samples
	  if (SamepleCount>1)
	    MedValue = MediumValue(pdxc->AVOffset, pdxc->AVSyncCorrectionTime);
	  else
	    MedValue = pdxc->AVOffset[0];
	}

	// calc current value of A V Lag
	elapsed = (currTime - pdxc->StartTime);
	AudioLag = (elapsed - AudioTime);
	VideoLag = (elapsed - VideoTime);

	//Check if audio needs correction - if so set AdjBlkCnt
	// make sure it is 0 - if not, audio thread is still working on recovery
	if( pdxc->AudioAdjBlkCnt == 0 ) {
	  if( (0 - AudioLag) > (pdxc->AudioCorrectionDelayTime * HZ / 1000) ) {
	    pdxc->AudioAdjBlkCnt -= ((0 - AudioLag) - (pdxc->AudioCorrectionDelayTime * HZ / 1000))
	      * (audio_config->AvgBytesPerSec / HZ) / pdxc->AudioBlockSize;
	  }
	  else if( AudioLag > (pdxc->AudioCorrectionDelayTime * HZ / 1000) ) {
	    pdxc->AudioAdjBlkCnt += (AudioLag - (pdxc->AudioCorrectionDelayTime * HZ / 1000))
	      * (audio_config->AvgBytesPerSec / HZ) / pdxc->AudioBlockSize;
	  }
	}

	// print stats out once every minute
	//	if (currTime - pdxc->LastAVSyncTime  > 60 * HZ) {
	if (pdxc->TimerCount && ((pdxc->TimerCount % 60) == 0)) {
	  //get Bitrate stats in Kbps
	  avgAudioBitrate = audio_config->AvgBytesPerSec * 8 / 1000
	    * (AudioTime - pdxc->lastTotalAudioTime) / (currTime - pdxc->LastAVSyncTime) ;
	  avgVideoBitrate = ((TotalVideoBytes - pdxc->lastTotalVideoBytes) * 8 
			     / (currTime - pdxc->LastAVSyncTime)) * HZ / 1000;
	  pdxc->lastTotalAudioTime = AudioTime;
	  pdxc->lastTotalVideoBytes = TotalVideoBytes;
	  // get readJitter stats
	  for(i=0; i<4; i++) {
	    currAudioReadJitterCnt[i] = pdxc->AudioReadJitterCnt[i] - pdxc->lastAudioReadJitterCnt[i];
	    pdxc->lastAudioReadJitterCnt[i] += currAudioReadJitterCnt[i];
	    currVideoReadJitterCnt[i] = pdxc->VideoReadJitterCnt[i] - pdxc->lastVideoReadJitterCnt[i];
	    pdxc->lastVideoReadJitterCnt[i] += currVideoReadJitterCnt[i];
	  }
	  curraudioretries = pdxc->audioretries - pdxc->lastaudioretries;
	  currvideoretries = pdxc->videoretries - pdxc->lastvideoretries;
	  PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_INFORMATION_LEVEL, 
		   ("Port %d : Elapsed: %d AVOffset:%d ALag:%d VLag:%d AKbps:%d VKbps:%d ABlkDrop:%d:%d VFrmDrop:%d:%d Aadd:drp %d:%d Vadd:drp %d:%d AJit:%d:%d:%d:%d ATmO:%d %d VJit:%d:%d:%d:%d VTmO:%d %d", 
		    pdxc->DeviceId, elapsed, MedValue,
		    AudioLag, VideoLag, avgAudioBitrate, avgVideoBitrate,
		    (int)DroppedAudioBlocks, (int)DuplicatedAudioBlocks,
		    (int)DroppedVideoFrames, (int)DuplicatedVideoFrames,
		    (int)(pdxc->AudioAddBlkCnt * pdxc->AudioBlockSize * HZ / audio_config->AvgBytesPerSec),
		    (int)(pdxc->AudioDropBlkCnt * pdxc->AudioBlockSize * HZ / audio_config->AvgBytesPerSec),
		    (int)pdxc->VideoAddCnt, (int)pdxc->VideoDropCnt,
		    currAudioReadJitterCnt[0],currAudioReadJitterCnt[1],
		    currAudioReadJitterCnt[2],currAudioReadJitterCnt[3],
		    pdxc->lastaudioretries, curraudioretries, 
		    currVideoReadJitterCnt[0],currVideoReadJitterCnt[1],
		    currVideoReadJitterCnt[2],currVideoReadJitterCnt[3],
		    pdxc->lastvideoretries, currvideoretries
		    ));
	  pdxc->lastaudioretries += curraudioretries;
	  pdxc->lastvideoretries += currvideoretries;
	  pdxc->LastAVSyncTime = currTime;
	}

	//check if the medium delay has reached the setting value
	//send a notification to FW to drop some video samples
	/* initial AV Offset is not allowed to persist - so no need to save it
	if(pdxc->TimerCount == (pdxc->AVSyncCorrectionTime -1))
	{
	  pdxc->InitAVOffset = MedValue;
	  PrintMsg(DEBUG_AVSYNC, ("Port %d : Initial AV Offset: %d", pdxc->DeviceId, pdxc->InitAVOffset));

	}
	else */
	if( ( (pdxc->TimerCount%SamepleCount) == 0 )&&
		(pdxc->TimerCount>SamepleCount ))
	// we do adjustment in the setting time interval
	{
	  SINT32 Offset1 = MedValue;// - pdxc->InitAVOffset;	  	
	  SINT32 maxOffset = pdxc->AVSyncCorrectionDelayTime * HZ / 1000;
	  // if A/V offset is larger than the setting threshould, do adjustment
	  if((Offset1 >= maxOffset)|| ((0 - Offset1) >= maxOffset))
	    {
	      int temp = Offset1*audio_config->SampleRate/HZ;
	      
	      if (Offset1 <0)
		{
		  Offset1= -Offset1; 
		  temp = -(Offset1*audio_config->SampleRate/HZ);
		}
	      
	      NotifyAVOffset(pdxc, temp);
	      if (temp >0)
		{
		  pdxc->VideoDropCnt += Offset1;
		}
	      else
		{
		  pdxc->VideoAddCnt += Offset1;
		}
	    }
	}

	//time count
	pdxc->TimerCount ++;

}
SINT32 MediumValue(SINT32 Data[], int Size)
{
	SINT32 Data1[128];
	int i;
	int Sum=0;

	if(Size>128)
		return 0;

	for(i=0;i<Size;i++)
		Sum += Data[i];

	return Sum/Size;
}
/* 
SINT32 MediumValue(SINT32 Data[], int Size)
{
	SINT32 Data1[128];
	int i;
	int Sum;

	if(Size>128)
		return 0;

	for(i=0;i<Size;i++)
		Data1[i] = Data[i];

	Sort(Data1, Size);

	Sum = 0;
	for(i=0;i<5;i++)
		Sum += Data1[i+(Size-5)/2];

	return Sum/5;
}
void Sort(SINT32 Data[], SINT32 Size)
{
	int i,j, index; 
	SINT32 t, Min;

	for(i=0; i<Size-1; i++) {
	  Min = Data[i];
	  index = i;
	  for(j=i+1; j<Size; j++)
	    {
	      if(Data[j] < Min)
		{
		  Min = Data[j];
		  index = j;
		}
	    }
	  if(index != i)
	    {
	      t = Data[i];
	      Data[i] = Data[index];
	      Data[index] = t;
	    }
	}
}
*/

#endif

int StreamThreadProc(void *arg)
{
	int	r;
	int init = 0;

	//SJ init time counters
	UINT32 currTime = 0;
	UINT32 lastTime = 0;
	struct timeval currUTime, lastUTime;
	int i, readJitterIdx;

	// added to allow graceful handling of ungracefule user space death
	// this along with checking for pending signals allows kill without zombie
	daemonize();
	reparent_to_init();

	PDEVICE_EXTENSION_COMMON pdxc = (PDEVICE_EXTENSION_COMMON)arg;

	currTime = jiffies;
	PrintMsg(DEBUG_VIDEO_CAPTURE, ("Port %d : Elapsed: %d StreamThreadProc : Enter", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));

	do_gettimeofday(&currUTime);
	lastUTime.tv_sec = currUTime.tv_sec;
	lastUTime.tv_usec = currUTime.tv_usec;
	for( i=0; i<4; i++ ) {
	  pdxc->VideoReadJitterCnt[i] = 0;
	}

	while(pdxc->ReadStreamState == 1)
	{
	  r = USB_SB_ReadStream(pdxc, pdxc->pReadStreamBlock, pdxc->BlockSize);

	  // check for pending signals
	  if( pdxc->ReadStreamState && signal_pending(current) ) {
	    pdxc->ReadStreamState = 0;
	    currTime = jiffies;
	    PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_INFORMATION_LEVEL, 
		     ("Port %d : Elapsed: %d StreamThreadProc : Caught Signal - Exiting",
		      pdxc->DeviceId, (currTime - pdxc->StartTime)));
	    break;
	  }

	  if(pdxc->ReadStreamState == 1 && r == 0 && (pdxc->video_urb->actual_length != 0))
	  {
		if (!init)//wait for video start before activate audio
			{	
			init = 1;
/*	        if ((pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1) &&
	        (pdxc->BoardInfo.RevisionNum == 0x0209))*/
		if (pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1)
				PISetEvent(&pdxc->wq_audio_wait);
	      }
		
		//SJ collect and report read jitter stats
		currTime = jiffies;
		do_gettimeofday(&currUTime);
		// count number of reads that took <20, <40, <60, 80-or-more ms
		readJitterIdx = ((currUTime.tv_sec - lastUTime.tv_sec) * 1000 +
				 (currUTime.tv_usec - lastUTime.tv_usec) / 1000) / 20;
		if( readJitterIdx > 3 ) readJitterIdx = 3;
		pdxc->VideoReadJitterCnt[readJitterIdx]++;

		lastTime = currTime;
		lastUTime.tv_sec = currUTime.tv_sec;
		lastUTime.tv_usec = currUTime.tv_usec;

		ReadVideoStreamCallBack(pdxc);
	  }
	  else if (r==1)
	  {
	    pdxc->videoretries++;
	    currTime = jiffies;
	    PrintMsg(DEBUG_VIDEO_CAPTURE, ("Port %d : Elapsed: %d Video read timeout", 
				    pdxc->DeviceId, (currTime - pdxc->StartTime)));
	      /*
	       * If a read timeout occurs, clean up the old URB which
	       * is still in the pipe so that the next time the read
	       * thread tries to acquire a frame-buffer, a new (valid)
	       * URB will be resubmitted. Failure todo this will
	       * create ENXIO errors.
	       */
	      usb_unlink_urb(pdxc->video_urb);
	  }
	}
	usb_unlink_urb(pdxc->video_urb);
	currTime = jiffies;
	PrintMsg(DEBUG_VIDEO_CAPTURE, ("Port %d : Elapsed: %d StreamThreadProc : Exit", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
	PISetEvent(&pdxc->ReadStreamStopEvent);
}

void audio_stream_pipe_complete(struct urb *purb)
{
	PDEVICE_EXTENSION_COMMON s = (PDEVICE_EXTENSION_COMMON) purb->context;

	// Simply return if urb was unlinked
	if ((purb->status == -ENOENT) || 
	    (purb->status == -ECONNRESET)) {
	  return;
	}
	//PrintMsg(DEBUG_AUDIO_CAPTURE, ("audio_stream_pipe_complete "));
	PISetEvent(&s->wq_audio_stream_pipe);
}

int USB_SB_ReadAudioStream(PDEVICE_EXTENSION_COMMON pdxc)
{
	struct urb *purb = pdxc->audio_urb;
	int r;
	
	//PrintMsg(DEBUG_AUDIO_CAPTURE, ("stream audio read "));
	FILL_BULK_URB(purb, pdxc->usbdev, pdxc->ReadAudioStreamHandle, 
				  pdxc->pReadAudioStreamBlock, pdxc->AudioBlockSize, audio_stream_pipe_complete, pdxc);

	r = usb_submit_urb(purb);

	if (r)
	{
		PrintMsg(DEBUG_AUDIO_CAPTURE, ("<USB ERROR> usb_submit_urb error return : %d,%d",r,purb->status));
		return r;
	}	

	r = PIWaitEvent(&pdxc->wq_audio_stream_pipe, USB_TIMEOUT);
	/*
	r = sleep_on_timeout (&pdxc->wq_audio_stream_pipe, HZ/10);
	if (r > 0) 
		r = 0;
	else
		r=1;
	*/
//	PrintMsg(DEBUG_AUDIO_CAPTURE, ("stream audio sleep return : %d",r));

	return r;
}

void audio_timeout(PDEVICE_EXTENSION_COMMON pdxc)
{
//	PrintMsg(DEBUG_AUDIO_CAPTURE, ("audio timeout "));
	wake_up_interruptible(&pdxc->wq_audio_wait);

}
int AudioStreamThreadProc(void *arg)
{
	int	r;

	//SJ init time counters
	UINT32 currTime = 0;
	UINT32 lastTime = 0;
	struct timeval currUTime, lastUTime;
	int i, readJitterIdx;

	// added to allow graceful handling of ungracefule user space death
	// this along with checking for pending signals allows kill without zombie
	daemonize();
	reparent_to_init();

	PDEVICE_EXTENSION_COMMON pdxc = (PDEVICE_EXTENSION_COMMON)arg;
	struct urb *purb = pdxc->audio_urb;
	unsigned short data = 0x0;

	do_gettimeofday(&currUTime);
	lastUTime.tv_sec = currUTime.tv_sec;
	lastUTime.tv_usec = currUTime.tv_usec;
	for( i=0; i<4; i++ ) {
	  pdxc->AudioReadJitterCnt[i] = 0;
	}
	pdxc->AudioAdjBlkCnt = 0;
	pdxc->AudioAddBlkCnt = 0;
	pdxc->AudioDropBlkCnt = 0;

	/*if ((pdxc->BusType == WisUsb) &&
	    (pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1) &&
	    (pdxc->BoardInfo.RevisionNum == 0x0209))*/
	if ((pdxc->BusType == WisUsb) &&
	    (pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1))
	{
		r = WriteCBusRegFW(pdxc, 0x1020, 0x0001);//start audio
		PrintMsg(DEBUG_AUDIO_CAPTURE, ("<DRV>write r %x, data=%x",r,data));
		interruptible_sleep_on(&pdxc->wq_audio_wait);//wait for video
		PrintMsg(DEBUG_AUDIO_CAPTURE, ("<DRV> AudioStreamThreadProc : Enter"));		
		init_timer(&pdxc->audio_timer);//for data length zero delay
		pdxc->audio_timer.function = audio_timeout;
		pdxc->audio_timer.data = (int)pdxc;
		pdxc->audio_timer.expires = jiffies + HZ/100;
		if (!pdxc->testFlag)
		{
			pdxc->testFlag++;
			PrintMsg(DEBUG_HARDWARE_MISC, ("pdxc->BoardInfo.BusMode  : %d", pdxc->BoardInfo.BusMode));
			pdxc->ReadAudioStreamHandle = usb_rcvbulkpipe(pdxc->usbdev, 5);	
			PrintMsg(DEBUG_AUDIO_CAPTURE, ("<DRV> pdxc->usbdev %x",
				     (int)pdxc->usbdev));
			if (usb_set_interface(pdxc->usbdev, 2, 1) < 0) 
			{
				PrintMsg(DEBUG_HARDWARE_MISC, ("usb_set_interface 2 fail"));
				return -EINVAL;
			}
		}
	}
	else {
	  currTime = jiffies;
	  PrintMsg(DEBUG_AUDIO_CAPTURE, ("Port %d : Elapsed: %d AudioStreamThreadProc : Enter", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
	}


	while(pdxc->ReadAudioStreamState != 0)
	{

		r = USB_SB_ReadAudioStream(pdxc);
//	    PrintMsg(DEBUG_AUDIO_CAPTURE, ("<DRV> r=%d,length=%d,status=%x",r,purb->actual_length,purb->status));

		// check for pending signals
		if( pdxc->ReadAudioStreamState && signal_pending(current) ) {
		  pdxc->ReadAudioStreamState = 0;
		  currTime = jiffies;
		  PrintMsg(DEBUG_ALWAYS_DISPLAY|DEBUG_INFORMATION_LEVEL, 
			   ("Port %d : Elapsed: %d AudioStreamThreadProc : Caught Signal - Exiting",
			    pdxc->DeviceId, (currTime - pdxc->StartTime)));
		  break;
		}

	    if ((purb->actual_length == 0)||(purb->status))
	    {
	        /*if ((pdxc->BusType == WisUsb) &&
	        (pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1) &&
	        (pdxc->BoardInfo.RevisionNum == 0x0209))*/
		if ((pdxc->BusType == WisUsb) &&
	        (pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1))
	        {
		  currTime = jiffies;
		  PrintMsg(DEBUG_AUDIO_CAPTURE, ("Port %d : Elapsed: %d Audio - Zero length return", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
				add_timer(&pdxc->audio_timer);
				interruptible_sleep_on(&pdxc->wq_audio_wait);
	         }
	     }
	     else if(pdxc->ReadAudioStreamState ==1 && r == 0 )
	        {
		  //SJ collect and report read jitter stats
		  //		  currTime = jiffies;
		  do_gettimeofday(&currUTime);
		  // count number of reads that took <10, <20, <30, 40-or-more ms
		  readJitterIdx = ((currUTime.tv_sec - lastUTime.tv_sec) * 1000 +
				   (currUTime.tv_usec - lastUTime.tv_usec) / 1000) / 10;
		  if( readJitterIdx > 3 ) readJitterIdx = 3;
		  pdxc->AudioReadJitterCnt[readJitterIdx]++;

		  //		  lastTime = currTime;
		  lastUTime.tv_sec = currUTime.tv_sec;
		  lastUTime.tv_usec = currUTime.tv_usec;

		  //Do the audio read callback - add/drop as needed to meet AdjBlkCnt
		  if( pdxc->AudioAdjBlkCnt < 0 ) {
		      pdxc->AudioDropBlkCnt++;
		      pdxc->AudioAdjBlkCnt++;
		  }
		  else {
		      ReadAudioStreamCallBack(pdxc);
		      if( pdxc->AudioAdjBlkCnt > 0 ) {
			ReadAudioStreamCallBack(pdxc);
			pdxc->AudioAddBlkCnt++;
			pdxc->AudioAdjBlkCnt--;
		      }
		  }		    
		    
		}
	    else if (r==1)
	        {
		  pdxc->audioretries++;
		  currTime = jiffies;
		  PrintMsg(DEBUG_AUDIO_CAPTURE, ("Port %d : Elapsed: %d Audio read timeout", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
	    /*
		     * NOTE: very important.
		     * The Audio pipe is very sensitive in that
		     * processing read requests must be handled
		     * in pseudo-realtime fashion. In the event
		     * that a read request for an audio stream
		     * buffer did not go through, we flush the
		     * read pipe by cancelling the request so
		     * that the next time through, we can resubmit
		     * the URB. Failure todo so will yield ENXIO
		     * errors; what's more, a "stuck" URB read request
		     * will kill the audio unless we cancel the URB
		     * with usb_unlink_urb()
		     */
		    usb_unlink_urb(pdxc->audio_urb);
		}
	}

	/*if ((pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1) &&
	    (pdxc->BoardInfo.RevisionNum == 0x0209))*/
	if ((pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1))
	{
		usb_unlink_urb(pdxc->audio_urb);
		del_timer(&pdxc->audio_timer);
	}
//make sure we dont leave without unlink
	else {
	  usb_unlink_urb(pdxc->audio_urb);
	}
	currTime = jiffies;
	PrintMsg(DEBUG_AUDIO_CAPTURE, ("Port %d : Elapsed: %d AudioStreamThreadProc : Exit", 
					  pdxc->DeviceId, (currTime - pdxc->StartTime)));
	PISetEvent(&pdxc->ReadAudioStreamStopEvent);
	return 0;
}

int USB_SB_StartReadVideoStream(PDEVICE_EXTENSION_COMMON pdxc)
{
    //dxc.TotalVideoChannel = VideoChannelCount;

	pdxc->ReadStreamState = 1;
	pdxc->videoretries = 0;
	pdxc->StartTime = jiffies;

	PIInitializeEvent(&pdxc->wq_audio_wait);
    
	kernel_thread(StreamThreadProc, (void *)pdxc, 0);
#if defined(__AVSYNC__)
	if (pdxc->DoAudio )
	{
	    pdxc->LastAVSyncTime = 0;
	    pdxc->AVSynchState = 1;
	    //pdxc->TotalAudioTime = 0;
	    pdxc->TimerCount=0;

	    PIInitializeEvent(&pdxc->wq_avsynch_wait);
	    kernel_thread(AVSynchThreadProc, (void *)pdxc, 0);
	}

#endif 

	return 0;
}

int USB_SB_StartReadAudioStream(PDEVICE_EXTENSION_COMMON pdxc)
{

	pdxc->ReadAudioStreamState = 1;
	pdxc->audioretries = 0;
	kernel_thread(AudioStreamThreadProc, (void *)pdxc, 0);

	return 0;
}

void USB_SB_StopReadAllPipes(PDEVICE_EXTENSION_COMMON pdxc)
{
	int rv;

	//wakeup all the thread for termination
	PISetEvent(&pdxc->wq_control_pipe);
	PISetEvent(&pdxc->wq_interrupt_pipe);
	PISetEvent(&pdxc->wq_download_pipe);

#if defined(__AVSYNC__)
	if( pdxc->AVSynchState ) {
	  pdxc->AVSynchState = 0;
	  PISetEvent(&pdxc->wq_avsynch_wait );
	  if ( pdxc->DoAudio )
	    rv = PIWaitEvent(&pdxc->AVSynchStopEvent, 1);
	}
#endif

	//[pma] we must check if we need to wait for the event
	if( pdxc->ReadStreamState ) {
	  pdxc->ReadStreamState = 0;
	  PISetEvent(&pdxc->wq_stream_pipe);
	  if (1 == pdxc->DoVideo )
	    rv = PIWaitEvent(&pdxc->ReadStreamStopEvent, 5);
	}

	if( pdxc->ReadAudioStreamState ) {
	  pdxc->ReadAudioStreamState = 0;
	  PISetEvent(&pdxc->wq_audio_stream_pipe);
	  if ( pdxc->DoAudio )
	    rv = PIWaitEvent(&pdxc->ReadAudioStreamStopEvent, 5);
	}

	(pdxc->hw_funcs.ResetBusDevice)(pdxc);
}

int USB_SB_StartReadInterrupt(PDEVICE_EXTENSION_COMMON pdxc)
{
	return 0;
}

void USB_SB_ResetBusDevice(PDEVICE_EXTENSION_COMMON pdxc)
{
	 int r;
	/* if ((pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1) &&
		(pdxc->BoardInfo.RevisionNum == 0x0209))*/
	if ((pdxc->BoardInfo.BusMode == UM_USB_ONCHIP_MODE1))
	 {
		r = usb_clear_halt (pdxc->usbdev, usb_sndctrlpipe(pdxc->usbdev, 2));
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> USB_SB_ResetBusDevice %d",r));
		r = usb_clear_halt (pdxc->usbdev, usb_sndctrlpipe(pdxc->usbdev, 3));
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> USB_SB_ResetBusDevice %d",r));
		r = usb_clear_halt (pdxc->usbdev, pdxc->ReadStreamHandle);
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> USB_SB_ResetBusDevice %d",r));
			r = usb_clear_halt (pdxc->usbdev, pdxc->ReadAudioStreamHandle);
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> USB_SB_ResetBusDevice %d",r));
			r = usb_clear_halt (pdxc->usbdev, pdxc->ReadInterruptHandle);
		PrintMsg(DEBUG_HARDWARE_USB_MISC, ("<DRV> USB_SB_ResetBusDevice %d",r));
	 }
}

NTSTATUS USB_SB_SoftwareReset(PDEVICE_EXTENSION_COMMON pdxc)
{
	return 0;
}

