// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.PESPacket.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.System.h"
#include "BFC.Base.Utils.h"

#include "VMP.M2S.PESPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PESPacket )

// ============================================================================

M2S::PESPacket::PESPacket(
	const	Bool		MPEG2Syntax ) :

	isMPEG2			( MPEG2Syntax ),
	packetReady		( false ),
	data_align_indicator	( false ),
	PTS_flag		( false ),
	DTS_flag		( false ),
	startCodeChecked	( false ) {

}

// ============================================================================

Uint32 M2S::PESPacket::decode(
	const	Buffer &	payload ) {

	if ( packetReady ) {
		return 0;
	}

	packetDump += payload;

	if ( ! startCodeChecked && packetDump.getLength() >= 3 ) {
		if ( packetDump[ 0 ] != 0x00
		  || packetDump[ 1 ] != 0x00
		  || packetDump[ 2 ] != 0x01 ) {
			throw InvalidArgument( "Invalid start code prefix!" );
		}
		startCodeChecked = true;
	}

	if ( packetDump.getLength() < 6 ) {
		return payload.getLength();
	}

	streamType = packetDump[ 3 ];
	packetLength = ( (Uint32)( packetDump[ 4 ] ) << 8 )
	             | ( (Uint32)( packetDump[ 5 ] )      );

	if ( ! packetLength ) {
		// May grow indefinitely...
		return payload.getLength();
	}

	Uint32	expLength = 6 + packetLength;
	Uint32	back = 0;

	if ( packetDump.getLength() > expLength ) {
		// We copied too much from payload...
		back = packetDump.getLength() - expLength;
		packetDump = packetDump( 0, expLength );
	}

	if ( packetDump.getLength() == expLength ) {
		packetReady = true;
		decodeFields();
	}

	return payload.getLength() - back;

}

// ============================================================================

Buffer M2S::PESPacket::encode() {

	if ( ! isMPEG2 ) {
		throw InvalidArgument( "Can't encode MPEG-1 :-(" );
	}

	headerLength = ( PTS_flag ? ( DTS_flag ? 10 : 5 ) : 0 );

	Uint32 total_length
		= 6		// 00.00.01.<id>.<len>
		+ 3		// header
		+ headerLength
		+ data_byte.getLength();

	packetDump.resize( total_length );

	Uchar *	buf = packetDump.getVarPtr();

	packetLength = total_length - 6;

	if ( packetLength > 0xFFFF ) {
		packetLength = 0;
	}

	*buf++ = 0x00;
	*buf++ = 0x00;
	*buf++ = 0x01;
	*buf++ = streamType;
	*buf++ = ( packetLength >> 8 );
	*buf++ = ( packetLength & 0xFF );

	*buf++ = 0x80 | ( data_align_indicator ? 0x04 : 0x00 );
	*buf++ = ( PTS_flag ? 0x80 : 0x00 )
	       | ( DTS_flag ? 0x40 : 0x00 );
	*buf++ = headerLength;

	if ( PTS_flag ) {
		PTS.encode_4_3_1_15_1_15_1( buf );
		buf[ 0 ] |= 0x01;
		buf[ 2 ] |= 0x01;
		buf[ 4 ] |= 0x01;
		if ( DTS_flag ) {
			buf[ 0 ] |= 0x30;
			buf += 5;
			DTS.encode_4_3_1_15_1_15_1( buf );
			buf[ 0 ] |= 0x11;
			buf[ 2 ] |= 0x01;
			buf[ 4 ] |= 0x01;
			buf += 5;
		}
		else {
			buf[ 0 ] |= 0x20;
			buf += 5;
		}
	}

	System::memcpy(
		buf,
		data_byte.getCstPtr(),
		data_byte.getLength() );

	return packetDump;

}

// ============================================================================

void M2S::PESPacket::forceReady() {

	if ( packetDump.getLength() >= 6
	  && packetLength
	  && packetLength != packetDump.getLength() ) {
		throw InternalError();
	}

	packetReady = true;
	decodeFields();

}

// ============================================================================

Buffer M2S::PESPacket::toBuffer() const {

	Buffer res;

	res = "id: " + Buffer( streamType, Buffer::Base16, 2 );

	if ( PTS_flag ) {
//		res += ", PTS: " + PTS.toClock().dateTimeToBuffer( false, true );
		res += ", PTS: " + PTS.toBuffer();
	}

	if ( DTS_flag ) {
//		res += ", DTS: " + DTS.toClock().dateTimeToBuffer( false, true );
		res += ", DTS: " + DTS.toBuffer();
	}

	res += ", " + Utils::toHex( data_byte( 0, 256 ) );

	return res;

}

// ============================================================================

