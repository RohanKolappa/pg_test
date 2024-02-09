/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        AVSync.c
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

//
//                     ()---()
//                     / o o \
//                ___-(       )-___
// --------------(,,,----\_/----,,,)--------------------------------------
//                        O
//Created by Xiaoli Xiong
//
//07/03/2003
//
// 
//      
//
//
// -----------------------------------------------------------------------
//                    | |---| |
//                  _-+ |   | +-_
//                 (,,,/     \,,,)

#include "AVSync.h"
#include "common.h"

//Data structure define
//Platform dependeded
//
//Macros define, We use 'As' as every macro's prefix
//
//#define _SCALE  10000000L
#if defined(_WINDOWS) || defined(WIN32)

#ifdef DRIVER
	#include "platform.h"
	#include "common.h"
	//LOCK METHODS
	#define AsDECLARELOCK(lock) KSPIN_LOCK m_##lock;KIRQL m_save##lock;
	#define AsINITLOCK(p,lock) KeInitializeSpinLock(&p->m_##lock)
	#define AsUNINITLOCK(p,lock) 
	#define AsLOCK(p,lock) KeAcquireSpinLock(&p->m_##lock,&p->m_save##lock)
	#define AsUNLOCK(p,lock) KeReleaseSpinLock(&p->m_##lock,p->m_save##lock)
	//Memory managing
	#define AsMALLOC(size) PIAllocMem(size)
	#define AsFREE(p) PIFreeMem(p)
#else
	#include <stdlib.h>
	#include <malloc.h>
	#include <memory.h>
	#include <Windows.h>

	#define AsDECLARELOCK(lock)  CRITICAL_SECTION m_##lock;
	#define AsINITLOCK(p,lock)   InitializeCriticalSection(&p->m_##lock);
	#define AsUNINITLOCK(p,lock) DeleteCriticalSection(&p->m_##lock);
	#define AsLOCK(p,lock)  EnterCriticalSection(&p->m_##lock);
	#define AsUNLOCK(p,lock) LeaveCriticalSection(&p->m_##lock) 

	//Memory managing
	#define AsMALLOC(size) malloc(size)
	#define AsFREE(p) free(p)
	#define PrintMsg(n,k) 
#endif /* DRIVER */

#elif defined (_LINUX) && defined(__KERNEL__)

#include "platform.h"
#include <linux/module.h>
#include <linux/spinlock.h>
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
#include <asm/uaccess.h>
#include <asm/div64.h> /* For do_div() */
//#define do_div(x,y) 
#define AsDECLARELOCK(lock)  spinlock_t  m_##lock; unsigned long m_flags##lock;
#define AsINITLOCK(p,lock)   spin_lock_init(&p->m_##lock);
#define AsUNINITLOCK(p,lock) 
#define AsLOCK(p,lock)  spin_lock_irqsave(&p->m_##lock,p->m_flags##lock);
#define AsUNLOCK(p,lck) spin_unlock_irqrestore(&p->m_##lck,p->m_flags##lck);
//Memory managing
#define AsMALLOC(size) kmalloc(size, GFP_KERNEL)
#define AsFREE(p) kfree(p)
#define LONGLONG unsigned int
//#define LONGLONG u64
//#define LONGLONG unsigned long long

#else
/* Windows again, not driver version */
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <Windows.h>
#define AsDECLARELOCK(lock)  CRITICAL_SECTION m_##lock;
#define AsINITLOCK(p,lock)   InitializeCriticalSection(&p->m_##lock);
#define AsUNINITLOCK(p,lock) DeleteCriticalSection(&p->m_##lock);
#define AsLOCK(p,lock)  EnterCriticalSection(&p->m_##lock);
#define AsUNLOCK(p,lock) LeaveCriticalSection(&p->m_##lock) 
//Memory managing
#define AsMALLOC(size) malloc(size)
#define AsFREE(p) free(p)
#define PrintMsg(n,k) 
#endif


//
#define AsSIZE64K 	64*1024
#define AsSIZE32K 32*1024
#define AsHASHRESERVE 0xFFFF
#define MINSYNCRADIUSVIDEOFRAMENUM 2
#define AUDIOCOUNT unsigned long

