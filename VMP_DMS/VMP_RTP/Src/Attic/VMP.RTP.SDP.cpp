// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.SDP.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <math.h>

#include "BFC.Base.BufferIO.h"
#include "BFC.Base.BufferParser.h"
#include "BFC.Base.Utils.h"

#include "VMP.DataStream.h"
#include "VMP.BytesSession.h"
#include "VMP.MuxedStream.h"

#include "VMP.RTP.TAVCHandler.h"
#include "VMP.RTP.SDP.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SDP )

// ============================================================================

RTP::SDP::SDP() {

	setObjectName( "VMP.RTP.SDP" );

}

// ============================================================================

BytesSessionPtr RTP::SDP::decode(
	const	Buffer &	pSDPData ) {

	oBytSess = new BytesSession;
	oMuxStrm = new MuxedStream;

	oBytSess->setContent( oMuxStrm );

	entrList.kill();
	currEntr.kill();

	BufferArray	lines = BufferParser::tokenize( pSDPData, "\r\n" );

	for ( Uint32 i = 0 ; i < lines.getSize() ; i++ ) {
		decodeLine( lines[ i ] );
	}

	return oBytSess;

}

// ============================================================================

void RTP::SDP::decodeLine(
	const	Buffer &	line ) {

//	msgDbg( "decodeLine(): \"" + line + "\"" );

	if ( line.isEmpty() ) {
		return;
	}

	if ( line.getLength() < 2 ) {
		throw InternalError( "len < 2!" );
	}

	if ( line[ 1 ] != '=' ) {
		throw InternalError( "Missing '='!" );
	}

	Uchar	letter = line[ 0 ];
	Buffer	content = line( 2 );

	BufferArray	tokens;

	switch ( letter ) {

	case 'v' :

		// Protocol version (mandatory).
		// ---> v=0

		if ( content != "0" ) {
			msgWrn( "Spurious SDP version \"" + content + "\"" );
		}

		break;

	case 's' :

		// Session name (mandatory).
		// ---> s=<session name>

		oMuxStrm->setTitle( content );

		break;

	case 'i' :

		// Session or media information (optional).
		// ---> i=<session description>

//		if ( currEntr && currEntr->strm ) {
//			currEntr->strm->setTitle( content );
//		}
//		else if ( oMuxStrm->getTitle().isEmpty() ) {
//			oMuxStrm->setTitle( content );
//		}
//		else {
//			oMuxStrm->setTitle(
//				oMuxStrm->getTitle() + " (" + content + ")" );
//		}

		break;

	case 'c' :

		// Connection information (optional).
		// ---> c=<network type> <address type> <connection address>

		tokens = BufferParser::tokenize( content, " " );
		if ( tokens.getSize() != 3
		  || tokens[ 0 ] != "IN"
		  || tokens[ 1 ] != "IP4" ) {
			throw InternalError( line );
		}
		tokens = BufferParser::tokenize( tokens[ 2 ], "/" );
		if ( currEntr ) {
			currEntr->addr = Net::IPAddr::getByName( tokens[ 0 ] );
//			msgDbg( "stream address: " + currEntr->addr->toBuffer() );
		}

		break;

	case 'b' :

		// Bandwidth information (optional).
		// ---> b=<modifier>:<bandwidth-value>

		tokens = BufferParser::tokenize( content, ":" );
		if ( tokens.getSize() != 2 ) {
			throw InternalError( line );
		}
		if ( tokens[ 0 ] == "AS" ) {
			oBytSess->setBitRate( tokens[ 1 ].toUint32() * 1000 );
		}

		break;

	case 'm' :

		// Media name and transport address.
		// ---> m=<media> <port> <transport> <fmt> [ <fmt> ... ]

		decodeMLine( content );

		break;

	case 'a' :

		// Session or media attribute (optional).
		// ---> a=<attribute>
		// ---> a=<attribute>:<value>

		decodeALine( content );

		break;

	case 'o' :

		// Owner/creator and session identifier (mandatory).
		// ---> o=<username> <session id> <version> <network type>
		//		<address type> <address>

	case 'u' :

		// URI of description (optional).
		// ---> u=<URI>

	case 'e' :

		// E-mail address (optional).
		// ---> e=<email address>

	case 'p' :

		// Phone number (optional).
		// ---> p=<phone number>

	case 'z' :

		// Time zone adjustments (optional).
		// ---> z=<adjustment time> <offset> <adjustement time> ...

	case 'k' :

		// Encryption key (optional).
		// ---> k=<method>
		// ---> k=<method>:<encryption key>

	case 't' :

		// Time the session is active (mandatory).
		// ---> t=<start time> <stop time>

	case 'r' :

		// Repeat times (optional).
		// ---> r=<repeat interval> <active duration>
		//		<list of offsets from start-time>

//		msgDbg( "decodeLine(): unhandled command \"" + line + "\"!" );

		break;

	default :

		msgDbg( "decodeLine(): unknown command \"" + line + "\"!" );

		break;

	}

}

// ============================================================================

void RTP::SDP::decodeMLine(
	const	Buffer &	content ) {

	BufferArray tokens = BufferParser::tokenize( content, " " );

	if ( tokens.getSize() != 4 ) {
		throw InternalError( content );
	}

	Buffer		metaType = tokens[ 0 ];
	Uint16		strmPort = tokens[ 1 ].toUint16();
	Buffer		strmPrfl = tokens[ 2 ];
	Uint32		dataType = tokens[ 3 ].toUint32();

	currEntr = new Entry;

	currEntr->meta = metaType;
	currEntr->port = strmPort;
	currEntr->prfl = strmPrfl;
	currEntr->type = dataType;
	currEntr->strm = makeStream( metaType );
	currEntr->phlp = decodeType( metaType, dataType, currEntr->strm );

	entrList.add( dataType, currEntr );

	oMuxStrm->addComponent( currEntr->strm );

}

