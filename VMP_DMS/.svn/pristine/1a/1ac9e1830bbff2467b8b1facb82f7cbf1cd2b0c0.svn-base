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
//	VMP.RTP.SDecoder.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SConnInfo.h"
#include "VMP.RTP.SDecoder.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.SOrigin.h"
#include "VMP.RTP.SProgram.h"
#include "VMP.RTP.STimeInfo.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, SDecoder )

// ============================================================================

RTP::SDecoder::SDecoder() {

	setObjectName( "VMP.RTP.SDecoder" );

}

// ============================================================================

RTP::SProgramPtr RTP::SDecoder::decodeSDP(
	const	Buffer &	pSDPData ) {

	currPrgr = new SProgram;
	currStrm.kill();

	BufferArray	lines = BufferParser::tokenize( pSDPData, "\r\n" );

	for ( Uint32 i = 0 ; i < lines.getSize() ; i++ ) {
		decodeLine( lines[ i ] );
	}

	SProgramPtr	res = currPrgr;

	currPrgr.kill();
	currStrm.kill();

	return res;

}

// ============================================================================

void RTP::SDecoder::decodeLine(
	const	Buffer &	line ) {

//	msgDbg( "decodeLine(): \"" + line + "\"" );

	if ( line.isEmpty() ) {
		return;
	}

	if ( line.getLength() < 2 ) {
		throw InternalError( "Invalid short SDP line: \"" + line + "\"!" );
	}

	if ( line[ 1 ] != '=' ) {
		throw InternalError( "Missing '=' in SDP line: \"" + line + "\"!" );
	}

	Uchar		lineCmnd = line[ 0 ];
	Buffer		lineData = line( 2 );

	switch ( lineCmnd ) {

	case 'v' :

		// Protocol version (mandatory).
		// ---> v=0

		if ( lineData != "0" ) {
			msgWrn( "Spurious SDP version: \"" + lineData + "\"" );
		}

		break;

	case 'o' : {

		// Owner/creator and session identifier (mandatory).
		// ---> o=<username> <session id> <version> <network type>
		//		<address type> <address>

		SOriginPtr	o = new SOrigin;

		o->decode( lineData );

		currPrgr->setOrigin( o );

		break;

	}

	case 's' :

		// Session name (mandatory).
		// ---> s=<session name>

		currPrgr->setTitle( lineData );

		break;

	case 'i' :

		// Session or media information (optional).
		// ---> i=<session description>

		if ( currStrm ) {
			currStrm->setEStreamInfo( lineData );
		}
		else {
			currPrgr->setProgramInfo( lineData );
		}

		break;

	case 'u' :

		// URI of description (optional).
		// ---> u=<URI>

		currPrgr->setDescriptionURI( lineData );

		break;

	case 'e' :

		// E-mail address (optional).
		// ---> e=<email address>

		currPrgr->setEmail( lineData );

		break;

	case 'p' :

		// Phone number (optional).
		// ---> p=<phone number>

		currPrgr->setPhone( lineData );

		break;

	case 'c' : {

		// Connection information (optional).
		// ---> c=<network type> <address type> <connection address>

		SConnInfoPtr	c = new SConnInfo;

		c->decode( lineData );

		if ( currStrm ) {
			currStrm->setConnectionInfo( c );
		}
		else {
			currPrgr->setConnectionInfo( c );
		}

		break;

	}

	case 'b' : {

		// Bandwidth information (optional).
		// ---> b=<modifier>:<bandwidth-value>

		SBandwidthPtr	b = new SBandwidth;

		b->decode( lineData );

		if ( currStrm ) {
			currStrm->setBandwidth( b );
		}
		else {
			currPrgr->setBandwidth( b );
		}

		break;

	}

	case 't' : {

		// Time the session is active (mandatory).
		// ---> t=<start time> <stop time>

		STimeInfoPtr	t = new STimeInfo;

		t->decode( lineData );

		currPrgr->setTimeInfo( t );

		break;

	}

	case 'r' :

		// Repeat times (optional).
		// ---> r=<repeat interval> <active duration>
		//		<list of offsets from start-time>

		msgWrn( "decodeLine(): unhandled 'r' line!" );

		break;

	case 'z' :

		// Time zone adjustments (optional).
		// ---> z=<adjustment time> <offset> <adjustement time> ...

		msgWrn( "decodeLine(): unhandled 'z' line!" );

		break;

	case 'k' :

		// Encryption key (optional).
		// ---> k=<method>
		// ---> k=<method>:<encryption key>

		msgWrn( "decodeLine(): unhandled 'k' line!" );

		break;

	case 'a' :

		// Session or media attribute (optional).
		// ---> a=<attribute>
		// ---> a=<attribute>:<value>

		decodeALine( lineData );

		break;

	case 'm' : {

		// Media name and transport address.
		// ---> m=<media> <port> <transport> <fmt> [ <fmt> ... ]

		BufferArray itemList = BufferParser::tokenize( lineData, " " );

		if ( itemList.getSize() != 4 ) {
			throw InternalError( lineData );
		}

		Buffer	metaType = itemList[ 0 ];
		Uint16	strmPort;
		Uint32	nmbrPort;
		Buffer	portBuff = itemList[ 1 ];
		Uint32	nmbrIndx = portBuff.findFwd( '/' );
		if ( nmbrIndx >= portBuff.getLength() ) {
			strmPort = portBuff.toUint16();
			nmbrPort = 1;
		}
		else {
			strmPort = portBuff( 0, nmbrIndx ).toUint16();
			nmbrPort = portBuff( nmbrIndx + 1 ).toUint32();
		}
		Buffer	strmPrfl = itemList[ 2 ];
		Uint32	dataType = itemList[ 3 ].toUint32();

		currStrm = new SEStream( metaType, strmPort, nmbrPort, strmPrfl, dataType );

		currPrgr->addSEStream( currStrm );

		break;

	}

	default :

		msgWrn( "decodeLine(): unknown command \"" + line + "\"!" );

		break;

	}

}

