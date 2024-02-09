/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        StreamBuffer.c
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

#include "platform.h"
#include "StreamBuffer.h"
#include "function.h"

#if defined(_WINDOWS)

	#define INITLOCK(psb) \
  		KeInitializeSpinLock(&(psb->_spin_lock))

	#define LOCKBUFFER(psb) \
		KeAcquireSpinLock(&(psb->_spin_lock), &(psb->_old_irql))

	#define UNLOCKBUFFER(psb) \
		KeReleaseSpinLock(&(psb->_spin_lock), psb->_old_irql)

	#define PICopyToUser PICopyMem

#elif defined(_LINUX)

	#define FALSE 0
	#define TRUE  1

	#define INITLOCK(psb) \
		spin_lock_init(&(psb->spinlock))

	#define LOCKBUFFER(psb) \
		spin_lock_irqsave(&(psb->spinlock), psb->lock_flags)

	#define UNLOCKBUFFER(psb) \
		spin_unlock_irqrestore(&(psb->spinlock), psb->lock_flags)

	#include <asm/uaccess.h>

	#define PICopyToUser copy_to_user

	#define ASSERT(x)

#endif

/////////////////////////////////////////////////////////////
// private function
/////////////////////////////////////////////////////////////

__inline UINT32 StreamBuffer_ByteAvailable(PSTREAM_BUFFER psb)
{
	UINT32 ByteAvailable;
	if ( psb->_read_byte_index == psb->_write_byte_index )
		ByteAvailable = psb->_buffer_size;
	else
		ByteAvailable = ( psb->_read_byte_index + psb->_buffer_size - psb->_write_byte_index ) % ( psb->_buffer_size );
	return ByteAvailable - 1;	// _read_byte_index will only equal to _write_byte_index whenever the buffer is empty.
}

void StreamBuffer_PushChannelFrame(PSTREAM_BUFFER psb, PFRAME_ITEM frame, CHANNEL_ID channel_id)
{
    DEBUG_FUNC_ENTER("StreamBuffer_PushChannelFrame")

	psb->_channels[channel_id]._frame_count ++;
	psb->_channels[channel_id]._tail_frame = frame;
	frame->_reference_count ++;
    DEBUG_FUNC_EXIT("StreamBuffer_PushChannelFrame")
}

PFRAME_ITEM StreamBuffer_PopFrame(PSTREAM_BUFFER psb)
{
	PFRAME_ITEM head = psb->_head_frame;

	if ( psb->_head_frame == psb->_tail_frame ) return NULL;
	
	psb->_head_frame = psb->_head_frame->_next;
	psb->_head_frame->_prev = NULL;
	psb->_read_byte_index = psb->_head_frame->_frame_start;
	psb->_frame_count --;

	return head;
}
void set_seq_end(PSTREAM_BUFFER psb)
{

	psb->_tail_frame->_frame_sequence_position=SEQ_END;


}
void StreamBuffer_PushFrame(PSTREAM_BUFFER psb, PFRAME_ITEM frame, UINT16 seq)
{
	ULONG i;

	psb->_frame_count ++;
	psb->_total_bytes_accquired += psb->_tail_frame->_frame_length;

/*	// detect duplication frame here
	if ( psb->_tail_frame->_vip_number == 0 && psb->_tail_frame->_frame_type == 0 && psb->_frame_serial != 0 )
	{
		psb->_tail_frame->_frame_type = 1; // firmware bug: they think it is I, but it is P.
		psb->_tail_frame->_duplicate_frame = 1;
//		psb->_total_frame_duplicated ++;
	}
*/
	// append the frame at the tail of the queue
	frame->_prev = psb->_tail_frame;
	psb->_tail_frame->_next = frame;
	psb->_tail_frame = frame;
	frame->_next = NULL;

	frame->_frame_length = 0;
	frame->_frame_type = 0;
	frame->_finger_print = 0;
	frame->_vip_number = 0;
	frame->_timestamp = 0;
	frame->_frame_header_founded = 0;
	frame->_vbi_length = 0;
	frame->_vbi_start = 0;
	frame->_flag = 0;
	frame->_reference_count = 0;
	frame->_duplicate_frame = 0;
	frame->_frame_serial = psb->_frame_serial ++;
	frame->_frame_sequence_position=seq;
	/*add 05-2-19 for timestamp*/
	frame->_video_timestamp = psb->_current_time;
	/*add 05-2-19 end*/	
//printk(" streambuffer putting: seq=%d\n",frame->_frame_sequence_position);
/*printk(" push frame seq=%d\n",frame->_frame_sequence_position);*/
	for ( i = 0 ; i < MAX_CHANNEL ; i ++ )
	{
		if ( psb->_channels[i]._valid == FALSE ) continue;
		StreamBuffer_PushChannelFrame(psb, frame, i);
	}
}

// Same as PopChannelFrame except it does not remove the head frame
// Used when we want to reuse the head frmae to make up for previous frame drops
PFRAME_ITEM StreamBuffer_PeekChannelFrame(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	PFRAME_ITEM head = psb->_channels[channel_id]._head_frame;
	PFRAME_ITEM tail = psb->_channels[channel_id]._tail_frame;
	if ( head == tail ) return NULL;

	return head;
}
PFRAME_ITEM StreamBuffer_PopChannelFrame(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	PFRAME_ITEM head = psb->_channels[channel_id]._head_frame;
	PFRAME_ITEM tail = psb->_channels[channel_id]._tail_frame;
	if ( head == tail ) return NULL;

	psb->_channels[channel_id]._head_frame = head->_next;
	psb->_channels[channel_id]._frame_count --;

	if ( -- head->_reference_count == 0 )
		StreamBuffer_PopFrame(psb);

	return head;
}

PFRAME_ITEM StreamBuffer_DropFrame(PSTREAM_BUFFER psb)
{
	ULONG i;
	PFRAME_ITEM head = psb->_head_frame;

	psb->_total_frame_dropped ++;
	
	for ( i = 0 ; i < MAX_CHANNEL ; i ++ )
	{
		if ( psb->_channels[i]._valid == FALSE ) continue;
		if ( psb->_channels[i]._head_frame == head )
		{
			if ( psb->_channels[i]._callback )
				(*psb->_channels[i]._callback)(i, DROPFRAME_EVENT, psb->_channels[i]._context);
			StreamBuffer_PopChannelFrame(psb, i);
		}
	}

	return head;
}

void StreamBuffer_ComputeResyncIndex(PSYNCWORD syncword)
{
	UINT32 n;
	
	if ( syncword->_present == 0 ) return;

	// resync index is defined as follows:
	// after matched n bytes, we got a mismatch, what should matched bytes in this case
	syncword->_resync_index[0] = 0;
	for ( n = 1; n < syncword->_length ; n ++ )
	{
		UINT32 i = n - 1;
		while ( i > 0 )
		{
			// if the first i bytes of syncword[0..n-1] is equal to the last i bytes of syncword[0..n-1]
			if ( PICmpMem(syncword->_syncword, syncword->_syncword + n - i, i) )
				break;
			i --;
		}
		syncword->_resync_index[n] = i;
	}
}