#define AVSync_TranlatingCountFromVideoFrameToAudioSample(p,lVFrameCount) \
	(lVFrameCount*p->m_lAudioWORDRateByVFrame+((lVFrameCount/p->m_lAudioWORDRateDividerByVFrame)*p->m_lAudioWORDRateResidualByVFrame))


typedef struct _HashBarrel
{
	unsigned short* m_pFPHash;
	unsigned short* m_pFPHashIndex;
	unsigned short m_lFPHashIndexPtr;
	unsigned long  m_lFPHashIndexSize;

	AsDECLARELOCK(hash)

}HashBarrel;

int HashBarrel_Init(HashBarrel* p);
void HashBarrel_Uninit(HashBarrel* p);
void HashBarrel_Reset(HashBarrel* p,unsigned long lHashIndexSize);
int HashBarrel_Lookup(HashBarrel* p,unsigned long lFP,unsigned long* plPostion);
int HashBarrel_AddEntry(HashBarrel* p,unsigned long lFingerprint);


//
//Fingerprint operations
//
#define FPDiffFromDword(fp) (unsigned short)((((unsigned char*)&fp)[3]<<8)|(((unsigned char*)&fp)[2]))
#define FPEntryFromDword(fp) (unsigned short)((((unsigned char*)&fp)[1]<<8)|(((unsigned char*)&fp)[0]))


typedef struct _VideoFrameInfo
{
	unsigned long m_lVip;
	unsigned long m_lFingerprint;
	unsigned long m_lRefcount;
}VideoFrameInfo;

#define MAXHOLDINGMOREVIDEO 20
typedef struct _AVSync
{

	//{{Init
	AUDIOCOUNT m_lAudioWORDRateByVFrame;
	AUDIOCOUNT m_lAudioWORDRateResidualByVFrame;
	AUDIOCOUNT m_lAudioWORDRateDividerByVFrame;

	unsigned long m_lAudioSampleChannelBits;

	unsigned char m_ucSyncIntervalFrameNum;
	unsigned char m_ucSyncRadiusFrameNum;

	//Init}}
	
	HashBarrel m_hb;
	
	unsigned long m_lMaxVFramesNum;
	VideoFrameInfo* m_pVFrames;
	unsigned long m_lVFramesNum;

	
	//{{Common, need to protect the access action
//Audio Data count, We use 32bits for matching with the hardware's reference aduio count, 
//it counts the number of word 
	unsigned long m_lAudioDataCount;
	unsigned long m_lSyncStartPostion;
	unsigned long m_lSyncStartVIP;
	unsigned long m_lSyncEndVIP;
//Sometime, the	m_lAudioBufferStartPostion is not equal to m_lSyncStartPostion 
//when the audio buffer recording doesn't start from every audio block's start postion
	unsigned long m_lAudioBufferStartPostion;
	//Common}}
	long m_lAudioOffsetByWORD;
	//{{Thread safe common 
	unsigned long m_ulCurFingerprint;//Loop fingerprint	
	int m_bStartFillFPBuffer;//For changing m_llAudioBufferStartTime and skip the first 31-bits

	int m_bFPBufferReady;//Status
	//Thread safe common }}

	AsDECLARELOCK(SyncCalling)
	AsDECLARELOCK(Common)

}AVSync;
//Private Methods for AVSync
long AVSync_Sync(AVSync* p,int bClearBuffer);//return value points to the new total offset

int AVSync_PrepareToNextSync(AVSync* p,unsigned long lSyncStartVip,unsigned long lSyncEndVip,unsigned long lAudioHashSize);

//

