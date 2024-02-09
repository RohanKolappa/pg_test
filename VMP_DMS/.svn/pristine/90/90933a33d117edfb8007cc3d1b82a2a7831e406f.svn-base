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
//	VMP.M2S.PSDecoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.DataStream.h"
#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L3.BackendRegisterer.h"

#include "VMP.M2S.PESPacket.h"
#include "VMP.M2S.PSDecoderBackend.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PSDecoderBackend )

// ============================================================================

const DL::TypeCPtr & M2S::PSDecoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "a19b7219-2164-48a6-9f78-66635076c62d" ),
		"VMP.M2S.PSDecoderBackend",
		L3::MuxedDecoderBackend::getClassType(),
		new UI::UObjectDescr(
			"MPEG2 PS Decoder",
			"Codecs,System",
			"MPEG2 ProgramStream decoder"
		)
	);

	return i;

}

// ============================================================================

M2S::PSDecoderBackend::PSDecoderBackend() :

	L3::MuxedDecoderBackend( getClassType() ) {

	reset();

}

M2S::PSDecoderBackend::~PSDecoderBackend() {

}

// ============================================================================

Bool M2S::PSDecoderBackend::canDecode(
		BytesSessionPtr		/* pIBSession */ ) {

	return ( getIODevice()->peekBEUint32() == 0x000001BA );

}

// ============================================================================

SessionPtr M2S::PSDecoderBackend::initSession(
		BytesSessionPtr	pIBSession ) {

	try {
		sync( 0x000001BA, 0xFFFFFFFF );
	}
	catch ( IO::EndOfStream & ) {
		throw NotSuitable( "Can't find sync!" );
	}

	msgDbg( "got pack sync!" );

	decodePackHeader();

	// Prepare output session descriptor...

	Session::Flags	flags = pIBSession->getFlags();

	oSession = new MuxedSession( flags );

	oSession->setBitRate( currRate );

	// Populate output session with estream descriptors...

	if ( getIODevice()->peekBEUint32() == 0x000001BB ) {
		decodeSystemHeader();
	}
	else {
		msgWrn( "init(): missing system header!" );
	}

	return oSession;

}

void M2S::PSDecoderBackend::exitSession() {

	reset();

}

