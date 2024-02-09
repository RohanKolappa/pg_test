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
//	VMP.RTP.TLATMHandler.cpp
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

#include "BFC.TL.BufferArray.h"

#include "VMP.RTP.TLATMContext.h"
#include "VMP.RTP.TLATMDecoderEngine.h"
#include "VMP.RTP.TLATMHandler.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TLATMHandler )

// ============================================================================

RTP::TLATMHandler::TLATMHandler() :

	codcCtxt	( new TLATMContext ) {

	setObjectName( "VMP.RTP.TLATMHandler" );

}

// ============================================================================

void RTP::TLATMHandler::decodeMimeType(
	const	BufferArray &	pMimeType ) {

	if ( pMimeType.getSize() != 2 ) {
		throw InternalError( "Invalid mimetype syntax! Expecting 2 components, but got "
			+ Buffer( pMimeType.getSize() ) + "!" );
	}

	if ( pMimeType[ 0 ].toLower() != "mp4a-latm" ) {
		throw InternalError();
	}

	codcCtxt->smplRate = pMimeType[ 1 ].toUint32();

//	msgDbg( "decodeMimeType(): sampling rate: " + Buffer( codcCtxt->smplRate ) );

}

void RTP::TLATMHandler::decodeFormat(
	const	Buffer &	pFmtpItem ) {

//	msgDbg( "decodeFormat(): interpreting: " + pFmtpItem );

	Uint32		p = pFmtpItem.findFwd( '=' );
	Buffer		k = pFmtpItem( 0, p ).toLower();
	Buffer		v = pFmtpItem( p + 1 );

	if ( k == "config" ) {
		codcCtxt->codcCnfg = v;
	}
//	else {
//		msgWrn( "decodeFmtpItem(): unsupported parameter \"" + k + "\"!" );
//	}

}

// ============================================================================

Uint32 RTP::TLATMHandler::getTimeScale() const {

	return codcCtxt->smplRate;

}

// ============================================================================

RTP::TEStreamDecoderEnginePtr RTP::TLATMHandler::makeTEStreamDecoderEngine() const {

	return new TLATMDecoderEngine( getSEStream(), codcCtxt );

}

RTP::TEStreamEncoderEnginePtr RTP::TLATMHandler::makeTEStreamEncoderEngine() const {

	throw NotImplemented();

//	return new TLATMEncoder( getSEStream(), codcCtxt );

}

// ============================================================================