void StreamBuffer_InitSyncWord(PSTREAM_BUFFER psb, TCFGVIDEOEX* video_config, UINT32 flags)
{
	PSYNCWORD seq = &psb->_seq_header;
	PSYNCWORD gop = &psb->_gop_header;
	PSYNCWORD frm = &psb->_frm_header;
	PSYNCWORD vbi = &psb->_vbi_header;
	PSYNCWORD ts = &psb->_ts_header;
	PSYNCWORD map = &psb->_map_header;

	switch ( psb->_stream_format ) // merge all go stream format to "GO"
	{
		case MPEG1:
		case MPEG2:
			seq->_length = 4;
			seq->_mask[0] = seq->_mask[1] = seq->_mask[2] = seq->_mask[3] = 0xFF;
			seq->_present = 1;
			seq->_syncword[0] = 0;
			seq->_syncword[1] = 0;
			seq->_syncword[2] = 1;
			seq->_syncword[3] = 0xB3;

			gop->_length = 4;
			gop->_mask[0] = gop->_mask[1] = gop->_mask[2] = gop->_mask[3] = 0xFF;
			gop->_present = 1;
			gop->_syncword[0] = 0;
			gop->_syncword[1] = 0;
			gop->_syncword[2] = 1;
			gop->_syncword[3] = 0xB8;

			frm->_length = 4;
			frm->_mask[0] = frm->_mask[1] = frm->_mask[2] = frm->_mask[3] = 0xFF;
			frm->_present = 1;
			frm->_syncword[0] = 0;
			frm->_syncword[1] = 0;
			frm->_syncword[2] = 1;
			frm->_syncword[3] = 0;
			break;

		case MPEG4:

			seq->_length = 4;
			seq->_mask[0] = seq->_mask[1] = seq->_mask[2] = seq->_mask[3] = 0xFF;
			seq->_present = 1;
			seq->_syncword[0] = 0;
			seq->_syncword[1] = 0;
			seq->_syncword[2] = 1;
			seq->_syncword[3] = 0xB0; 

			gop->_length = 4;
			gop->_mask[0] = gop->_mask[1] = gop->_mask[2] = gop->_mask[3] = 0xFF;
			gop->_present = 1;
			gop->_syncword[0] = 0;
			gop->_syncword[1] = 0;
			gop->_syncword[2] = 1;
			gop->_syncword[3] = 0xB3;

			frm->_length = 4;
			frm->_mask[0] = frm->_mask[1] = frm->_mask[2] = frm->_mask[3] = 0xFF;
			frm->_present = 1;
			frm->_syncword[0] = 0;
			frm->_syncword[1] = 0;
			frm->_syncword[2] = 1;
			frm->_syncword[3] = 0xB6;

			break;

		case GO:

			seq->_length = 4;
			seq->_mask[0] = seq->_mask[1] = seq->_mask[2] = seq->_mask[3] = 0xFF;
			seq->_present = 1;
			seq->_syncword[0] = 0;
			seq->_syncword[1] = 0;
			seq->_syncword[2] = 1;
			seq->_syncword[3] = 0xF1;

			gop->_length = 4;
			gop->_mask[0] = gop->_mask[1] = gop->_mask[2] = gop->_mask[3] = 0xFF;
			gop->_present = 0;

			frm->_length = 4;
			frm->_mask[0] = frm->_mask[1] = frm->_mask[2] = frm->_mask[3] = 0xFF;
			frm->_present = 1;
			frm->_syncword[0] = 0;
			frm->_syncword[1] = 0;
			frm->_syncword[2] = 1;
			frm->_syncword[3] = 0xF0;

			break;

		case H263:
			seq->_length = 4;
			seq->_mask[0] = seq->_mask[1] = seq->_mask[2] = seq->_mask[3] = 0xFF;
			seq->_present = 0;

			gop->_length = 4;
			gop->_mask[0] = gop->_mask[1] = gop->_mask[2] = gop->_mask[3] = 0xFF;
			gop->_present = 0;

			frm->_length = 3;
			frm->_mask[0] = 0xFF;
			frm->_mask[1] = 0xFF;
			frm->_mask[2] = 0xFC;
			frm->_mask[3] = 0x0;
			frm->_present = 1;
			frm->_syncword[0] = 0;
			frm->_syncword[1] = 0;
			frm->_syncword[2] = 0x80;
			frm->_syncword[3] = 0;

			break;

		case MOTIONJPEG:
			seq->_length = 4;
			seq->_mask[0] = seq->_mask[1] = seq->_mask[2] = seq->_mask[3] = 0xFF;
			seq->_present = 0;

			gop->_length = 4;
			gop->_mask[0] = gop->_mask[1] = gop->_mask[2] = gop->_mask[3] = 0xFF;
			gop->_present = 0;

			frm->_length = 2;
			frm->_mask[0] = 0xFF;
			frm->_mask[1] = 0xFF;
			frm->_present = 1;
			frm->_syncword[0] = 0xFF;
			frm->_syncword[1] = 0xD8;
			break;

		default:
			break;
	}

	StreamBuffer_ComputeResyncIndex(&psb->_seq_header);
	StreamBuffer_ComputeResyncIndex(&psb->_gop_header);
	StreamBuffer_ComputeResyncIndex(&psb->_frm_header);

	// vbi syncword
	vbi->_length = 4;
	vbi->_mask[0] = vbi->_mask[1] = vbi->_mask[2] = vbi->_mask[3] = 0xFF;
	vbi->_present = flags & VBI_PRESENT;
	vbi->_syncword[0] = 0;
	vbi->_syncword[1] = 0;
	vbi->_syncword[2] = 1;
	vbi->_syncword[3] = 0xF6;
	StreamBuffer_ComputeResyncIndex(&psb->_vbi_header);

	// timestamp syncword
	ts->_length = 4;
	ts->_mask[0] = ts->_mask[1] = ts->_mask[2] = ts->_mask[3] = 0xFF;
	ts->_present = flags & FINGERPRINT_PRESENT;
	ts->_syncword[0] = 0;
	ts->_syncword[1] = 0;
	ts->_syncword[2] = 1;
	ts->_syncword[3] = 0xF5;
	StreamBuffer_ComputeResyncIndex(&psb->_ts_header);

	// map syncword
	map->_length = 4;
	map->_mask[0] = ts->_mask[1] = ts->_mask[2] = ts->_mask[3] = 0xFF;
	map->_present = flags & MAP_PRESENT;
	map->_syncword[0] = 0;
	map->_syncword[1] = 0;
	map->_syncword[2] = 1;
	map->_syncword[3] = 0xF8;
	StreamBuffer_ComputeResyncIndex(&psb->_map_header);
}

