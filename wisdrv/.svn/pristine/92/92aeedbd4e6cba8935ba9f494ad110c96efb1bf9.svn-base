/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        StreamBuffer.h
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

#ifndef __STREAMBUFFER_H__
#define __STREAMBUFFER_H__

#include "typedef.h"
#include "Struct.h"
#include <linux/time.h>
#if defined(_WINDOWS)

	#ifdef DRIVER

		#include <wdm.h>

	#else

		#include <windows.h>
		#include <string.h>

		#define ULONG unsigned long
		#define KSPIN_LOCK ULONG
		#define KIRQL ULONG
		#define NTSTATUS ULONG
		#define STATUS_SUCCESS 0
		#define STATUS_UNSUCCESSFUL 1
		#define NTSUCCESS(status) (status==STATUS_SUCCESS)
		#define KeInitializeSpinLock(p)
		#define LOCKBUFFER(psb)
		#define UNLOCKBUFFER(psb) 

		#define STREAMBUFFER_ALLOC(size) malloc(size)
		#define STREAMBUFFER_FREE(bfr) free(bfr)
		#define PICmpMem(str1, str2, length) ( memcmp(str1, str2, length) == 0 )

	#endif	// DRIVER

#elif defined (_LINUX)

	#include <asm/system.h>
	#include <linux/spinlock.h>

	#define STATUS_BUFFER_TOO_SMALL		(0xC0000023L)

#endif  // _WINDOWS

#if defined(_WINDOWS)
#define MAX_CHANNEL 		4
#elif defined (_LINUX)
//[pma] for Linux app,we only need 1 channel per device.
#define MAX_CHANNEL 		1
#endif

#define CHANNEL_ID			UINT32
#define STREAM_BUFFER_SIZE	4096000
#define MAX_SYNCWORD_LENGTH 0x10
#define VBI_PRESENT				0x000001
#define FINGERPRINT_PRESENT		0x000002
#define MAP_PRESENT				0x000004

//#define MAP_BUFFER_LENGTH		172
/* MD_OUTPUT_MAP size = 102 WORDs
 * MD_REGION_MAP size =   2 WORDs (WORD1 & WORD2 are duplicated each other)
 * total 104 WORDs = 208 bytes
 */
#define MAP_BUFFER_LENGTH		208