//
//
//  Hash Barrel operations 
//
//
int HashBarrel_Init(HashBarrel* p)
{
	p->m_pFPHash =(unsigned short *)AsMALLOC( sizeof(unsigned short)*AsSIZE64K );
	if( p->m_pFPHash == NULL)
		return 0;
	p->m_pFPHashIndex = (unsigned short *)AsMALLOC( sizeof(unsigned short)*AsSIZE64K );
	if( p->m_pFPHashIndex == NULL)
	{
		AsFREE(p->m_pFPHash);
		return 0;
	}
	AsINITLOCK(p,hash);
	return 1;
}
void HashBarrel_Uninit(HashBarrel* p)
{
	AsFREE(p->m_pFPHash);
	AsFREE(p->m_pFPHashIndex);
	AsUNINITLOCK(p,hash);
}
void HashBarrel_Reset(HashBarrel* p,unsigned long lHashIndexSize)
{
	AsLOCK(p,hash);
	memset(p->m_pFPHash,AsHASHRESERVE,AsSIZE64K*sizeof(unsigned short));
	p->m_lFPHashIndexSize=lHashIndexSize>AsSIZE32K?AsSIZE32K:lHashIndexSize;
	p->m_lFPHashIndexPtr =0;
	AsUNLOCK(p,hash);
}
//
//Note: the postion should be added 32 as the real postion
//
int HashBarrel_Lookup(HashBarrel* p,unsigned long lFP,unsigned long* plPostion)
{
	int bRet = 0;
	unsigned short usEntry;
	unsigned short sDiffFP;
	unsigned short sEntryFP;

	sDiffFP = FPDiffFromDword(lFP);
	sEntryFP = FPEntryFromDword(lFP);

	AsLOCK(p,hash);
	if( p->m_pFPHash[sEntryFP] !=AsHASHRESERVE)
	{
		//if( (p->m_pFPHashIndex[2*p->m_pFPHash[sEntryFP]]&0x7FFF) ==  (sDiffFP&0x7FFF) )
		if( p->m_pFPHashIndex[2*p->m_pFPHash[sEntryFP]] ==  sDiffFP )//Just GO-Stream use 31 bits as fingerprint
		{
			*plPostion = p->m_pFPHash[sEntryFP]+32;
			bRet =  1;
		}
		else
		{//Lookup more linked entrys
			usEntry = p->m_pFPHash[sEntryFP];
			while(	p->m_pFPHashIndex[ 2*usEntry+1 ] >0)
			{
				usEntry = p->m_pFPHashIndex[ 2*usEntry+1];
				//if( (p->m_pFPHashIndex[2*usEntry]&0x7FFF) ==  (sDiffFP&0x7FFF) )
				if( p->m_pFPHashIndex[2*usEntry] ==  sDiffFP )
				{
					
					*plPostion = usEntry+32;
					bRet =  1;
				}
			}
			
		}
	}
	AsUNLOCK(p,hash);
	return bRet;
}
//
//Return value:
//TRUE for buffer is ready
//
int HashBarrel_AddEntry(HashBarrel* p,unsigned long lFingerprint)
{
	int bBufferReady = 0;
	unsigned short  usEntry =FPEntryFromDword(lFingerprint);
	unsigned short  usDiff =FPDiffFromDword(lFingerprint );

	AsLOCK(p,hash);

	if( p->m_pFPHash[usEntry] == AsHASHRESERVE)
	{
		p->m_pFPHash[usEntry] = p->m_lFPHashIndexPtr;			
		//We should think about this case that don't override the data close to the center of current sync time
		//
		p->m_pFPHashIndex[2*p->m_lFPHashIndexPtr] =usDiff;
		p->m_pFPHashIndex[2*p->m_lFPHashIndexPtr+1] = 0;
	}
	else
	{//Link to first entry
		usEntry = p->m_pFPHash[usEntry];//First Entry
		while(	p->m_pFPHashIndex[ 2*usEntry+1 ] >0 )
		{
#if 0
			if( p->m_pFPHashIndex[ 2*usEntry ]	 ==  usDiff )
			{
				//PrintMsg(DEBUG_AVSYNC,("AVSYNC,Same Hash Value %x\r\n",lFingerprint));
			}
#endif
			usEntry = p->m_pFPHashIndex[ 2*usEntry+1 ];
		}
		p->m_pFPHashIndex[2*p->m_lFPHashIndexPtr] = usDiff;
		p->m_pFPHashIndex[2*p->m_lFPHashIndexPtr+1] = 0;

		//Link to previous entry
		p->m_pFPHashIndex[2*usEntry+1] =p->m_lFPHashIndexPtr ;
	}
	p->m_lFPHashIndexPtr++;
	if( p->m_lFPHashIndexPtr >= p->m_lFPHashIndexSize)
	{
		bBufferReady = 1;
	}

	AsUNLOCK(p,hash);

	return bBufferReady;
}