__inline UINT32 StreamBuffer_MatchByte(unsigned char ch, PSYNCWORD syncword)
{
	if ( syncword->_present ) {
		do {
			if ( syncword->_syncword[syncword->_matched] == ( ch & syncword->_mask[syncword->_matched] ) ) {
				syncword->_matched ++;
				break;
			}
			syncword->_matched = syncword->_resync_index[syncword->_matched];
		} while ( syncword->_matched > 0 );

		if ( syncword->_matched == syncword->_length )
		{
			syncword->_matched = 0;
			return 1;
		}
	}

	return 0;
}

UINT32 StreamBuffer_MatchSyncWord(PSTREAM_BUFFER psb, UINT8* bfr, UINT32 length) // return non-zero if new frame found
{
	UINT32 i;
	PSYNCWORD seq = &(psb->_seq_header);
	PSYNCWORD gop = &(psb->_gop_header);
	PSYNCWORD frm = &(psb->_frm_header);
	PSYNCWORD vbi = &psb->_vbi_header;
	PSYNCWORD ts = &psb->_ts_header;
	PSYNCWORD map = &psb->_map_header;
	PFRAME_ITEM new_frame;
	UINT32 new_frame_found = 0;
	UINT32 total_frame_header = psb->_interlace_coding ? 2 : 1;

	for ( i = 0 ; i < length ; i++ )
	{
		UINT8 ch = bfr[i];

		if ( psb->_state != STATE_NORMAL )
		{
			psb->_parse_count --;

			if ( psb->_state == STATE_PARSE_VBI )
			{
				psb->_tail_frame->_vbi_length <<= 8;
				psb->_tail_frame->_vbi_length |= bfr[i];
				if ( psb->_parse_count == 0 )
				{
					psb->_state = STATE_SKIP_VBI;
					psb->_tail_frame->_vbi_length *= 4;	// vbi length are in the unit of DWORD
					if ( psb->_tail_frame->_vbi_length == 0 ) psb->_state = STATE_NORMAL;
					psb->_parse_count = psb->_tail_frame->_vbi_length;
				}
			} else if ( psb->_state == STATE_PARSE_TIMESTAMP )
			{
				if ( psb->_parse_count >= 8 ) // first 4 bytes are finger print
				{
					// finger print are sent as little endian number
					psb->_tail_frame->_finger_print <<= 8;
					psb->_tail_frame->_finger_print |= bfr[i];
				}
				else if ( psb->_parse_count >= 4 ) // the second 4 bytes are timestamp
				{
					// timestamp are sent as bigger endian number
					psb->_tail_frame->_timestamp <<= 8;
					psb->_tail_frame->_timestamp |= bfr[i];
				}
				else if ( psb->_parse_count < 4 )	// the following 4 bytes are VIP number
				{
					// vip number are sent as bigger endian number
					psb->_tail_frame->_vip_number <<= 8;
					psb->_tail_frame->_vip_number |= bfr[i];
				}
				if ( psb->_parse_count == 0 )
				{
					UINT32 temp = psb->_tail_frame->_vip_number;
					UINT32 vip_number = temp & 0x3FFFFFFF;
					UINT32 frame_type = (temp >> 30) & 0x3;

					// detect duplication frame here
					if ( vip_number == 0 && frame_type == 1 && psb->_frame_serial != 0 )
					{
						psb->_tail_frame->_duplicate_frame = 1;
//						psb->_total_frame_duplicated ++;
						psb->_tail_frame->_vip_number = vip_number;
						psb->_tail_frame->_frame_type = frame_type;
					}
					else
					{
						psb->_tail_frame->_frame_type = psb->_next_frame_type;
						psb->_tail_frame->_vip_number = psb->_next_vip_number;
						psb->_next_frame_type = frame_type;
						psb->_next_vip_number = vip_number;
					}
					psb->_state = STATE_NORMAL;
				}
			} else if ( psb->_state == STATE_SKIP_VBI )
			{
				if ( psb->_parse_count == 0 ) psb->_state = STATE_NORMAL;
			} else if ( psb->_state == STATE_PARSE_MAP )
			{
				if ( psb->_parse_count == 0 ) psb->_state = STATE_NORMAL;
			}

			if ( psb->_state == STATE_NORMAL )	// we are returning from other state to normal state, next frame should start here.
			{
				psb->_seq_header._matched = 0;
				psb->_gop_header._matched = 0;
				psb->_frm_header._matched = 0;
				psb->_vbi_header._matched = 0;
				psb->_ts_header._matched = 0;
				psb->_map_header._matched = 0;
			}
			continue;
		}

		{
			UINT32 syncword_length=0;
			UINT32 seq_found = StreamBuffer_MatchByte(ch, seq);
			UINT32 gop_found = StreamBuffer_MatchByte(ch, gop);
			UINT32 frm_found = StreamBuffer_MatchByte(ch, frm);
			UINT32 vbi_found = StreamBuffer_MatchByte(ch, vbi);
			UINT32 ts_found = StreamBuffer_MatchByte(ch, ts);
			UINT32 map_found = StreamBuffer_MatchByte(ch, map);

			if ( seq_found == 0 && gop_found == 0 && frm_found == 0 && vbi_found == 0 && ts_found == 0 && map_found == 0 ) continue;

			if ( seq_found ) syncword_length = seq->_length;
			if ( gop_found ) syncword_length = gop->_length;
			if ( frm_found ) syncword_length = frm->_length;
			if ( vbi_found ) syncword_length = vbi->_length;
			if ( ts_found ) syncword_length = ts->_length;
			if ( map_found ) syncword_length = map->_length;

			if ( psb->_tail_frame->_frame_header_founded == total_frame_header )  // new frame has completely arrived
			{
				ULONG start = psb->_tail_frame->_frame_start;
				ULONG end = psb->_write_byte_index + i + 1 - syncword_length;

				// After we found the frame header of the last frame, only when we meet 
				// the first syncword we should calculate the frame length, and only when
				// we meet seq or gop or frm, we should allocate new FRAME_ITEM, because 
				// all of the vbi and fingerprint should belong to the last frame.
				if ( psb->_tail_frame->_frame_length == 0 ) 
				{
					psb->_tail_frame->_frame_length = ( end + psb->_buffer_size - start ) % ( psb->_buffer_size );
				}

				if ( seq_found || gop_found || frm_found )
				{
					new_frame = PIAllocMem(sizeof(FRAME_ITEM));
					if(seq_found) 
						StreamBuffer_PushFrame(psb, new_frame,SEQ_START);
					else 
						StreamBuffer_PushFrame(psb, new_frame,SEQ_MIDDLE);
					new_frame_found = 1;
					new_frame->_frame_start = ( end + psb->_buffer_size ) % ( psb->_buffer_size );
				}
			}

			if ( vbi_found ) { 
				ULONG syncword_pos = psb->_write_byte_index + i + 1 - syncword_length;	// start position of the syncword
				psb->_tail_frame->_vbi_start = ( syncword_pos + psb->_buffer_size + 6 ) % ( psb->_buffer_size ); // 4 bytes for syncword length and 2 bytes for length
				psb->_state = STATE_PARSE_VBI; 
				psb->_parse_count = 2; 
				psb->_tail_frame->_flag |= VBI_PRESENT;
				psb->_tail_frame->_vbi_length = 0;
			}

			if ( ts_found ) { 
				psb->_state = STATE_PARSE_TIMESTAMP; 
				psb->_parse_count = 12; 
				psb->_tail_frame->_flag |= FINGERPRINT_PRESENT;
				psb->_tail_frame->_finger_print = 0;
				psb->_tail_frame->_vip_number = 0;
			}

			if ( map_found ) {
				ULONG syncword_pos = psb->_write_byte_index + i + 1 - syncword_length;	// start position of the syncword
				psb->_tail_frame->_map_start = ( syncword_pos + psb->_buffer_size + 4 ) % ( psb->_buffer_size ); // 4 bytes for syncword length and 2 bytes for length
				psb->_state = STATE_PARSE_MAP; 
				psb->_parse_count = MAP_BUFFER_LENGTH; 
				psb->_tail_frame->_flag |= MAP_PRESENT;
			}

			if ( frm_found ) psb->_tail_frame->_frame_header_founded ++;
		}
	}

	return new_frame_found;
}

