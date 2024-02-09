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
//	VMP.RTP.NEStreamDecoderWorker.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "VMP.RTP.NEStreamDecoderWorker.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.TFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NEStreamDecoderWorker )

// ============================================================================

RTP::NEStreamDecoderWorker::NEStreamDecoderWorker() {

	setObjectName( "VMP.RTP.NEStreamDecoderWorker" );

}

// ============================================================================

RTP::TFramePtr RTP::NEStreamDecoderWorker::decodeNFrame(
		NFramePtr	pNetFrame ) {

//	msgDbg( "decodeNFrame(): --->" );

	NFramePtr	netFrame = pNetFrame;

	Buffer		buf = netFrame->getData();
	const Uchar *	ptr = buf.getCstPtr();
	Uint32		len = buf.getLength();

	if ( len < 12 ) {
		throw InternalError( "RTP header length < 12!" );
	}

	Uchar		c0 = ptr[ 0 ];
	Uchar		c1 = ptr[ 1 ];

	if ( ( c0 & 0xC0 ) != 0x80 ) {
		throw InternalError( "RTP not version 2!" );
	}

	if ( c0 & 0x20 ) {
		throw InternalError( "Padding not supported!" );
	}

	if ( c0 & 0x10 ) {
		throw InternalError( "Extension not supported!" );
	}

	Uint32		sizeCSRC	= ( ( Uint32 )( c0 & 0x0F ) << 2 );
	Bool		packMFlg	= ( ( c1 & 0x80 ) != 0 );
	Uint32		packType	= ( c1 & 0x7F );
	Uint16		packSqNb	= System::Host2BE( *( const Uint16 * )( ptr + 2 ) );
	Uint32		timeStmp	= System::Host2BE( *( const Uint32 * )( ptr + 4 ) );
	Uint32		packSSRC	= System::Host2BE( *( const Uint32 * )( ptr + 8 ) );

	len -= 12;
	ptr += 12;

	if ( len < sizeCSRC ) {
		throw InternalError( "RTP header length < 12 + CSRCs!" );
	}

	if ( sizeCSRC ) {
		msgWrn( "getObject(): skipping CSRC's!" );
		len -= sizeCSRC;
		ptr += sizeCSRC;
	}

	TFramePtr	rtpTFrme = new TFrame;

	rtpTFrme->srcSAddr	= netFrame->getSourceAddress();
	rtpTFrme->tgtSAddr	= netFrame->getTargetAddress();
	rtpTFrme->readTime	= netFrame->getTime();
	rtpTFrme->M		= packMFlg;
	rtpTFrme->PT		= packType;
	rtpTFrme->SN		= packSqNb;
	rtpTFrme->TS		= timeStmp;
	rtpTFrme->SSRC		= packSSRC;
	rtpTFrme->data		= buf( buf.getLength() - len );

//	msgWrn( "getTPacket(): " + rtpTFrme->toBuffer() );

	return rtpTFrme;

//	msgDbg( "decodeNFrame(): <---" );

}

// ============================================================================

