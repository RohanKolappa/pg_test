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
//	VMP.M2S.PSDecoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.GPC.Consumer.h"

#include "VMP.M2S.PESPacket.h"
#include "VMP.M2S.PSDecoder.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PSDecoder )

// ============================================================================

M2S::PSDecoder::PSDecoder() {

	setObjectName( "VMP.M2S.PSDecoder" );

	reset();

	ptr = 0;
	len = 0;

}

// ============================================================================

void M2S::PSDecoder::attachVideoConsumer(
		GPC::ConsumerPtr
				consumer ) {

	vconsumer = consumer;

}

void M2S::PSDecoder::attachAudioConsumer(
		GPC::ConsumerPtr
				consumer ) {

	aconsumer = consumer;

}

// ============================================================================

void M2S::PSDecoder::consume(
	const	Buffer&		payload ) {

	if ( len ) {
		cache = cache( cache.length() - len );
	}
	else {
		cache.kill();
	}

	cache += payload;
	ptr = cache.getBufferAddr();
	len = cache.length();

//	msgDbg( "consume: len == " + Buffer( len ) );

	while ( len >= 4 ) {
		// Skip start code...
		if ( ptr[ 0 ] != 0x00
		  || ptr[ 1 ] != 0x00
		  || ptr[ 2 ] != 0x01 ) {
			throw InternalError( "Invalid start code!" );
		}
		Uint32 tag = ptr[ 3 ];
		Uint32 orig_len = len;
		if ( tag == aid ) {
			PESPacketPtr p = decodePESPacket();
			if ( p ) {
//				if ( p->PTS_flag ) {
//					Time::Clock at = p->PTS.toTime::Clock();
//					Uint32 pval = p->PTS.val;
//					Uint32 pext = p->PTS.ext;
//					msgDbg( "stream_id[]: "
//						+ Buffer( tag, Buffer::Base16, 2 )
//						+ ", PTS: "
//						+ at.toBuffer()
//						+ " (val:"
//						+ Buffer( pval, Buffer::Base16, 8 )
//						+ ",ext:"
//						+ Buffer( pext, Buffer::Base16, 4 )
//						+ ")" );
//				}
				if ( ! p->packetReady ) {
					throw InternalError( "PES packet not ready!" );
				}
				if ( aconsumer ) {
					aconsumer->putObject( p );
				}
			}
		}
		else if ( tag == vid ) {
			PESPacketPtr p = decodePESPacket();
			if ( p ) {
//				if ( p->PTS_flag ) {
//					Time::Clock at = p->PTS.toTime::Clock();
//					Uint32 pval = p->PTS.val;
//					Uint32 pext = p->PTS.ext;
//					msgDbg( "stream_id[]: "
//						+ Buffer( tag, Buffer::Base16, 2 )
//						+ ", PTS: "
//						+ at.toBuffer()
//						+ " (val:"
//						+ Buffer( pval, Buffer::Base16, 8 )
//						+ ",ext:"
//						+ Buffer( pext, Buffer::Base16, 4 )
//						+ ")" );
//				}
//				else {
//					msgDbg( "stream_id[]: "
//						+ Buffer( tag, Buffer::Base16, 2 )
//						+ ", NO PTS" );
//				}
				if ( ! p->packetReady ) {
					throw InternalError( "PES packet not ready!" );
				}
				if ( vconsumer ) {
					vconsumer->putObject( p );
				}
			}
		}
		else if ( tag == 0xB9 ) {
			decodeEndCode();
		}
		else if ( tag == 0xBA ) {
			decodePackHeader();
		}
		else if ( tag == 0xBB ) {
			decodeSystemHeader();
		}
		else if ( tag >= 0xBC && 0xFF ) {
			if ( ! announced[ tag ] ) {
				announced[ tag ] = true;
				msgWrn( "Skpping stream_id ["
					+ Buffer( tag, Buffer::Base16, 2 )
					+ "] (\""
					+ Buffer( getStreamName( tag ) )
					+ "\")" );
			}
			decodePESPacket();
		}
		else {
			msgWrn( "Don't know what to do with tag "
				+ Buffer( tag, Buffer::Base16, 2 ) );
			len -= 4;
			ptr += 4;
		}
		if ( orig_len == len ) {
			return;
		}
	}

}

// ============================================================================