// ============================================================================

void RTP::SDecoder::decodeALine(
	const	Buffer &	content ) {

	if ( content.beginsWith( "rtpmap:" ) ) {

		// ---> rtpmap:<payload type> <encoding name>/<clock ...
		//		... rate>[/<encoding parameters>]

		Buffer rtpmap = content( 7 ); // "96 MP4V-ES/90000"

		BufferArray itemList = BufferParser::tokenize( rtpmap, " " );

		if ( itemList.getSize() != 2 ) {
			msgWrn( "decodeALine(): unsupported 'a=rtpmap' line \"" + content + "\"!" );
			return;
		}

		Uint32 dataType = itemList[ 0 ].toUint32();

		if ( ! currStrm || currStrm->getDataType() != dataType ) {
			msgWrn( "decodeALine(): can't find estream for 'a=rtpmap' line \""
				+ content + "\"!" );
			return;
		}

		currStrm->setEncoding( itemList[ 1 ] );

	}

	else if ( content.beginsWith( "fmtp:" ) ) {

		// ---> fmtp:<format> <format specific parameters>
		// Ex: fmtp:96 profile-level-id=245; config=000001B0F5000001B50900000100000001200088401928B02240A21F;

		Buffer	strmFmtp = content( 5 );
		Uint32	fmtpOffs = 0;
		Uint32	dataType = strmFmtp.getUintAt( fmtpOffs );

		if ( ! fmtpOffs ) {
			msgWrn( "decodeALine(): unsupported 'a=fmtp' line \"" + content + "\"!" );
			return;
		}

		if ( ! currStrm || currStrm->getDataType() != dataType ) {
			throw InternalError( "Orphan 'a=fmtp' line!" );
		}

		if ( fmtpOffs + 1 < strmFmtp.getLength() ) {
			currStrm->setFormat( strmFmtp( fmtpOffs + 1 ) );
		}

	}

	else if ( content.beginsWith( "framerate:" ) ) {

		if ( ! currStrm ) {
			msgWrn( "decodeALine(): skipping 'a=framerate' because in session part!" );
			return;
		}

		if ( currStrm->getMetaType() != "video" ) {
			msgWrn( "decodeALine(): skipping 'a=framerate' because not for a video stream!" );
			return;
		}

		Double d = content( 10 ).toDouble();

		if ( d <= 0.0 || d > 1000.0 ) {
//			msgWrn( "decodeALine(): skipping 'a=framerate' because value ("
//				+ Buffer( d ) + ") seems out of range!" );
			return;
		}

		currStrm->setFrameRate( Frac64( d ) );

//		msgDbg( "Framerate: " + Buffer( frmeRate ) + "x" + Buffer( frmeScle ) );

	}

//	else if ( content.beginsWith( "control:" )
//	       || content.beginsWith( "range:" ) ) {
//
//	}

//	else {
//		msgWrn( "decodeALine(): unhandled 'a' line \"" + content + "\"!" );
//	}

}

// ============================================================================