//
//
//AVSync
//
//

long AVSync_Init(
		HAVSYNC* pHandle,
		unsigned long lFrameRate,
		unsigned long lFrameRateScale,
		unsigned long lAudioSampleRate,
		unsigned long lAudioChannelNum,
		unsigned long lAudioBitsPerSample,
		unsigned long lMaxHoldingVideoFrameNum)
{
	AUDIOCOUNT lTemp;
	AVSync* p = (AVSync*)AsMALLOC(sizeof(AVSync));
	if( p == NULL)
		return 0;
	*pHandle = (HAVSYNC)p;

	AsINITLOCK(p,SyncCalling);
	AsINITLOCK(p,Common);
	
	lTemp =lAudioSampleRate*lAudioChannelNum*lAudioBitsPerSample;//Audio data rate by bit
	lTemp *=lFrameRateScale;
	p->m_lAudioWORDRateDividerByVFrame = 16*lFrameRate;
	p->m_lAudioWORDRateByVFrame = lTemp/p->m_lAudioWORDRateDividerByVFrame;//By WORD
	//Keep the residual for adjusting
	p->m_lAudioWORDRateResidualByVFrame = lTemp - (p->m_lAudioWORDRateByVFrame * p->m_lAudioWORDRateDividerByVFrame);

	p->m_lAudioSampleChannelBits = lAudioChannelNum*lAudioBitsPerSample;

	if( HashBarrel_Init(&p->m_hb) == 0)
	{
		AsFREE(p);
		return 0;
	}

	//(lAudioSampleRate*lAudioChannelNum*lAudioBitsPerSample) *lFrameRateScale  / lFrameRate

	p->m_lMaxVFramesNum = lMaxHoldingVideoFrameNum;
	p->m_pVFrames = (VideoFrameInfo*)AsMALLOC( sizeof(VideoFrameInfo)*lMaxHoldingVideoFrameNum );
	if( p->m_pVFrames == NULL)
	{
		HashBarrel_Uninit(&p->m_hb);
		AsFREE(p);
		return 0;
	}
	p->m_lVFramesNum =0;

	p->m_lAudioDataCount =0;
	p->m_lAudioBufferStartPostion =0;//Not required to init it

	p->m_ucSyncIntervalFrameNum = 15;
	p->m_ucSyncRadiusFrameNum =8;

	p->m_lAudioOffsetByWORD =0;

	//For firt sync
	AVSync_PrepareToNextSync(p,0,lMaxHoldingVideoFrameNum/2,AsSIZE32K);
	return 1;
}

void AVSync_Uninit(HAVSYNC h)
{
	AVSync* p = (AVSync*)h;
	HashBarrel_Uninit(&p->m_hb);
	AsFREE(p->m_pVFrames);
	AsFREE(p);
}

//
//If lAudioHashSize is 0, set to the real size match with the vip
//
int AVSync_PrepareToNextSync(AVSync* p,unsigned long lSyncStartVip,unsigned long lSyncEndVip,unsigned long lAudioHashSize)
{
	AUDIOCOUNT lTemp;
	AsLOCK(p,Common);
	
	p->m_bFPBufferReady = 0;

	p->m_bStartFillFPBuffer = 0;

	lTemp = AVSync_TranlatingCountFromVideoFrameToAudioSample(p,lSyncStartVip);
	p->m_lSyncStartPostion =(unsigned long)(lTemp+p->m_lAudioOffsetByWORD);

	p->m_lVFramesNum =0;

	p->m_lSyncStartVIP = lSyncStartVip;
	p->m_lSyncEndVIP = lSyncEndVip;

	AsUNLOCK(p,Common);
	if( lAudioHashSize == 0)
	{
		lTemp = lSyncEndVip-lSyncStartVip;
		lAudioHashSize = AVSync_TranlatingCountFromVideoFrameToAudioSample(p,lTemp);
	}

	HashBarrel_Reset(&p->m_hb,lAudioHashSize);
	PrintMsg(DEBUG_AVSYNC,("AVSYNC,PrepareToNextSync,AudioStart:%d,AudioHashSize:%d,VipStart:%d,VipEnd:%d\r\n",\
		p->m_lSyncStartPostion,lAudioHashSize,lSyncStartVip,lSyncEndVip));

	return 1;
}