void RTP::SDP::decodeALine(
	const	Buffer &	content ) {

	if ( content.beginsWith( "rtpmap:" ) ) {

		// ---> rtpmap:<payload type> <encoding name>/<clock ...
		//		... rate>[/<encoding parameters>]

		Buffer rtpmap = content( 7 ); // "96 MP4V-ES/90000"

		BufferArray tokens = BufferParser::tokenize( rtpmap, " " );

		if ( tokens.getSize() != 2 ) {
			msgWrn( "Unsupported 'a' line \"" + content + "\"!" );
			return;
		}

		Uint32 dataType = tokens[ 0 ].toUint32();

		if ( ! entrList.has( dataType ) ) {
			msgWrn( "Can't find estream for 'a' line \""
				+ content + "\"!" );
			return;
		}

		EntryPtr e = entrList[ dataType ];

		if ( e != currEntr ) {
			throw InternalError( "Misplaced 'rtpmap' line!" );
		}

		if ( e->phlp ) {
			throw InternalError( "Already got payload type info!" );
		}

		e->phlp = decodeType( e->meta, tokens[ 1 ], e->strm );
		
	}

	else if ( content.beginsWith( "fmtp:" ) ) {

		// ---> fmtp:<format> <format specific parameters>
		// Ex: fmtp:96 profile-level-id=245; config=000001B0F5000001B50900000100000001200088401928B02240A21F;

		Buffer	fmtp = content( 5 );
		Uint32	offs = 0;
		Uint32	type = fmtp.getUintAt( offs );

		if ( ! offs || offs == fmtp.getLength() ) {
			msgWrn( "Unsupported 'a' line \"" + content + "\"!" );
			return;
		}

		if ( ! entrList.has( type ) ) {
			throw InternalError( "Orphan 'a=fmtp' line!" );
		}

		EntryPtr	entr = entrList[ type ];

		if ( entr != currEntr ) {
			throw InternalError( "Misplaced 'a=fmtp' line!" );
		}

		if ( ! entr->phlp ) {
			msgWrn( "Skipping 'a=fmtp' line because no helper!" );
			return;
		}

		entr->phlp->decodeFmtpLine( fmtp( offs + 1 ) );

	}

	else if ( content.beginsWith( "control:" )
	       || content.beginsWith( "range:" )
	       || content.beginsWith( "mpeg4-iod:" )
	       || content.beginsWith( "mpeg4-esid:" ) ) {

	}

	else if ( content.beginsWith( "framerate:" ) ) {

		if ( ! currEntr ) {
			msgWrn( "Skipping 'a=framerate' because in session part!" );
			return;
		}

		if ( ! currEntr->strm.isA< VideoStream >() ) {
			msgWrn( "Skipping 'a=framerate' because not for a video stream!" );
			return;
		}

		Double	tempRate = content( 10 ).toDouble();
		Uint32	frmeRate = 0;
		Uint32	frmeScle = 1;
		Double	tempDble;

		while ( ::modf( tempRate, &tempDble ) != 0.0 ) {
			tempRate *= 10.0;
			frmeScle *= 10;
		}

		frmeRate = ( Uint32 )tempRate;

		VideoStreamPtr( currEntr->strm )->setFrameRate( frmeRate );
		VideoStreamPtr( currEntr->strm )->setFrameScale( frmeScle );

//		msgDbg( "Framerate: " + Buffer( frmeRate ) + "x" + Buffer( frmeScle ) );

	}

	else {
//		msgWrn( "Unhandled 'a' line \"" + content + "\"!" );
	}

}

// ============================================================================

StreamPtr RTP::SDP::makeStream(
	const	Buffer &	pMetaType ) const {

	if ( pMetaType == "audio" ) {
		return new AudioStream;
	}

	if ( pMetaType == "video" ) {
		return new VideoStream;
	}

	if ( pMetaType == "application"
	  || pMetaType == "data"
	  || pMetaType == "control" ) {
		return new DataStream;
	}

	throw InternalError( "Unsupported metatype: \"" + pMetaType + "\"!" );

}

RTP::THandlerPtr RTP::SDP::decodeType(
	const	Buffer &	/*pMetaType*/,
	const	Uint32		/*pDataType*/,
		StreamPtr	/*pCurrStrm*/ ) const {

	return THandlerPtr();

}

RTP::THandlerPtr RTP::SDP::decodeType(
	const	Buffer &	pMetaType,
	const	Buffer &	pMimeType,
		StreamPtr	pCurrStrm ) const {

	BufferArray	t = BufferParser::tokenize( pMimeType, "/" );
	Uint32		s = t.getSize();
	Buffer		m = t[ 0 ];

	if ( pMetaType == "audio" ) {
	}
	else if ( pMetaType == "video" ) {
		if ( m == "H264" ) {
			if ( s != 2 || t[ 1 ] != "90000" ) {
				throw InternalError( "Unsupported H264 entry: \"" + pMimeType + "\"!" );
			}
			return new TAVCHandler( pCurrStrm );
		}
	}
	else if ( pMetaType == "application"
	       || pMetaType == "data"
	       || pMetaType == "control" ) {
	}
	else {
		throw InternalError( "Unsupported metatype: \"" + pMetaType + "\"!" );
	}

	return THandlerPtr();

}

// ============================================================================

BFC_PTR_IMPL_NS_CC( RTP, SDP, Entry )

// ============================================================================