void M2S::PESPacket::decodeFields() {

	if ( streamType == PaddingStream ) {
// FIXME: should check that everything is 0xFF...
		return;
	}

	if ( streamType == PrivateStream2
	  || streamType == ProgramStreamMap
	  || streamType == ECMStream
	  || streamType == EMMStream
	  || streamType == ProgramStreamDirectory
	  || streamType == DSMCCStream
	  || streamType == H222TypeE ) {
		if ( ! isMPEG2 && streamType != PrivateStream2 ) {
			throw InvalidArgument( "stream_id not allowed in MPEG-1!" );
		}
		data_byte = packetDump( 6 );
		return;
	}

	Uint32		len = packetDump.getLength();
	const Uchar *	ptr = packetDump.getCstPtr();

	if ( isMPEG2 ) {

		if ( len < 9 ) {
			throw InternalError( "Can't read header fields! Stream_id: "
				+ Buffer( streamType, Buffer::Base16, 2 )
				+ ", len: "
				+ Buffer( len ) );
		}

		Uchar c = ptr[ 6 ];

		PES_scrambling_control	= ( ( c & 0x30 ) >> 4 );
		PES_priority		= ( ( c & 0x08 ) != 0 );
		data_align_indicator	= ( ( c & 0x04 ) != 0 );
		copyright		= ( ( c & 0x02 ) != 0 );
		original_or_copy	= ( ( c & 0x01 ) != 0 );

		c = ptr[ 7 ];

		PTS_flag		= ( ( c & 0x80 ) != 0 );
		DTS_flag		= ( ( c & 0x40 ) != 0 );
		ESCR_flag		= ( ( c & 0x20 ) != 0 );
		ES_rate_flag		= ( ( c & 0x10 ) != 0 );
		DSM_trick_mode_flag	= ( ( c & 0x08 ) != 0 );
		additional_copy_info	= ( ( c & 0x04 ) != 0 );
		PES_CRC_flag		= ( ( c & 0x02 ) != 0 );
		PES_extension_flag	= ( ( c & 0x01 ) != 0 );

		headerLength = ptr[ 8 ];

		if ( 6 + 3 + headerLength >= packetDump.getLength() ) {
			throw InternalError( "Not enough to read flags! Stream_id: "
				+ Buffer( streamType, Buffer::Base16, 2 )
				+ ", header_len: "
				+ Buffer( headerLength )
				+ ", packet_len: "
				+ Buffer( packetDump.getLength() ) );
		}

		if ( ! PES_extension_flag ) {

			Uint32 headerNeeded
				= ( PTS_flag && ! DTS_flag ? 5 : 0 )
				+ ( PTS_flag && DTS_flag ? 10 : 0 )
				+ ( ESCR_flag ? 6 : 0 )
				+ ( ES_rate_flag ? 3 : 0 )
				+ ( DSM_trick_mode_flag ? 1 : 0 )
				+ ( additional_copy_info ? 1 : 0 )
				+ ( PES_CRC_flag ? 2 : 0 );

			if ( headerNeeded > headerLength ) {
				throw InternalError( "Can't read flags!" );
			}

		}

		if ( PTS_flag ) {
			PTS.decode_4_3_1_15_1_15_1( ptr + 9 );
			if ( DTS_flag ) {
				DTS.decode_4_3_1_15_1_15_1( ptr + 14 );
			}
		}

//		Buffer msg = "PES flags: ";
//		msg += Buffer( streamType, Buffer::Base16, 2 );

//		if ( PTS_flag )			msg += " PTS[" + PTS.toTime::Clock().toBuffer() + "]";
//		if ( DTS_flag )			msg += " DTS[" + DTS.toTime::Clock().toBuffer() + "]";
//		if ( PES_scrambling_control )	msg += " scrb";
//		if ( PES_priority )		msg += " prio";
//		if ( data_align_indicator )	msg += " align";
//		if ( copyright )		msg += " (c)";
//		if ( original_or_copy )		msg += " orig";
//		if ( ESCR_flag )		msg += " ESCR";
//		if ( ES_rate_flag )		msg += " ES_rate";
//		if ( DSM_trick_mode_flag )	msg += " trick";
//		if ( additional_copy_info )	msg += " copy_info";
//		if ( PES_CRC_flag )		msg += " CRC";
//		if ( PES_extension_flag )	msg += " ext";

//		msg += " hdr_len:";
//		msg += Buffer( headerLength );

		data_byte = packetDump( 6 + 3 + headerLength );

//		msg += " len:";
//		msg += Buffer( data_byte.getLength() );

//		msgDbg( msg );

	}
	else { // MPEG-1

		len -= 6;
		ptr += 6;

		while ( len && *ptr == 0xFF ) {
			ptr++;
			len--;
		}

		if ( ! len ) {
			throw InternalError( "Invalid MPEG-1 header!" );
		}

		if ( ( *ptr & 0xC0 ) == 0x40 ) {
			if ( len < 2 ) {
				throw InternalError( "Invalid MPEG-1 STD buffer field!" );
			}
			ptr += 2;
			len -= 2;
		}

		if ( ! len ) {
			throw InternalError( "Can't read PTS or DTS!" );
		}

		if ( ( *ptr & 0xF0 ) == 0x20 ) {
			if ( len < 5 ) {
				throw InternalError( "Can't read PTS!" );
			}
			PTS_flag = true;
			DTS_flag = false;
			PTS.decode_4_3_1_15_1_15_1( ptr );
			ptr += 5;
			len -= 5;
		}
		else if ( ( *ptr & 0xF0 ) == 0x30 ) {
			if ( len < 10 ) {
				throw InternalError( "Can't read PTS and DTS!" );
			}
			PTS_flag = true;
			DTS_flag = true;
			PTS.decode_4_3_1_15_1_15_1( ptr );
			DTS.decode_4_3_1_15_1_15_1( ptr + 5 );
			ptr += 10;
			len -= 10;
		}
		else if ( *ptr == 0x0F ) {
			PTS_flag = false;
			DTS_flag = false;
			ptr++;
			len--;
		}
		else {
			throw InvalidArgument( "Invalid PTS|DTS flags!" );
		}

		if ( len ) {
			data_byte = Buffer( ptr, len );
		}

	}

}

// ============================================================================