UINT32 StreamBuffer_QuickMatchSyncWord(PSTREAM_BUFFER psb, UINT8* bfr, UINT32 length) // return non-zero if new frame found
{
	UINT32 i;
	PSYNCWORD seq = &(psb->_seq_header);
	PSYNCWORD gop = &(psb->_gop_header);
	PSYNCWORD frm = &(psb->_frm_header);
	PSYNCWORD vbi = &psb->_vbi_header;
	PSYNCWORD ts = &psb->_ts_header;
	PSYNCWORD map = &psb->_map_header;

	PFRAME_ITEM new_frame;
	UINT32 new_frame_found = 0;
	UINT32 total_frame_header = psb->_interlace_coding ? 2 : 1;

	for ( i = 0 ; i < length ; i++ )
	{
		UINT8 ch = bfr[i];

		if ( psb->_state != STATE_NORMAL )
		{
			psb->_parse_count --;

			if ( psb->_state == STATE_PARSE_VBI )
			{
				psb->_tail_frame->_vbi_length <<= 8;
				psb->_tail_frame->_vbi_length |= bfr[i];
				if ( psb->_parse_count == 0 )
				{
					psb->_state = STATE_SKIP_VBI;
					psb->_tail_frame->_vbi_length *= 4;	// vbi length are in the unit of DWORD
					if ( psb->_tail_frame->_vbi_length == 0 ) psb->_state = STATE_NORMAL;
					psb->_parse_count = psb->_tail_frame->_vbi_length;
				}
			} else if ( psb->_state == STATE_PARSE_TIMESTAMP )
			{
				if ( psb->_parse_count >= 8 ) // first 4 bytes are finger print
				{
					// finger print are sent as little endian number
					psb->_tail_frame->_finger_print <<= 8;
					psb->_tail_frame->_finger_print |= bfr[i];
				}
				else if ( psb->_parse_count >= 4 ) // the second 4 bytes are timestamp
				{
					// timestamp are sent as bigger endian number
					psb->_tail_frame->_timestamp <<= 8;
					psb->_tail_frame->_timestamp |= bfr[i];
				}
				else if ( psb->_parse_count < 4 )	// the following 4 bytes are VIP number
				{
					// vip number are sent as bigger endian number
					psb->_tail_frame->_vip_number <<= 8;
					psb->_tail_frame->_vip_number |= bfr[i];
				}
				if ( psb->_parse_count == 0 )
				{
					UINT32 temp = psb->_tail_frame->_vip_number;
					UINT32 vip_number = temp & 0x3FFFFFFF;
					UINT32 frame_type = (temp >> 30) & 0x3;

					// detect duplication frame here
					if ( vip_number == 0 && frame_type == 1 && psb->_frame_serial != 0 )
					{
						psb->_tail_frame->_duplicate_frame = 1;
//						psb->_total_frame_duplicated ++;
						psb->_tail_frame->_vip_number = vip_number;
						psb->_tail_frame->_frame_type = frame_type;
					}
					else
					{
						psb->_tail_frame->_frame_type = psb->_next_frame_type;
						psb->_tail_frame->_vip_number = psb->_next_vip_number;
						psb->_next_frame_type = frame_type;
						psb->_next_vip_number = vip_number;
					}
					psb->_state = STATE_NORMAL;
				}
			} else if ( psb->_state == STATE_SKIP_VBI )
			{
				if ( psb->_parse_count == 0 ) psb->_state = STATE_NORMAL;
			} else if ( psb->_state == STATE_PARSE_MAP )
			{
				if ( psb->_parse_count == 0 ) psb->_state = STATE_NORMAL;
			} 

			if ( psb->_state == STATE_NORMAL )	// we are returning from other state to normal state, next frame should start here.
			{
				psb->_seq_header._matched = 0;
				psb->_gop_header._matched = 0;
				psb->_frm_header._matched = 0;
				psb->_vbi_header._matched = 0;
				psb->_ts_header._matched = 0;
				psb->_map_header._matched = 0;
			}
			continue;
		}

		{
			UINT32 syncword_length = 4;
			UINT32 seq_found, gop_found, frm_found, vbi_found, ts_found, map_found ;

			if ( psb->_matching_in_process == 0 )
			{
				if ( ch != 0 ) continue; 
				//if ( i < length - 1 && psb->_bfr[i+1] != 0 ) continue;
				psb->_matching_in_process = 1;
			}
			else if ( psb->_matching_in_process == 1 )
			{
				if ( ch != 0 ) 
					psb->_matching_in_process = 0; // fall through to let syncword reset
				else
					psb->_matching_in_process = 2;
			}
			else if ( psb->_matching_in_process == 2 )
			{
				if ( ch == 0 )
					psb->_matching_in_process = 2;
				else if ( ch != 1 )
					psb->_matching_in_process = 0; //SJ - 00 00 xx 00 01 should not match
				//	psb->_matching_in_process = 1; // fall through to let syncword reset
				else
					psb->_matching_in_process = 3;
			}
			else if ( psb->_matching_in_process == 3 )
			{
				seq_found = ( ( psb->_seq_header._present ) && ( ch == psb->_seq_header._syncword[3] ) ) ? 1 : 0;
				gop_found = ( ( psb->_gop_header._present ) && ( ch == psb->_gop_header._syncword[3] ) ) ? 1 : 0;
				frm_found = ( ( psb->_frm_header._present ) && ( ch == psb->_frm_header._syncword[3] ) ) ? 1 : 0;
				vbi_found = ( ( psb->_vbi_header._present ) && ( ch == psb->_vbi_header._syncword[3] ) ) ? 1 : 0;
				ts_found = ( ( psb->_ts_header._present ) && ( ch == psb->_ts_header._syncword[3] ) ) ? 1 : 0;
				map_found = ( ( psb->_map_header._present ) && ( ch == psb->_map_header._syncword[3] ) ) ? 1 : 0;

				if ( seq_found == 0 && gop_found == 0 && frm_found == 0 && vbi_found == 0 && ts_found == 0 && map_found == 0 ) 
				{
					if ( ch != 0 )
						psb->_matching_in_process = 0;
					else
						psb->_matching_in_process = 1;
					continue;
				}

				psb->_matching_in_process = 0;

				if ( psb->_tail_frame->_frame_header_founded == total_frame_header )  // new frame has completely arrived
				{
					ULONG start = psb->_tail_frame->_frame_start;
					ULONG end = psb->_write_byte_index + i + 1 - syncword_length;

					// After we found the frame header of the last frame, only when we meet 
					// the first syncword we should calculate the frame length, and only when
					// we meet seq or gop or frm, we should allocate new FRAME_ITEM, because 
					// all of the vbi and fingerprint should belong to the last frame.
					if ( psb->_tail_frame->_frame_length == 0 ) 
					{
						psb->_tail_frame->_frame_length = ( end + psb->_buffer_size - start ) % ( psb->_buffer_size );
					}

					if ( seq_found || gop_found || frm_found )
					{
						new_frame = PIAllocMem(sizeof(FRAME_ITEM));
						if(seq_found)
							StreamBuffer_PushFrame(psb, new_frame,SEQ_START);
						else 
							StreamBuffer_PushFrame(psb, new_frame,SEQ_MIDDLE);
						new_frame_found = 1;
						new_frame->_frame_start = ( end + psb->_buffer_size ) % ( psb->_buffer_size );
					}
				}

				if ( vbi_found ) { 
					ULONG syncword_pos = psb->_write_byte_index + i + 1 - syncword_length;	// start position of the syncword
					psb->_tail_frame->_vbi_start = ( syncword_pos + psb->_buffer_size + 6 ) % ( psb->_buffer_size ); // 4 bytes for syncword length and 2 bytes for length
					psb->_state = STATE_PARSE_VBI; 
					psb->_parse_count = 2; 
					psb->_tail_frame->_flag |= VBI_PRESENT;
					psb->_tail_frame->_vbi_length = 0;
				}

				if ( ts_found ) { 
					psb->_state = STATE_PARSE_TIMESTAMP; 
					psb->_parse_count = 12; 
					psb->_tail_frame->_flag |= FINGERPRINT_PRESENT;
					psb->_tail_frame->_finger_print = 0;
					psb->_tail_frame->_vip_number = 0;
				}

				if ( map_found ) {
					ULONG syncword_pos = psb->_write_byte_index + i + 1 - syncword_length;	// start position of the syncword
					psb->_tail_frame->_map_start = ( syncword_pos + psb->_buffer_size + 4 ) % ( psb->_buffer_size ); // 4 bytes for syncword length and 2 bytes for length
					psb->_state = STATE_PARSE_MAP; 
					psb->_parse_count = MAP_BUFFER_LENGTH; 
					psb->_tail_frame->_flag |= MAP_PRESENT;
				}

				if ( frm_found ) psb->_tail_frame->_frame_header_founded ++;
			}
		}
	}

	return new_frame_found;
}

