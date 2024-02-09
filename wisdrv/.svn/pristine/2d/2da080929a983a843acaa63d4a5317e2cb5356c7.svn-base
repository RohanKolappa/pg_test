/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        AVSync.h
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

#ifndef _AV_SYNC_H_
#define _AV_SYNC_H_

typedef unsigned long HAVSYNC;
#define NOTFOUNDSYNC  0x00ffdead
#ifdef __cplusplus
extern "C" {
#endif

		//Export Methods
		long AVSync_Init(
				HAVSYNC* pHandle,
				unsigned long lFrameRate,
				unsigned long lFrameRateScale,
				unsigned long lAudioSampleRate,
				unsigned long lAudioChannelNum,
				unsigned long lAudioBitsPerSample,
				unsigned long lMaxHoldingVideoFrameNum);
		void AVSync_Uninit(HAVSYNC h);
		
		long AVSync_ReceiveAudio(
				HAVSYNC h,
				unsigned short* pData,
				unsigned long lLen
				);//return value points to the new total offset
		long AVSync_ReceiveVideo(
				HAVSYNC h,
				unsigned long lVip,
				unsigned long lFingerprint,
				unsigned long lRefcount
				);//return value points to the new total offset

#ifdef __cplusplus
}
#endif

#endif

/******************************** end of AVSync.h ****************************/