void M2S::PSDecoder::decodePackHeader() {

//	msgDbg( "decodePackHeader()" );

	if ( len < 12 ) {
		return;
	}

	Mode tmp_mode;

	if ( ( ptr[ 4 ] & 0xC0 ) == 0x40 ) {
		tmp_mode = MPEG2Mode;
	}
	else if ( ( ptr[ 4 ] & 0xF0 ) == 0x20 ) {
		tmp_mode = MPEG1Mode;
	}
	else {
		throw InternalError( "Unsupported MPEG mode!" );
	}

	if ( mode != NoMode ) {
		if ( mode != tmp_mode ) {
			throw InternalError( "Mixing MPEG1 & MPEG2 modes!" );
		}
	}
	else {
		mode = tmp_mode;
		if ( mode == MPEG1Mode ) {
			msgDbg( "Found MPEG1 system stream!" );
		}
		else {
			msgDbg( "Found MPEG2 program stream!" );
		}
	}

	if ( mode == MPEG1Mode ) { // MPEG1 mode!

		if ( len < 12 ) {
			return;
		}

		len -= 12;
		ptr += 12;

	}

	else { // MPEG2 mode!

		if ( len < 14 ) {
			return;
		}

		TimeStamp SCR;

		SCR.decode_2_3_1_15_1_15_1_9_1( ptr + 4 );

		msgDbg( "... SCR: " + SCR.toClock().toBuffer() );

		Uint32 mux_rate;

		mux_rate = ( (Uint32)( ptr[ 10 ]        ) << 14 )
		         | ( (Uint32)( ptr[ 11 ]        ) <<  6 )
		         | ( (Uint32)( ptr[ 12 ] & 0xFC ) >> 2 );

		msgDbg( "... mux rate: "
			+ Buffer( mux_rate * 50 )
			+ " bytes/sec" );

		Uint32 stuffing = ( ptr[ 13 ] & 0x07 );

		if ( stuffing ) {
			msgWrn( "... using stuffing ("
				+ Buffer( stuffing )
				+ " bytes)!" );
			for ( Uint32 i = 0 ; i < stuffing ; i++ ) {
				if ( ptr[ 14 + i ] != 0xFF ) {
					throw InternalError(
						"Invalid stuffing byte "
						+ Buffer( i )
						+ " in pack header!" );
				}
			}
		}

		if ( len < 14 + stuffing ) {
			return;
		}

		stuffing += 14;

//		msgDbg( "... consuming " + Buffer( stuffing ) + " bytes" );

		len -= stuffing;
		ptr += stuffing;

	}

}

void M2S::PSDecoder::decodeSystemHeader() {

	msgDbg( "Found system header!" );

	if ( mode == NoMode ) {
		throw InternalError( "SystemHeader without previous PackHeader!" );
	}

	if ( len < 6 ) {
		return;
	}

	Uint32 header_len = ( (Uint32)( ptr[ 4 ] ) << 8 )
	                  | ( (Uint32)( ptr[ 5 ] )      );

	if ( len < 6 + header_len ) {
		return;
	}

	if ( header_len < 6 ) {
		throw InternalError( "SystemHeader with header_len < 6!" );
	}

	if ( header_len % 3 ) {
		throw InternalError( "SystemHeader with header_len % 3!" );
	}

	Uint32 loop = ( header_len - 6 ) / 3;

	len -= 12;
	ptr += 12;

	while ( loop-- ) {
		Uint32 stream_id = ptr[ 0 ];
		msgDbg( "... stream_id: "
			+ Buffer( stream_id, Buffer::Base16, 2 ) );
		if ( stream_id >= 0xC0 && stream_id <= 0xDF ) {
			if ( aid == InvalidId ) {
//				msgDbg( "Found main audio stream: " +
//					Buffer( stream_id, Buffer::Base16, 2 ) );
				aid = stream_id;
			}
			else {
//				msgDbg( "Found aux. audio stream: " +
//					Buffer( stream_id, Buffer::Base16, 2 ) );
			}
		}
		else if ( stream_id >= 0xE0 && stream_id <= 0xEF ) {
			if ( vid == InvalidId ) {
//				msgDbg( "Found main video stream: " +
//					Buffer( stream_id, Buffer::Base16, 2 ) );
				vid = stream_id;
			}
			else {
//				msgDbg( "Found aux. video stream: " +
//					Buffer( stream_id, Buffer::Base16, 2 ) );
			}
		}
		len -= 3;
		ptr += 3;
	}

}

void M2S::PSDecoder::decodeEndCode() {

//	msgDbg( "decodeEndCode()" );

	if ( len < 4 ) {
		return;
	}

	reset();

	len -= 4;
	ptr += 4;

}

M2S::PESPacketPtr M2S::PSDecoder::decodePESPacket() {

//	msgDbg( "decodePESPacket()" );

	PESPacketPtr res;

	if ( len < 6 ) {
		return res;
	}

	Uint32 packet_len = ( (Uint32)( ptr[ 4 ] ) << 8 )
	                  | ( (Uint32)( ptr[ 5 ] )      );

	if ( ! packet_len ) {
		throw InternalError( "PES packet_len == 0!" );
	}

	if ( len < 6 + packet_len ) {
		return res;
	}

	res = new PESPacket( mode == MPEG2Mode );

	res->decode( Buffer( ptr, 6 + packet_len ) );

	packet_len += 6;

	len -= packet_len;
	ptr += packet_len;

	return res;

}

// ============================================================================

void M2S::PSDecoder::reset() {

	vid = InvalidId;
	aid = InvalidId;

	mode = NoMode;

	vconsumer.kill();
	aconsumer.kill();

	for ( Uint32 i = 0 ; i < 0x100 ; i++ ) {
		announced[ i ] = false;
	}

}

// ============================================================================

const char *M2S::PSDecoder::getStreamName(
	const	Uint32		tag ) {

	if ( tag == 0xBC ) return "program_stream_map";
	if ( tag == 0xBD ) return "private_stream_1";
	if ( tag == 0xBE ) return "padding_stream";
	if ( tag == 0xBF ) return "private_stream_2";
	if ( tag >= 0xC0 && tag <= 0xDF ) return "audio_stream";
	if ( tag >= 0xE0 && tag <= 0xEF ) return "video_stream";
	if ( tag == 0xFF ) return "program_stream_directory";

	return "???";

}

// ============================================================================