#ifdef __cplusplus
extern "C" {
#endif
#define SEQ_START     0
#define SEQ_MIDDLE    1
#define SEQ_END       2
enum OPERATION_STATE { 
	STATE_NORMAL, 
	STATE_SKIP_VBI, 
	STATE_PARSE_VBI, 
	STATE_PARSE_TIMESTAMP,
	STATE_PARSE_MAP,
};

enum CHANNEL_EVENT {
	NEWFRAME_EVENT,
	DROPFRAME_EVENT,
	DUPFRAME_EVENT
};

typedef void (* STREAMBUFFER_CALLBACKFUNC)(CHANNEL_ID channel_id, enum CHANNEL_EVENT event, void* context);

typedef struct tagSYNCWORD	//syncword information
{
	unsigned char _syncword[MAX_SYNCWORD_LENGTH];	// syncword
	unsigned char _mask[MAX_SYNCWORD_LENGTH];		// for syncword size that is not BYTE oriented
	SINT32	_resync_index[MAX_SYNCWORD_LENGTH];		// for resync after mismatch
	UINT32	_length;								// length of the syncword in byte
	UINT32	_matched;								// how many byte has been mathced
	UINT32	_present;								// if the syncword is valid in current stream
} SYNCWORD, *PSYNCWORD;

typedef struct tagFRAME_ITEM
{
	UINT32	_frame_start;
	UINT32	_frame_length;
	UINT32	_frame_type;
	struct	tagFRAME_ITEM* _next;
	struct	tagFRAME_ITEM* _prev;
	UINT32	_finger_print;
	UINT32	_vip_number;
	UINT32	_frame_serial;
	UINT32	_timestamp;
	UINT32	_vbi_start;
	UINT32	_vbi_length;
	UINT32	_map_start;
	UINT32	_frame_header_founded;
	UINT32  _flag;
	UINT32  _reference_count;
	UINT16  _frame_sequence_position;  /* 0: first frame in sequence, 1:in the middle of sequence,
								 2: last frame of sequence*/
	UINT32	_duplicate_frame;	// whether it is a duplicated frame
	struct timeval  _video_timestamp;
} FRAME_ITEM, *PFRAME_ITEM;

typedef struct tagSTREAM_CHANNEL
{
	BOOLEAN		_valid;
	UINT32		_frame_count;
	PFRAME_ITEM _head_frame;
	PFRAME_ITEM _tail_frame;

	STREAMBUFFER_CALLBACKFUNC _callback;	// callback function when new frame arrived
	PVOID	_context;						// context informaiton passed to callback function

} STREAM_CHANNEL, *PSTREAM_CHANNEL;

typedef struct tagSTREAM_BUFFER
{
	UINT32		_buffer_size;
	PFRAME_ITEM _head_frame;
	PFRAME_ITEM _tail_frame;
	UINT32		_frame_count;
	UINT32		_frame_serial;
	UINT32		_next_frame_type;		// 0: I frame; 1 : P frame; 2 : B frame;
	UINT32		_next_vip_number;		

	unsigned char * _bfr;

	UINT32 _read_byte_index;
	UINT32 _write_byte_index;

#if defined(_WINDOWS)
	KSPIN_LOCK _spin_lock; 
	KIRQL _old_irql;
#elif defined (_LINUX)
	spinlock_t spinlock; 
	unsigned long lock_flags;
#endif

	EVideoFormat _stream_format;
	ESequenceMode _sequence_mode;
	UINT32		_interlace_coding;

	SYNCWORD _seq_header;
	SYNCWORD _gop_header;
	SYNCWORD _frm_header;
	SYNCWORD _vbi_header;
	SYNCWORD _ts_header;
	SYNCWORD _map_header;

	UINT32 _first_syncword_founded;
	UINT32 _sync_word_length;

	UINT32 _parse_count;		// used both in skip mode and parse mode

	enum OPERATION_STATE _state;

	// statistics information
	UINT32		_total_frame_dropped;
	UINT32		_total_frame_duplicated;
	UINT64		_total_bytes_accquired;

	STREAM_CHANNEL _channels[MAX_CHANNEL];
	UINT32 _channel_count;

	UINT32 _matching_in_process;
	struct timeval _current_time;
} STREAM_BUFFER, *PSTREAM_BUFFER;

typedef struct tagFRAME_INFO
{
	UINT32 _max_frame_length;		// caller fill this
	UINT8  *_frame_buffer;			// caller allocate this
	UINT32 _actual_frame_length;	// callee fill this

	UINT32 _max_vbi_length;			// caller fill this
	UINT8 *_vbi_buffer;				// caller allocate this
	UINT32 _actual_vbi_length;		// callee fill this

	UINT8 *_map_buffer;				// caller allocate this

	UINT32 _finger_print;			// callee fill this
	UINT32 _vip_number;				// callee fill this
	UINT32 _frame_serial;			// callee fill this
	UINT32 _duplicate_frame;		// callee fill this
	UINT32 _frame_pts;				// callee fill this
	UINT32 _frame_type;				// callee fill this	
	UINT32 _timestamp;				// callee fill this
	UINT32 _flag;					// callee fill this
	UINT32  _frame_sequence_position;  /* 0: first frame in sequence, 1:in the middle of sequence,
                                                                 2: last frame of sequence*/
	struct timeval _video_timestamp;

} FRAME_INFO, *PFRAME_INFO;

#define GET_BIG_ENDIAN_ULONG(p) \
	( ( (UINT32)(p[0]) << 24 ) + ( (UINT32)(p[1]) << 16 ) + ( (UINT32)(p[2]) << 8 ) + ( (UINT32)(p[3]) ) )

NTSTATUS StreamBuffer_Init(PSTREAM_BUFFER psb, 
						   UINT32 buffer_size, 
						   TCFGVIDEOEX* video_config,
						   UINT32 flags);
NTSTATUS StreamBuffer_Uninit(PSTREAM_BUFFER psb);
NTSTATUS StreamBuffer_Reset(PSTREAM_BUFFER psb);

int StreamBuffer_AddBlock(PSTREAM_BUFFER psb, unsigned char* bfr, UINT32 length);

UINT32 StreamBuffer_GetFrameCount(PSTREAM_BUFFER psb);

NTSTATUS StreamBuffer_OpenChannel(PSTREAM_BUFFER psb, CHANNEL_ID* channel_id);
NTSTATUS StreamBuffer_CloseChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id);

NTSTATUS StreamBuffer_SetChannelCallBack(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, STREAMBUFFER_CALLBACKFUNC fp, void* context);
UINT32 StreamBuffer_GetChannelFrameCount(PSTREAM_BUFFER psb, CHANNEL_ID channel_id);
NTSTATUS StreamBuffer_GetChannelFrame(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, PFRAME_INFO frmInfo);

INLINE VOID StreamBuffer_Lock(PSTREAM_BUFFER psb);
INLINE VOID StreamBuffer_Unlock(PSTREAM_BUFFER psb);

// you need hold the lock of the stream buffer while you access the returned frame pointers
PFRAME_ITEM StreamBuffer_GetFirstFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id);
PFRAME_ITEM StreamBuffer_GetLastFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id);
PFRAME_ITEM StreamBuffer_GetNextFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, PFRAME_ITEM pFrame);

// statistics
VOID StreamBuffer_GetStatistics(PSTREAM_BUFFER psb, UINT32* total_acquired, UINT32* total_dropped, UINT32* total_duplicated, UINT64* total_bytes_acquired);

#ifdef __cplusplus
}
#endif

/*


			     Index (link table)

			  +---------+
_head_frame-->|			| ----------+
			  |---------|			|
			  |			| 			|
		      |---------|			|
		      |    .	|			|
		      |    .	|			|
		      |    .	|			|
		      |---------|			|
_tail_frame-->|			| ----------+---------------------+
		      +---------+			|					  |
		      			 			|_read_byte_index	  |		   _write_byte_index
		                 			|					  |		  |
									V					  v		  v
							   +-+-+-+-+-+-+-+-+----------------------------+
				  _bfr (cycle) | | | | | | | | |          ......  	        |
							   +-+-+-+-+-+-+-+-+----------------------------+
				  

*/

#endif


/****************************** end of StreamBuffer.h ************************/

