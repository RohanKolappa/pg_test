// AudioStreamBuffer.c

#include "AudioStreamBuffer.h"
#include "platform.h"
#include "common.h"

#define AUDIOBLOCKSIZEMIN 512		/*the minimal size of audioblocksize */

#if ( defined(_WINDOWS) && defined(DRIVER) )

	#define INITLOCK(pBM) \
  		KeInitializeSpinLock(&(pBM->m_lock))

	#define LOCKBUFFER(pBM) \
		KeAcquireSpinLock(&(pBM->m_lock), &(pBM->m_saveLock))

	#define UNLOCKBUFFER(pBM) \
		KeReleaseSpinLock(&(pBM->m_lock), pBM->m_saveLock)

	#define PICopyToUser PICopyMem

#elif defined (_LINUX)

	#define INITLOCK(pBM) \
		spin_lock_init(&(pBM->spinlock))

	#define LOCKBUFFER(pBM) \
		spin_lock_irqsave(&(pBM->spinlock), pBM->lock_flags)

	#define UNLOCKBUFFER(pBM) \
		spin_unlock_irqrestore(&(pBM->spinlock), pBM->lock_flags)

	#include <asm/uaccess.h>

	#define PICopyToUser copy_to_user

#endif

CAudioStreamBuffer* CAudioStreamBuffer_Init(unsigned char cBufferLenBit)
{
	CAudioStreamBuffer* pBM;
	
	pBM = (CAudioStreamBuffer*)MALLOC(sizeof(CAudioStreamBuffer));
	if(pBM==NULL)
		return NULL;

	pBM->m_lBufferLenMask = (0x1<<cBufferLenBit);
	pBM->m_pBuffer =(unsigned char*)PIAllocVirtualMem(pBM->m_lBufferLenMask);
	if( pBM->m_pBuffer == NULL)
	{
		FREE(pBM);	
		return NULL;
	}
	/*add 05-2-19 for timestamp*/
	pBM->audio_timestamp = 
		(struct timeval*)MALLOC(sizeof(struct timeval)*pBM->m_lBufferLenMask/AUDIOBLOCKSIZEMIN); 
	if(pBM->audio_timestamp==NULL)
	{
		PIFreeVirtualMem(pBM->m_pBuffer);
		FREE(pBM);	
		return NULL;
	}
	/*add 05-2-19 end*/

	INITLOCK(pBM);

	pBM->m_lBufferLenMask -=1;
	pBM->m_lFreeAreaHeadPtr =0;
	pBM->m_lDataAreaHeadPtr =0;
	pBM->m_lDroppedBlockCnt = 0;
	pBM->m_lDuplicatedBlockCnt = 0;
	pBM->m_lTotalBlockCnt = 0;
	pBM->m_pCB = 0;
	pBM->m_pCBContext =0;

	return pBM;
}

void CAudioStreamBuffer_Uninit(CAudioStreamBuffer* pBM)
{
	PIFreeVirtualMem(pBM->m_pBuffer);
	FREE(pBM->audio_timestamp);
	FREE(pBM);	
}
void CAudioStreamBuffer_Reset(CAudioStreamBuffer* pBM)
{
	LOCKBUFFER(pBM);
	pBM->m_lFreeAreaHeadPtr =0;
	pBM->m_lDataAreaHeadPtr =0;	
	pBM->m_lDroppedBlockCnt = 0;
	pBM->m_lDuplicatedBlockCnt = 0;
	pBM->m_lTotalBlockCnt = 0;
	UNLOCKBUFFER(pBM);
}
void CAudioStreamBuffer_GetStatistics(CAudioStreamBuffer* pBM, unsigned long* droppedBlockCnt, unsigned long* duplicatedBlockCnt, unsigned long long* totalBlockCnt)
{
	if( droppedBlockCnt ) {
		*droppedBlockCnt = pBM->m_lDroppedBlockCnt;
	}
	if( duplicatedBlockCnt ) {
		*duplicatedBlockCnt = pBM->m_lDuplicatedBlockCnt;
	}
	if( totalBlockCnt ) {
		*totalBlockCnt = pBM->m_lTotalBlockCnt;
	}
}