long AVSync_ReceiveAudio(HAVSYNC h,unsigned short* pData,unsigned long lLen)
{
	AVSync* p = (AVSync*)h;

	unsigned long lStartPostion = p->m_lAudioDataCount;
	unsigned long lEndPostion = p->m_lAudioDataCount+lLen;
	unsigned long  ulStartSampleIndex =0;
	unsigned short usEntry =0;
	unsigned long i;
	long lOffetFrames =NOTFOUNDSYNC;

	//Update the count
	p->m_lAudioDataCount += lLen;
	

	//Buffer is ready, so we just return, the flag will be cleard at PrepareToNextSync
	//These data aren't in the area, just return
	AsLOCK(p,Common);
	if( p->m_bFPBufferReady || (lEndPostion < p->m_lSyncStartPostion)) 
	{
		AsUNLOCK(p,Common);
		return lOffetFrames;
	}

	//First Audio block
	if(! p->m_bStartFillFPBuffer )
	{
		p->m_ulCurFingerprint = 0;

		if( p->m_lSyncStartPostion>lStartPostion)
		{/*--------AUDIOBLOCKSTART-----------SYNCSTART----------------------AUDIOBLOCKEND------------------*/
			p->m_lAudioBufferStartPostion = p->m_lSyncStartPostion;
		}
		else
		{/*-----SYNCSTART---AUDIOBLOCKSTART---------------------------------AUDIOBLOCKEND------------------*/

			p->m_lAudioBufferStartPostion = lStartPostion;
		}
		//Skip first 31 bits;
		for(i=ulStartSampleIndex;i<ulStartSampleIndex+31;i++)
		{
			p->m_ulCurFingerprint <<=1;
			p->m_ulCurFingerprint |=(pData[i]&0x1);
		}
		p->m_bStartFillFPBuffer = 1;
		ulStartSampleIndex+=31;
	}

	for(i=ulStartSampleIndex;i<lLen;i++)
	{
		p->m_ulCurFingerprint <<=1;
		p->m_ulCurFingerprint |=(pData[i]&0x1);
		p->m_bFPBufferReady = HashBarrel_AddEntry(&p->m_hb,p->m_ulCurFingerprint);
		if( p->m_bFPBufferReady )
		{
			break;
		}
	}
	AsUNLOCK(p,Common);
	
	//At this time, the AVSync_PrepareToNextSync will be called, and the m_bFPBufferReady had been seted to 0
	//at this case, we should give up this Calling for AVSync_Sync, it's right.!!!!!
	if( p->m_bFPBufferReady )
	{
		PrintMsg(DEBUG_AVSYNC,("AVSYNC,DoSync From Audio stream\r\n"));
		lOffetFrames = AVSync_Sync(p,0);
	}

	return lOffetFrames;
}
long AVSync_ReceiveVideo(HAVSYNC h,unsigned long lVip,unsigned long lFingerprint,unsigned long lRefcount)
{
	AVSync* p = (AVSync*)h;
	long lOffetFrames =NOTFOUNDSYNC;
	int bClearBufferFlag =0;
	
	PrintMsg(DEBUG_AVSYNC,("AVSYNC,ReceiveVideo,VIP:%d,FP:%x,RC:%d,RC(HEX):%x\r\n",lVip,lFingerprint,lRefcount,lRefcount));
	AsLOCK(p,Common);
	if( lVip< p->m_lSyncStartVIP || p->m_lVFramesNum >= p->m_lMaxVFramesNum )
	{
		AsUNLOCK(p,Common);
		return lOffetFrames;
	}
	

	//Push into buffer
	p->m_pVFrames[p->m_lVFramesNum].m_lFingerprint = lFingerprint;
	p->m_pVFrames[p->m_lVFramesNum].m_lVip  = lVip;
	p->m_pVFrames[p->m_lVFramesNum].m_lRefcount  = lRefcount;
	p->m_lVFramesNum++;

	
	if( (lVip > (p->m_lSyncEndVIP+MAXHOLDINGMOREVIDEO) )&& p->m_bFPBufferReady)
	{
		bClearBufferFlag =1;//Give up this run, Clear all buffer for next run
	}
	AsUNLOCK(p,Common);

	lOffetFrames = AVSync_Sync(p,bClearBufferFlag);

	return lOffetFrames;
}