FramePtr M2S::PSDecoderBackend::getFrame() {

	try {

		Uint32		wrd;
		PESPacketPtr	pes;
		Uint32		tag;
	
		for (;;) {
	
			wrd = getIODevice()->peekBEUint32();
	
			if ( ( wrd & 0xFFFFFF00 ) != 0x00000100 ) {
				throw EndOfStream( "Lost sync!" );
			}
	
			tag = wrd & 0xFF;
	
			if ( tag >= 0xC0 && tag <= 0xEF ) {
				pes = decodePESPacket();
				if ( stream[ tag ].announced ) {
					break;
				}
				msgWrn( "getFrame(): spooky PES packet!" );
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
			else {
				throw EndOfStream( "Unsupported sync word!" );
			}
	
		}
	
		Buffer		payload = pes->data_byte;
	
		BytesFramePtr	bframe = new BytesFrame;
	
		bframe->setOffset( stream[ tag ].byteOffset );
//		bframe->setTimeStamp( ??? );
		bframe->setData( payload );
		bframe->setUnitOffset( stream[ tag ].unitOffset );
	
		stream[ tag ].byteOffset += payload.getLength();
		stream[ tag ].unitOffset += 1;
	
		MuxedFramePtr res;
	
		res = new MuxedFrame( stream[ tag ].sessionId, bframe );
	
		res->setOffset( nextOffset++ );
	
		return res;

	}
	catch ( IO::EndOfStream & ) {
		throw EndOfStream();
	}
	catch ( IO::BrokenPipe & ) {
		reset();
		throw BrokenPipe();
	}

}

void M2S::PSDecoderBackend::seekFrame(
	const	Uint64		/* pIndex */ ) {

	throw NotSuitable();

}

// ============================================================================

void M2S::PSDecoderBackend::sync(
	const	Uint32		pSyncWord,
	const	Uint32		pSyncMask ) {

	while ( ( getIODevice()->peekBEUint32() & pSyncMask ) != pSyncWord ) {
		getIODevice()->skip( 1 );
	}

}

void M2S::PSDecoderBackend::reset() {

	mode		= NoMode;
	nextOffset	= 0;

	for ( Uint32 i = 0 ; i < 0x100 ; i++ ) {
		stream[ i ].announced = false;
	}

	currRate	= 0;
	fileTick	= Time::Clock();
	wallTick	= Time::Clock();
	tickOffs	= 0;

	oSession.kill();

}

// ============================================================================

void M2S::PSDecoderBackend::decodePackHeader() {

	Time::Clock	newWallTick	= Time::now();
	Uint64		newTickOffs	= getIODevice()->tell();

//	msgDbg( "decodePackHeader()" );

	getIODevice()->skip( 4 );

	Mode tmp_mode;

	Uchar c = getIODevice()->peekUchar();

	if ( ( c & 0xC0 ) == 0x40 ) {
		tmp_mode = MPEG2Mode;
	}
	else if ( ( c & 0xF0 ) == 0x20 ) {
		tmp_mode = MPEG1Mode;
	}
	else {
		throw InvalidArgument( "Unsupported MPEG mode!" );
	}

	if ( mode == NoMode ) {
		mode = tmp_mode;
		if ( mode == MPEG1Mode ) {
			msgDbg( "Found MPEG1 system stream!" );
		}
		else {
			msgDbg( "Found MPEG2 program stream!" );
		}
	}
	else if ( mode != tmp_mode ) {
		throw InvalidArgument( "Mixing MPEG1 & MPEG2 modes!" );
	}

	TimeStamp	SCR;
	Uint32		mux_rate;

	if ( mode == MPEG1Mode ) { // MPEG1 mode!

		Buffer		b = getIODevice()->getBytes( 8 );
		const Uchar *	p = b.getCstPtr();

		SCR.decode_4_3_1_15_1_15_1( p );

		mux_rate = ( ( Uint32 )( p[ 5 ] & 0x7F ) << 15 )
		         | ( ( Uint32 )( p[ 6 ]        ) <<  7 )
		         | ( ( Uint32 )( p[ 7 ] & 0xFE ) >>  1 );

	}

	else { // MPEG2 mode!

		Buffer		b = getIODevice()->getBytes( 10 );
		const Uchar *	p = b.getCstPtr();

		SCR.decode_2_3_1_15_1_15_1_9_1( p );

		mux_rate = ( ( Uint32 )( p[ 6 ]        ) << 14 )
		         | ( ( Uint32 )( p[ 7 ]        ) <<  6 )
		         | ( ( Uint32 )( p[ 8 ] & 0xFC ) >>  2 );

		Uint32 stuffing = ( p[ 9 ] & 0x07 );

		if ( stuffing ) {
			msgWrn( "... using stuffing ("
				+ Buffer( stuffing )
				+ " bytes)!" );
			for ( Uint32 i = 0 ; i < stuffing ; i++ ) {
				if ( getIODevice()->getUchar() != 0xFF ) {
					throw InvalidArgument(
						"Invalid stuffing byte "
						+ Buffer( i )
						+ " in pack header!" );
				}
			}
		}

	}

	mux_rate *= 50;

	Uint32		newCurrRate = mux_rate;
	Time::Clock	newFileTick = SCR.toClock();

	// Validate PCR, mux rate, ...

	if ( newTickOffs <= tickOffs ) {
		msgDbg( "Stats: resync'ing..." );
	}
	else if ( newFileTick <= fileTick ) {
		msgWrn( "Corrupted PCR!\n"
			"... old: " + fileTick.toBuffer() + "\n"
			"... new: " + newFileTick.toBuffer() );
	}
	else if ( ! currRate ) {
		msgWrn( "Missing mux rate!" );
	}
	else {
		Uint64	diffTime = newFileTick - fileTick;
		Uint64	diffOffs = newTickOffs - tickOffs;
		Uint64	theoRate = ( diffOffs * 1000000 ) / diffTime;
		Double	ratio	= ( Double )( theoRate )
				/ ( Double )( currRate );
		msgDbg( "Stats: speed factor: " + Buffer( ratio ) );
	}

	// Update stats...

	currRate	= newCurrRate;
	fileTick	= newFileTick;
	wallTick	= newWallTick;
	tickOffs	= newTickOffs;

	msgDbg( "... SCR     : " + fileTick.toBuffer() + " ("
			+ SCR.toBuffer() + ")" );
	msgDbg( "... mux rate: " + Buffer( currRate ) + " bytes/sec" );

}

void M2S::PSDecoderBackend::decodeSystemHeader() {

	msgDbg( "Found system header!" );

	getIODevice()->skip( 4 );

	if ( mode == NoMode ) {
		throw InvalidArgument( "Spurious SystemHeader!" );
	}

	Uint32 header_len = getIODevice()->getBEUint16();

	if ( header_len < 6 || header_len % 3 ) {
		throw InvalidArgument( "Invalid header_len!" );
	}

	Buffer		b = getIODevice()->getBytes( 6 );
	const Uchar *	p = b.getCstPtr();

	Uint32	rate_bound	= ( ( Uint32 )( p[ 0 ] & 0x7F ) << 15 )
				| ( ( Uint32 )( p[ 1 ]        ) <<  7 )
				| ( ( Uint32 )( p[ 2 ] & 0xFE ) >>  1 );

	msgDbg( "systemHeader: rate_bound: "
		+ Buffer( rate_bound ) );

	Bool	fixed_flag	= ( ( p[ 3 ] & 0x02 ) != 0 );
	Bool	CSPS_flag	= ( ( p[ 3 ] & 0x01 ) != 0 );
	Bool	audio_lock	= ( ( p[ 4 ] & 0x80 ) != 0 );
	Bool	video_lock	= ( ( p[ 4 ] & 0x40 ) != 0 );

	msgDbg( "systemHeader: flags: "
		+ Buffer( fixed_flag ? " fixed" : "" )
		+ Buffer( CSPS_flag ? " CSPS" : "" )
		+ Buffer( audio_lock ? " audio_lock" : "" )
		+ Buffer( video_lock ? " video_lock" : "" ) );

	Uint32	loop = ( header_len - 6 ) / 3;

	msgDbg( "systemHeader: #streams: "
		+ Buffer( loop ) );

	Bool	dupl = ( oSession->getNbrComponents() > 0 );

	if ( dupl && oSession->getNbrComponents() != loop ) {
		throw InvalidArgument( "Mutating # streams!" );
	}

	b = getIODevice()->getBytes( loop * 3 );
	p = b.getCstPtr();

	if ( dupl ) {

		while ( loop-- ) {
			Uint32 stream_id = p[ 0 ];
			if ( ! stream[ stream_id ].announced ) {
				msgWrn( "systemHeader: mutating stream_ids!!!" );
			}
			ElementaryStreamPtr estream = oSession->getMuxedStream()
				->getComponent( stream[ stream_id ].sessionId );
			if ( ( stream_id >= 0xC0
			    && stream_id <= 0xDF
			    && ! estream->inherits( AudioStream::getClassType() ) )
			  || ( stream_id >= 0xE0
			    && stream_id <= 0xEF
			    && ! estream->inherits( VideoStream::getClassType() ) )
			  || ( ! estream->inherits( DataStream::getClassType() ) ) ) {
				msgWrn( "systemHeader: mutating stream type!!!" );
			}
			p += 3;
		}

	}
	else {

		while ( loop-- ) {
			Uint32 stream_id = p[ 0 ];
			if ( stream[ stream_id ].announced ) {
				msgWrn( "systemHeader: duplicate stream_id!!!" );
			}
			else {
				stream[ stream_id ].announced = true;
				stream[ stream_id ].byteOffset = 0;
				stream[ stream_id ].unitOffset = 0;
			}
			ElementaryStreamPtr estream;
			if ( stream_id >= 0xC0 && stream_id <= 0xDF ) {
				msgDbg( "... "
					+ Buffer( stream_id, Buffer::Base16, 2 )
					+ " ---> audio stream" );
				estream = new AudioStream;
			}
			else if ( stream_id >= 0xE0 && stream_id <= 0xEF ) {
				msgDbg( "... "
					+ Buffer( stream_id, Buffer::Base16, 2 )
					+ " ---> video stream" );
				estream = new VideoStream;
			}
			else {
				msgWrn( "... "
					+ Buffer( stream_id, Buffer::Base16, 2 )
					+ " ---> unknown stream" );
				estream = new DataStream;
			}
			estream->setStreamId( stream_id );
			BytesSessionPtr session = new BytesSession( oSession->getFlags() );
			session->setContent( estream );
			stream[ stream_id ].sessionId = oSession->addComponent( session );
			p += 3;
		}

	}

}

void M2S::PSDecoderBackend::decodeEndCode() {

//	msgDbg( "decodeEndCode()" );

	getIODevice()->skip( 4 );

	reset();

}

M2S::PESPacketPtr M2S::PSDecoderBackend::decodePESPacket() {

//	msgDbg( "decodePESPacket()" );

	Buffer		b = getIODevice()->peekBytes( 6 );
	const Uchar *	p = b.getCstPtr();

	Uint32 packet_len = ( ( Uint32 )( p[ 4 ] ) << 8 )
	                  | ( ( Uint32 )( p[ 5 ] )      );

	if ( ! packet_len ) {
		throw InvalidArgument( "PES packet_len == 0!" );
	}

	b = getIODevice()->getBytes( 6 + packet_len );

	PESPacketPtr res = new PESPacket( mode == MPEG2Mode );

	res->decode( b );

	return res;

}

// ============================================================================

const char * M2S::PSDecoderBackend::getStreamName(
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

static L3::BackendRegisterer< M2S::PSDecoderBackend > psDecoderRegisterer;

// ============================================================================

