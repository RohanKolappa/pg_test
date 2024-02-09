// AudioStreamBuffer.h

#ifndef __AUDIO_STREAMBUFFER_H__
#define __AUDIO_STREAMBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MALLOC(size) PIAllocMem(size)
#define FREE(p) PIFreeMem(p)

#if ( defined(_WINDOWS) && defined(DRIVER) )
	#include <wdm.h>
#elif defined (_LINUX)
	#include <asm/system.h>
	#include <linux/spinlock.h>
	#include <linux/time.h>
#endif

enum
{
	AUDIO_DROPFRAME_EVENT,
	AUDIO_NEWFRAME_EVENT
};

typedef void (*AUDIOSTREAMBUFFERCALLBACK)(int event, int param, void* context);

typedef struct tagCAudioStreamBuffer
{
	unsigned long m_lBufferLenMask;
	unsigned long m_lFreeAreaHeadPtr;
	unsigned long m_lDataAreaHeadPtr;
	unsigned long m_lDroppedBlockCnt;
	unsigned long m_lDuplicatedBlockCnt;
	unsigned long long m_lTotalBlockCnt;
	unsigned char* m_pBuffer;
	//For Callback
	AUDIOSTREAMBUFFERCALLBACK m_pCB;
	void* m_pCBContext;
	//

#if ( defined(_WINDOWS) && defined(DRIVER) )
	KSPIN_LOCK m_lock;
	KIRQL m_saveLock;
#else
	spinlock_t spinlock; 
	unsigned long lock_flags;
#endif

	struct timeval* audio_timestamp;
	unsigned int audio_block_size;
}CAudioStreamBuffer;


//Method:

CAudioStreamBuffer* CAudioStreamBuffer_Init(unsigned char cBufferLenBit);
void CAudioStreamBuffer_Uninit(CAudioStreamBuffer* pBM);
void CAudioStreamBuffer_Reset(CAudioStreamBuffer* pBM);
void CAudioStreamBuffer_SetCallback(CAudioStreamBuffer* pBM,AUDIOSTREAMBUFFERCALLBACK pCB,void* context);


unsigned long CAudioStreamBuffer_Alloc(CAudioStreamBuffer* pBM,unsigned char** ppBuffer, unsigned long lSize);
int CAudioStreamBuffer_Commit(CAudioStreamBuffer* pBM,unsigned char* pBuffer, unsigned long lSize);
int CAudioStreamBuffer_Release(CAudioStreamBuffer* pBM,unsigned char* pBuffer, unsigned long lSize);


unsigned long CAudioStreamBuffer_GetData(CAudioStreamBuffer* pBM,unsigned char* pBuffer,unsigned long lSize,int Delay);
void CAudioStreamBuffer_GetStatistics(CAudioStreamBuffer* pBM,unsigned long* droppedBlockCnt,unsigned long* duplicatedBlockCnt, unsigned long long* totalBlockCnt);

#ifdef __cplusplus
}
#endif

#endif