//
//AVSync_Sync
//return value points to the new total offset
//
long AVSync_Sync(AVSync* p,int bClearBuffer)
{
	unsigned long i;
	unsigned long lPostion;
	int bSyncOK = 0;
	unsigned long lVip;
	unsigned long lAudioCount;
	AUDIOCOUNT lTemp;
	AUDIOCOUNT lTemp1;

	long lOffetFrames =NOTFOUNDSYNC;


	AsLOCK(p,SyncCalling);

	
	for( i=0;i<p->m_lVFramesNum;i++)
	{
		if( p->m_pVFrames[i].m_lFingerprint !=0 )//For CASE:The duplicated video frame will put 0 as fingerprint
		{
			if( HashBarrel_Lookup(&p->m_hb,p->m_pVFrames[i].m_lFingerprint,&lPostion ) )
			{
				bSyncOK = 1;
				break;
			}
		}
	}
	
	if( bSyncOK)
	{//We already got syncing
		lAudioCount = lPostion+p->m_lAudioBufferStartPostion;
		//Calc the difference with video frame
		lTemp1 = AVSync_TranlatingCountFromVideoFrameToAudioSample(p,p->m_pVFrames[i].m_lVip);
		p->m_lAudioOffsetByWORD =lAudioCount - lTemp1;

		lOffetFrames = (long)(p->m_lAudioOffsetByWORD/p->m_lAudioWORDRateByVFrame);//need adjusting.

		if( lOffetFrames>1000 || lOffetFrames<-1000)
			lOffetFrames =NOTFOUNDSYNC;

		lTemp =p->m_pVFrames[i].m_lRefcount;
		lTemp =lTemp*p->m_lAudioSampleChannelBits/16;
		PrintMsg(DEBUG_AVSYNC,("<AVSYNC>***SYNC*****AudioCount:%d,RefSampleCount:%d,VIP:%d,"
			"DiffWithRef:%d,DiffWithVideo:%d,FrameOffset:%d"
			"AudioWORDRateByVFrame:%d",
			lAudioCount,p->m_pVFrames[i].m_lRefcount,
			p->m_pVFrames[i].m_lVip,
			lAudioCount - lTemp,
			p->m_lAudioOffsetByWORD,lOffetFrames,
			p->m_lAudioWORDRateByVFrame
			));

		if( p->m_ucSyncRadiusFrameNum>MINSYNCRADIUSVIDEOFRAMENUM)
			p->m_ucSyncRadiusFrameNum--;	

		lVip = p->m_pVFrames[i].m_lVip;
	}
	else if( bClearBuffer)
	{//Lost sync
		lVip =  p->m_pVFrames[p->m_lVFramesNum-1].m_lVip;//Last VIP
		lAudioCount = p->m_lAudioDataCount;
		if( p->m_ucSyncRadiusFrameNum< p->m_ucSyncIntervalFrameNum)
			p->m_ucSyncRadiusFrameNum++;	
		PrintMsg(DEBUG_AVSYNC,("<AVSYNC>****LOSTSYNC******** AudioCount:%d",lAudioCount));

	}
	AsUNLOCK(p,SyncCalling);

	if( bSyncOK || bClearBuffer )
	{
		AVSync_PrepareToNextSync(p,
			lVip+p->m_ucSyncIntervalFrameNum-p->m_ucSyncRadiusFrameNum,
			lVip+p->m_ucSyncIntervalFrameNum+p->m_ucSyncRadiusFrameNum,
			0);
	}
	return lOffetFrames;
}

/********************************* end of AVSync.c ***************************/