UINT32 StreamBuffer_ComputeFramePTS(PSTREAM_BUFFER psb, PFRAME_INFO frame)
{
	// this algorithm will not work if the frame structure is not complete!
	// this algorithm will not valid if the sub_gop_size of the IPB mode is not 3!
	#define SUB_GOP_SIZE 3

	ULONG frame_serial =  frame->_frame_serial ;
 
	if ( psb->_sequence_mode == IONLY )
	{
		return frame_serial;
	}
	else if ( psb->_sequence_mode == IPONLY )
	{
		return frame_serial;
	}
	else if ( psb->_sequence_mode == IPB )
	{
		switch ( frame->_frame_type )
		{
			case I_FRAME:	// I frame
			//case I_TOP_FIELD:
			//case I_BOTTOM_FIELD:
				return frame_serial + SUB_GOP_SIZE;
			case P_FRAME:	// P frame
			//case P_TOP_FIELD:
			//case P_BOTTOM_FIELD:
				if ( frame->_duplicate_frame )
					return frame_serial + 1;
				else
					return frame_serial + SUB_GOP_SIZE;
			case B_FRAME:	// B frame
			//case B_TOP_FIELD:
			//case B_BOTTOM_FIELD:
				return frame_serial;
			default:
				return 0;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////
// public function
////////////////////////////////////////////////////////////

NTSTATUS StreamBuffer_Init(PSTREAM_BUFFER psb, 
						   UINT32 buffer_size, 
						   TCFGVIDEOEX* video_config,
						   UINT32 flags)
{
	psb->_buffer_size = buffer_size; 
	psb->_bfr = (UINT8 *)PIAllocVirtualMem(psb->_buffer_size);
	if( psb->_bfr == NULL ) return STATUS_UNSUCCESSFUL;

	psb->_head_frame = psb->_tail_frame = NULL;

	psb->_stream_format = ( video_config->strcfg.compress_mode >= GO ) ? GO : video_config->strcfg.compress_mode;
	psb->_sequence_mode = video_config->strcfg.sequence;
	psb->_interlace_coding = ( video_config->strcfg.deinterlace_mode == 2 ) ? 1 : 0;
	StreamBuffer_InitSyncWord(psb, video_config, flags);

	INITLOCK(psb);

	StreamBuffer_Reset(psb);

	return STATUS_SUCCESS;
}

NTSTATUS StreamBuffer_Reset(PSTREAM_BUFFER psb)
{
	ULONG i;

	LOCKBUFFER(psb);

	if ( psb->_head_frame )
	{
		PFRAME_ITEM iterator = psb->_head_frame;
		while(iterator)
		{
			psb->_head_frame = psb->_head_frame->_next;
			PIFreeMem(iterator);
			iterator = psb->_head_frame;
		}
	}

	psb->_head_frame = psb->_tail_frame = (PFRAME_ITEM)PIAllocMem(sizeof(FRAME_ITEM)); // dummy head
	psb->_frame_count = 0;
	psb->_frame_serial = 0;
	psb->_next_frame_type = 0;	// always assume the first frame will be I frame;
	psb->_next_vip_number = (psb->_sequence_mode==IPB) ? 3 : 1; // IONLY and IPONLY initialize to 1, else 3

	psb->_head_frame->_frame_length = 0;
	psb->_head_frame->_frame_start = 0;
	psb->_head_frame->_frame_type = 0;
	psb->_head_frame->_vbi_length = 0;
	psb->_head_frame->_vbi_start = 0;
	psb->_head_frame->_flag = 0;
	psb->_head_frame->_next = NULL;
	psb->_head_frame->_prev = NULL;
	psb->_head_frame->_finger_print = 0;
	psb->_head_frame->_vip_number = 0;
	psb->_head_frame->_frame_serial = psb->_frame_serial ++;
	psb->_head_frame->_duplicate_frame = 0;
	psb->_head_frame->_timestamp = 0;
	psb->_head_frame->_frame_header_founded = 0;
	psb->_head_frame->_reference_count = 0;
        /*2005-04-15 add for fixing bug 1584*/
	psb->_head_frame->_video_timestamp.tv_sec = 0;
	psb->_head_frame->_video_timestamp.tv_usec = 0;
        /*2005-04-15 add end*/

	psb->_read_byte_index = psb->_write_byte_index = 0;

	psb->_first_syncword_founded = 0;

	psb->_gop_header._matched = 0;
	psb->_seq_header._matched = 0;
	psb->_frm_header._matched = 0;
	psb->_vbi_header._matched = 0;
	psb->_ts_header._matched = 0;

	psb->_matching_in_process = 0;

	psb->_state = STATE_NORMAL;
	psb->_parse_count = 0;

	for ( i = 0 ; i < MAX_CHANNEL ; i ++ )
	{
		psb->_channels[i]._valid = FALSE;
		psb->_channels[i]._frame_count = 0;
		psb->_channels[i]._head_frame = NULL;
		psb->_channels[i]._tail_frame = NULL;
		psb->_channels[i]._callback = NULL;
		psb->_channels[i]._context = NULL;
	}

	// statistics information
	psb->_total_frame_dropped = 0;
	// this is being used for counting frame drop recovery related duplication
	psb->_total_frame_duplicated = 0;
	psb->_total_bytes_accquired = 0;

	UNLOCKBUFFER(psb); 

	return STATUS_SUCCESS;
}

NTSTATUS StreamBuffer_Uninit(PSTREAM_BUFFER psb)
{
	LOCKBUFFER(psb);

	psb->_frame_count = 0;

	if ( ( psb->_bfr == NULL ) && ( psb->_head_frame == NULL ) ) goto StreamBuffer_Uninit_Done;
	
	if ( psb->_bfr )
	{
		PIFreeVirtualMem(psb->_bfr);
		psb->_bfr = NULL;
	}

	if ( psb->_head_frame )
	{
		PFRAME_ITEM iterator = psb->_head_frame;
		while(iterator)
		{
			psb->_head_frame = psb->_head_frame->_next;
			PIFreeMem(iterator);
			iterator = psb->_head_frame;
		}

		psb->_head_frame = NULL;
	}

StreamBuffer_Uninit_Done:
	UNLOCKBUFFER(psb);
	return STATUS_SUCCESS;
}

int StreamBuffer_AddBlock(PSTREAM_BUFFER psb, UINT8* bfr, UINT32 length)
{
	int r, i;
	UINT32  new_frame_callback = 0;
	PFRAME_ITEM frame;

    DEBUG_FUNC_ENTER("StreamBuffer_AddBlock")
    LOCKBUFFER(psb);

	if ( psb->_bfr == NULL || psb->_head_frame == NULL ) {
		r = -1; // exception
		goto StreamBuffer_AddBlock_Done;
	}

	while(1) {
		// calculate the left space of buffer to find if there is space for new block
		if ( length <= StreamBuffer_ByteAvailable(psb) ) break;
	
		if ( psb->_head_frame == psb->_tail_frame )	{
			r = -2; // exception
			PrintMsg(DEBUG_VIDEO_STREAMBUFFER, ("Stream Buffer Overlapped & No Frame To Drop!!!"));
			goto StreamBuffer_AddBlock_Done;
		}

		// drop first frame in buffer
		frame = StreamBuffer_DropFrame(psb);
		ASSERT( frame != NULL );
		PIFreeMem(frame);

		PrintMsg(DEBUG_VIDEO_STREAMBUFFER, ("Frame Dropped In Stream Buffer!!"));
	}

	/*add 05-2-19 for timestamp*/	
	do_gettimeofday(&(psb->_current_time));
	if ((psb->_tail_frame->_video_timestamp.tv_sec == 0)&&
		(psb->_tail_frame->_video_timestamp.tv_usec == 0))
		psb->_tail_frame->_video_timestamp = psb->_current_time;
	/*add 05-2-19 end*/
	if ( psb->_stream_format == MPEG1 || psb->_stream_format == MPEG2 || psb->_stream_format == MPEG4 )
		new_frame_callback = StreamBuffer_QuickMatchSyncWord(psb, bfr, length);
	else
		new_frame_callback = StreamBuffer_MatchSyncWord(psb, bfr, length);

	// copy new block to buffer
	if ( psb->_write_byte_index + length <= psb->_buffer_size )
	{
		PICopyMem(psb->_bfr+psb->_write_byte_index, bfr, length);
	}
	else
	{
		ULONG first_chunk_size = psb->_buffer_size - psb->_write_byte_index;
		PICopyMem(psb->_bfr+psb->_write_byte_index, bfr, first_chunk_size);
		PICopyMem(psb->_bfr, bfr+first_chunk_size, length-first_chunk_size); 
	}
	psb->_write_byte_index += length;
	psb->_write_byte_index = psb->_write_byte_index % ( psb->_buffer_size );

StreamBuffer_AddBlock_Done:

	UNLOCKBUFFER(psb);

	for ( i = 0 ; i < MAX_CHANNEL; i ++ )
	{
		if ( psb->_channels[i]._valid == FALSE ) continue;
		if ( new_frame_callback && psb->_channels[i]._callback )
			(*psb->_channels[i]._callback)(i, NEWFRAME_EVENT, psb->_channels[i]._context);
	}

	r = psb->_frame_count;

    DEBUG_FUNC_EXIT("StreamBuffer_AddBlock")
	return r;
}

UINT32 StreamBuffer_GetFrameCount(PSTREAM_BUFFER psb) 
{ 
	return psb->_frame_count;
}

NTSTATUS StreamBuffer_OpenChannel(PSTREAM_BUFFER psb, CHANNEL_ID* channel_id)
{
	ULONG channel_index;
	NTSTATUS ntStatus = STATUS_SUCCESS;

    DEBUG_FUNC_ENTER("StreamBuffer_OpenChannel")
	LOCKBUFFER(psb);

	for ( channel_index = 0 ; channel_index < MAX_CHANNEL ; channel_index ++ )
	{
		if ( psb->_channels[channel_index]._valid == TRUE ) continue;
		psb->_channels[channel_index]._valid = TRUE;
		psb->_channels[channel_index]._frame_count = 0;
		psb->_channels[channel_index]._head_frame = psb->_head_frame;
		psb->_channels[channel_index]._tail_frame = psb->_head_frame;
		psb->_channels[channel_index]._callback = NULL;
		psb->_channels[channel_index]._context = NULL;
		psb->_channel_count ++;
		*channel_id = channel_index;

		{
			PFRAME_ITEM iterator = psb->_head_frame;
			while ( iterator != NULL )
			{
				StreamBuffer_PushChannelFrame(psb, iterator, channel_index);
				iterator = iterator->_next;
			}
			psb->_channels[channel_index]._frame_count --; // the last one frame is dummy frame;
		}

		goto OpenChannelDone;
	}

	*channel_id = (ULONG)(-1);
	ntStatus = STATUS_UNSUCCESSFUL;

OpenChannelDone:
	UNLOCKBUFFER(psb);
    DEBUG_FUNC_EXIT("StreamBuffer_OpenChannel")
	return ntStatus;
}

NTSTATUS StreamBuffer_CloseChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;

	LOCKBUFFER(psb);

	if ( psb->_channels[channel_id]._valid == FALSE )
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto CloseChannelDone;
	}

	while ( psb->_channels[channel_id]._frame_count > 0 )
	{
		PFRAME_ITEM iterator = StreamBuffer_PopChannelFrame(psb, channel_id);
		if ( iterator->_reference_count == 0 ) PIFreeMem(iterator);
	}
	
	// remove the reference count on the dummy frame;
	psb->_tail_frame->_reference_count --;

	psb->_channels[channel_id]._valid = FALSE;
	psb->_channels[channel_id]._head_frame = NULL;
	psb->_channels[channel_id]._tail_frame = NULL;
	psb->_channels[channel_id]._callback = NULL;
	psb->_channels[channel_id]._context = NULL;
	psb->_channel_count --;

CloseChannelDone:
	UNLOCKBUFFER(psb);
	return ntStatus;
}

NTSTATUS StreamBuffer_SetChannelCallBack(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, STREAMBUFFER_CALLBACKFUNC fp, void* context)
{
    DEBUG_FUNC_ENTER("StreamBuffer_SetChannelCallBack")
	psb->_channels[channel_id]._callback = fp;
	psb->_channels[channel_id]._context = context;
    DEBUG_FUNC_EXIT("StreamBuffer_SetChannelCallBack")
	return STATUS_SUCCESS;
}

UINT32 StreamBuffer_GetChannelFrameCount(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	return psb->_channels[channel_id]._frame_count ;
}

NTSTATUS StreamBuffer_GetChannelFrame(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, PFRAME_INFO frmInfo)
{

	NTSTATUS ntStatus = STATUS_SUCCESS;
	PFRAME_ITEM next_frame;
	PSTREAM_CHANNEL channel = &(psb->_channels[channel_id]);

	LOCKBUFFER(psb);
	frmInfo->_flag = 0;

	if ( StreamBuffer_GetChannelFrameCount(psb, channel_id) == 0 ) // no frame available
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto StreamBuffer_GetChannelFrame_Done;
	}

	if ( ( frmInfo->_frame_buffer != NULL ) && (channel->_head_frame->_frame_length > frmInfo->_max_frame_length) ) // target buffer too small
	{
		ntStatus = STATUS_BUFFER_TOO_SMALL;
		goto StreamBuffer_GetChannelFrame_Done;
	}

	if ( ( frmInfo->_vbi_buffer != NULL ) && (channel->_head_frame->_vbi_length > frmInfo->_max_vbi_length) ) // target vbi buffer too small
	{
		ntStatus = STATUS_BUFFER_TOO_SMALL;
		goto StreamBuffer_GetChannelFrame_Done;
	}

	// If frames have been dropped then reuse the head frame to make up for them 
	if( psb->_total_frame_dropped > psb->_total_frame_duplicated ) {
	  next_frame = StreamBuffer_PeekChannelFrame(psb, channel_id);
	  psb->_total_frame_duplicated ++;
	}
	else {
	  next_frame = StreamBuffer_PopChannelFrame(psb, channel_id);
	}
	if ( next_frame == NULL ) // I do not know if this can happen
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		goto StreamBuffer_GetChannelFrame_Done;
	}

	if ( frmInfo->_frame_buffer )
	{
		// copy data to target buffer;
		if ( next_frame->_frame_start + next_frame->_frame_length > psb->_buffer_size )
		{
			ULONG first_chunk_size = psb->_buffer_size - next_frame->_frame_start;
			PICopyToUser(frmInfo->_frame_buffer, psb->_bfr + next_frame->_frame_start, first_chunk_size);
			PICopyToUser(frmInfo->_frame_buffer + first_chunk_size, psb->_bfr, next_frame->_frame_length - first_chunk_size);
		}
		else
		{
			PICopyToUser(frmInfo->_frame_buffer, psb->_bfr + next_frame->_frame_start, next_frame->_frame_length);
		}

	}

	frmInfo->_actual_frame_length = next_frame->_frame_length;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the signature from the frame
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GO_STREAM_SEQ_HEADER_LENGTH 24  // sequence header length of the GO stream

	frmInfo->_finger_print = 0;
	frmInfo->_vip_number = 0;
	frmInfo->_frame_type = 0;
	frmInfo->_frame_serial = next_frame->_frame_serial;
	/*add 05-2-19 for timestamp*/
	frmInfo->_video_timestamp = next_frame->_video_timestamp;
	/*add 05-2-19 end*/
	if ( psb->_stream_format == GO )
	{
		// the signature begins at the 8th (zero based) byte of the frame header
		UINT32* sig = NULL;
		UINT8*  ts = NULL;
		ULONG len = 0;
		if ( frmInfo->_frame_buffer[0] == psb->_frm_header._syncword[0] &&
			 frmInfo->_frame_buffer[1] == psb->_frm_header._syncword[1] &&
			 frmInfo->_frame_buffer[2] == psb->_frm_header._syncword[2] &&
			 frmInfo->_frame_buffer[3] == psb->_frm_header._syncword[3] )
		{
			 sig = (UINT32*)(frmInfo->_frame_buffer + 8);
			 ts = (UINT8*)(frmInfo->_frame_buffer + 4);
			 len = 12;
		}
		else if ( frmInfo->_frame_buffer[0] == psb->_seq_header._syncword[0] &&
				  frmInfo->_frame_buffer[1] == psb->_seq_header._syncword[1] &&
				  frmInfo->_frame_buffer[2] == psb->_seq_header._syncword[2] &&
				  frmInfo->_frame_buffer[3] == psb->_seq_header._syncword[3] )
		{
			sig = (UINT32*)(frmInfo->_frame_buffer + GO_STREAM_SEQ_HEADER_LENGTH + 8 );
			ts = (UINT8*)(frmInfo->_frame_buffer + GO_STREAM_SEQ_HEADER_LENGTH + 4 );
			len = GO_STREAM_SEQ_HEADER_LENGTH + 12;
		}

		if ( sig && ts && len <= next_frame->_frame_length ) // ensure that we do not read random data or page fault
		{
			ULONG tmp = GET_BIG_ENDIAN_ULONG(ts);
			frmInfo->_finger_print = *sig;
			frmInfo->_vip_number = tmp & 0x1FFFFFFF;
			frmInfo->_frame_type = ( tmp >> 29 ) & 0x3;
			frmInfo->_flag |= FINGERPRINT_PRESENT;
		}
	}
	else if ( next_frame->_flag & FINGERPRINT_PRESENT )
	{
		// the signature begins at the end of each frame
		frmInfo->_finger_print = next_frame->_finger_print;
		frmInfo->_vip_number = next_frame->_vip_number;
		frmInfo->_frame_type = next_frame->_frame_type;
		frmInfo->_timestamp = next_frame->_timestamp;
		frmInfo->_flag |= FINGERPRINT_PRESENT;
	}


	frmInfo->_actual_vbi_length = 0;
	if ( ( next_frame->_flag & VBI_PRESENT ) &&		// if there is some vbi data
		( frmInfo->_vbi_buffer != NULL ) )			// and the client are interested in it
	{
		// copy data to vbi buffer;
		if ( next_frame->_vbi_start + next_frame->_vbi_length > psb->_buffer_size )
		{
			ULONG first_chunk_size = psb->_buffer_size - next_frame->_vbi_start;
                        /*2005-03-30 modified*/
			//PICopyToUser(frmInfo->_vbi_buffer, psb->_bfr + next_frame->_vbi_start, first_chunk_size);
			//PICopyToUser(frmInfo->_vbi_buffer + first_chunk_size, psb->_bfr, next_frame->_vbi_length - first_chunk_size);
			PICopyMem(frmInfo->_vbi_buffer, psb->_bfr + next_frame->_vbi_start, first_chunk_size);
			PICopyMem(frmInfo->_vbi_buffer + first_chunk_size, psb->_bfr, next_frame->_vbi_length - first_chunk_size);
                        /*2005-03-30 end*/
		}
		else
		{
                        /*2005-03-30 modified*/
			//PICopyToUser(frmInfo->_vbi_buffer, psb->_bfr + next_frame->_vbi_start, next_frame->_vbi_length);
			PICopyMem(frmInfo->_vbi_buffer, psb->_bfr + next_frame->_vbi_start, next_frame->_vbi_length);
                        /*2005-03-30 end*/
		}
		frmInfo->_actual_vbi_length = next_frame->_vbi_length;
		frmInfo->_flag |= VBI_PRESENT;
	}

	if ( ( next_frame->_flag & MAP_PRESENT ) &&		// if there is some map data
		( frmInfo->_map_buffer != NULL ) )			// and the client are interested in it
	{
		// copy data to map buffer;
		if ( next_frame->_map_start + MAP_BUFFER_LENGTH > psb->_buffer_size )
		{
			ULONG first_chunk_size = psb->_buffer_size - next_frame->_map_start;
			PICopyMem(frmInfo->_map_buffer, psb->_bfr + next_frame->_map_start, first_chunk_size);
			PICopyMem(frmInfo->_map_buffer + first_chunk_size, psb->_bfr, MAP_BUFFER_LENGTH - first_chunk_size);
		}
		else
		{
			PICopyMem(frmInfo->_map_buffer, psb->_bfr + next_frame->_map_start, MAP_BUFFER_LENGTH);
		}
		frmInfo->_flag |= MAP_PRESENT;
	}

	frmInfo->_duplicate_frame = next_frame->_duplicate_frame;
	frmInfo->_frame_sequence_position=next_frame->_frame_sequence_position;