unsigned long CAudioStreamBuffer_Alloc(CAudioStreamBuffer* pBM,unsigned char** ppBuffer, unsigned long lSize)
{
	unsigned long lLeftSize =0;

	LOCKBUFFER(pBM);

	lLeftSize = (pBM->m_lDataAreaHeadPtr>pBM->m_lFreeAreaHeadPtr) ?
		(pBM->m_lDataAreaHeadPtr-pBM->m_lFreeAreaHeadPtr): 
	(pBM->m_lBufferLenMask+1-(pBM->m_lFreeAreaHeadPtr-pBM->m_lDataAreaHeadPtr) );
	if( lLeftSize<lSize )
	{
		PrintMsg(DEBUG_AUDIO_STREAMBUFFER,("<DRV> AUDIOBUFFER,Overflow,left size:%d %x\r\n",(int)lLeftSize,(int)lSize));
		//Simple logic to free all buffers
		//		pBM->m_lFreeAreaHeadPtr =0;
		//		pBM->m_lDataAreaHeadPtr =0;	
		// Free only as much as needed - similar to video buffer handling
		pBM->m_lDataAreaHeadPtr = (pBM->m_lDataAreaHeadPtr+lSize-lLeftSize)&pBM->m_lBufferLenMask;
          	if (pBM->audio_block_size == 0)
		        pBM->m_lDroppedBlockCnt += (lSize - lLeftSize) / 1024;
		else
		        pBM->m_lDroppedBlockCnt += (lSize - lLeftSize) / pBM->audio_block_size;
	}

	*ppBuffer = &pBM->m_pBuffer[pBM->m_lFreeAreaHeadPtr];

	UNLOCKBUFFER(pBM);

	if( lLeftSize<lSize )
	{
		if( pBM->m_pCB)
		{
			(*pBM->m_pCB)(AUDIO_DROPFRAME_EVENT,pBM->m_lBufferLenMask+1-lLeftSize,pBM->m_pCBContext);
		}
	}

	return lSize;
}
int CAudioStreamBuffer_Commit(CAudioStreamBuffer* pBM,unsigned char* pBuffer, unsigned long lSize)
{
	UINT32 serial_number;
	LOCKBUFFER(pBM);
	/*add 05-2-19 for timestamp*/	
	pBM->audio_block_size = lSize;		
	if (pBM->audio_block_size == 0)
		serial_number = pBM->m_lFreeAreaHeadPtr/1024;
	else
		serial_number = pBM->m_lFreeAreaHeadPtr/pBM->audio_block_size;
	do_gettimeofday(&(pBM->audio_timestamp[serial_number]));
	/*add 05-2-19 end*/	
	pBM->m_lFreeAreaHeadPtr = (pBM->m_lFreeAreaHeadPtr+lSize)&pBM->m_lBufferLenMask;
	pBM->m_lTotalBlockCnt++;
	UNLOCKBUFFER(pBM);

	if( pBM->m_pCB)
	{
		(*pBM->m_pCB)(AUDIO_NEWFRAME_EVENT,0,pBM->m_pCBContext);
	}

	return 1;
}

int CAudioStreamBuffer_Release(CAudioStreamBuffer* pBM,unsigned char* pBuffer, unsigned long lSize)
{
	LOCKBUFFER(pBM);
	pBM->m_lDataAreaHeadPtr = ((pBuffer-pBM->m_pBuffer)+lSize+1)&pBM->m_lBufferLenMask;
	UNLOCKBUFFER(pBM);
	return 1;
}
unsigned long CAudioStreamBuffer_GetData(CAudioStreamBuffer* pBM,
					 unsigned char* pBuffer,
					 unsigned long lSize,int Delay)
{
	unsigned long lSize1;
	UINT32 serial_number;

	LOCKBUFFER(pBM);

	if( pBM->m_lDataAreaHeadPtr<=pBM->m_lFreeAreaHeadPtr )
	{
		if( pBM->m_lFreeAreaHeadPtr - pBM->m_lDataAreaHeadPtr <lSize + Delay)
			lSize =  0;
		else
			PICopyToUser(pBuffer,&pBM->m_pBuffer[pBM->m_lDataAreaHeadPtr],lSize);
	}
	else
	{
		if( pBM->m_lBufferLenMask+1-(pBM->m_lDataAreaHeadPtr-pBM->m_lFreeAreaHeadPtr) < lSize + Delay )
			lSize =  0;
		else
		{
			lSize1 = pBM->m_lBufferLenMask-pBM->m_lDataAreaHeadPtr+1;
			if( lSize1>=lSize )
				PICopyToUser(pBuffer,&pBM->m_pBuffer[pBM->m_lDataAreaHeadPtr],lSize);		
			else
			{
				PICopyToUser(pBuffer,&pBM->m_pBuffer[pBM->m_lDataAreaHeadPtr],lSize1);		
				PICopyToUser(pBuffer+lSize1,&pBM->m_pBuffer[0],lSize-lSize1);		
			}
		}
	}
	if( lSize >0)
		/*modify 05-2-19 for timestamp*/		
	{
		if (pBM->audio_block_size == 0)
			serial_number = pBM->m_lDataAreaHeadPtr/1024;
		else
			serial_number = pBM->m_lDataAreaHeadPtr/pBM->audio_block_size;
		//SJ - no locking change for now                UNLOCKBUFFER(pBM);
		PICopyToUser(pBuffer+lSize,&(pBM->audio_timestamp[serial_number]),sizeof(struct timeval));	
		//                LOCKBUFFER(pBM);
		// Reuse head data till dropped blocks have been made up for
		if( pBM->m_lDroppedBlockCnt > pBM->m_lDuplicatedBlockCnt ) {
		  if (pBM->audio_block_size == 0)
		    pBM->m_lDuplicatedBlockCnt += lSize / 1024;
		  else
		    pBM->m_lDuplicatedBlockCnt += lSize / pBM->audio_block_size;
		}
		else {
		  pBM->m_lDataAreaHeadPtr = (pBM->m_lDataAreaHeadPtr+lSize)&pBM->m_lBufferLenMask;
		}
		lSize += sizeof(struct timeval);
	}
		/*add 05-2-19 end*/
	UNLOCKBUFFER(pBM);
	return lSize;
}

void CAudioStreamBuffer_SetCallback(CAudioStreamBuffer* pBM,AUDIOSTREAMBUFFERCALLBACK pCB,void* context)
{
	pBM->m_pCB = pCB;
	pBM->m_pCBContext = context;
}
