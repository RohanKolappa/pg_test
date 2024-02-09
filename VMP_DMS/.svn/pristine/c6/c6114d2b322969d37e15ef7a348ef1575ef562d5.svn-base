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
//	VMP.RTP.TAVCHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"
#include "BFC.Base.Utils.h"

#include "BFC.Base64.Decoder.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.NALU.h"

#include "VMP.RTP.TAVCDecoderEngine.h"
#include "VMP.RTP.TAVCEncoderEngine.h"
#include "VMP.RTP.TAVCHandler.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TAVCHandler )

// ============================================================================

RTP::TAVCHandler::TAVCHandler() {

	setObjectName( "VMP.RTP.TAVCHandler" );

}

// ============================================================================

void RTP::TAVCHandler::decodeMimeType(
	const	BufferArray &	pMimeType ) {

	if ( pMimeType.getSize() != 2
	  || pMimeType[ 0 ].toLower() != "h264"
	  || pMimeType[ 1 ] != "90000" ) {
		throw InternalError();
	}

}

void RTP::TAVCHandler::decodeFormat(
	const	Buffer &	pFmtpItem ) {

//	msgDbg( "... interpreting: " + pFmtpItem );

	Uint32		p = pFmtpItem.findFwd( '=' );
	Buffer		k = pFmtpItem( 0, p );
	Buffer		v = pFmtpItem( p + 1 );

	if ( k == "packetization-mode" ) {
		packMode = v.toUint32();
	}
	else if ( k == "profile-level-id" ) {
		profLevl = v;
	}
	else if ( k == "sprop-parameter-sets" ) {
		AVC::CodecConfigPtr c = new AVC::CodecConfig;
		Base64::DecoderPtr d = new Base64::Decoder;
		BufferArray t = BufferParser::tokenize( v, "," );
		for ( Uint32 i = 0 ; i < t.getSize() ; i++ ) {
//			msgDbg( "sprop: " + t[ i ] );
			v = d->decode( t[ i ] );
//			msgDbg( "sprop: " + Utils::toHex( v ) );
			AVC::NALUCPtr n = new AVC::NALU( v );
			if ( n->isSPS() ) {
				c->addSPS( n );
			}
			else if ( n->isPPS() ) {
				c->addPPS( n );
			}
		}
		cnfgRcrd = c;
//		msgDbg( "decodeFormat(): DCR: " + cnfgRcrd->toBuffer() );
//		msgDbg( "decodeFormat(): DCR: " + Utils::toHex( cnfgRcrd->encode() ) );
	}
// FIXME: do something with the following parameters too!
	else if ( k == "width"
	       || k == "height"
	       || k == "depth"
	       || k == "framerate"
	       || k == "fieldrate" ) {
	}
	else {
		msgWrn( "decodeFormat(): unsupported parameter \"" + k + "\"!" );
	}

}

// ============================================================================

Uint32 RTP::TAVCHandler::getTimeScale() const {

	return 90000;

}

// ============================================================================

RTP::TEStreamDecoderEnginePtr RTP::TAVCHandler::makeTEStreamDecoderEngine() const {

	return new TAVCDecoderEngine( getSEStream(), packMode, profLevl, cnfgRcrd );

}

RTP::TEStreamEncoderEnginePtr RTP::TAVCHandler::makeTEStreamEncoderEngine() const {

	return new TAVCEncoderEngine;

}

// ============================================================================