/*
printk("get one video frame seq_position=%d\n",frmInfo->_frame_sequence_position);
*/
	frmInfo->_frame_pts = StreamBuffer_ComputeFramePTS(psb, frmInfo);

	if ( next_frame->_reference_count == 0 ) PIFreeMem(next_frame);

StreamBuffer_GetChannelFrame_Done:
	UNLOCKBUFFER(psb);

	if ( frmInfo->_duplicate_frame && psb->_channels[channel_id]._callback )
		(*psb->_channels[channel_id]._callback)(channel_id, DUPFRAME_EVENT, psb->_channels[channel_id]._context);

	return ntStatus;
}

VOID StreamBuffer_Lock(PSTREAM_BUFFER psb)
{
	LOCKBUFFER(psb);
}

VOID StreamBuffer_Unlock(PSTREAM_BUFFER psb)
{
	UNLOCKBUFFER(psb);
}

// you need hold the lock of the stream buffer while you access the returned frame pointers
PFRAME_ITEM StreamBuffer_GetFirstFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	return ( psb->_channels[channel_id]._frame_count > 0 ) ? 
		psb->_channels[channel_id]._head_frame : NULL;
}

PFRAME_ITEM StreamBuffer_GetLastFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id)
{
	return ( psb->_channels[channel_id]._frame_count > 0 ) ? 
		psb->_channels[channel_id]._tail_frame->_prev : NULL;
}

PFRAME_ITEM StreamBuffer_GetNextFrameFromChannel(PSTREAM_BUFFER psb, CHANNEL_ID channel_id, PFRAME_ITEM pFrame)
{
	return ( pFrame == psb->_channels[channel_id]._tail_frame ) ? NULL : pFrame->_next;
}

VOID StreamBuffer_GetStatistics(PSTREAM_BUFFER psb, UINT32* total_acquired, UINT32* total_dropped, UINT32* total_duplicated, UINT64* total_bytes_acquired)
{
	if ( total_acquired )
	{
		*total_acquired = psb->_frame_serial;
	}

	if ( total_dropped )
	{
		*total_dropped = psb->_total_frame_dropped;
	}
	
	if ( total_duplicated )
	{
		*total_duplicated = psb->_total_frame_duplicated;
	}

	if ( total_bytes_acquired )
	{
		*total_bytes_acquired = psb->_total_bytes_accquired;
	}
}

/****************************** end of StreamBuffer.c ************************/
