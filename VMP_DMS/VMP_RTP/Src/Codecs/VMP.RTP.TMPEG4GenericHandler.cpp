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
//	VMP.RTP.TMPEG4GenericHandler.cpp
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

#include "VMP.RTP.TMPEG4GenericContext.h"
#include "VMP.RTP.TMPEG4GenericDecoderEngine.h"
#include "VMP.RTP.TMPEG4GenericHandler.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TMPEG4GenericHandler )

// ============================================================================

RTP::TMPEG4GenericHandler::TMPEG4GenericHandler() :

	codcCtxt	( new TMPEG4GenericContext ) {

	setObjectName( "VMP.RTP.TMPEG4GenericHandler" );

}

// ============================================================================

void RTP::TMPEG4GenericHandler::decodeMimeType(
	const	BufferArray &	pMimeType ) {

	if ( pMimeType.getSize() != 3 ) {
		throw InternalError( "Invalid mimetype syntax! Expecting 3 components, but got "
			+ Buffer( pMimeType.getSize() ) + "!" );
	}

	if ( pMimeType.getSize() != 3
	  || pMimeType[ 0 ].toLower() != "mpeg4-generic" ) {
		throw InternalError();
	}

	codcCtxt->smplRate = pMimeType[ 1 ].toUint32();
	codcCtxt->nbrChans = pMimeType[ 2 ].toUint32();

//	msgDbg( "decodeMimeType(): sampling rate: " + Buffer( codcCtxt->smplRate )
//		+ ", #chans: " + Buffer( codcCtxt->nbrChans ) );

}

void RTP::TMPEG4GenericHandler::decodeFormat(
	const	Buffer &	pFmtpItem ) {

//	msgDbg( "decodeFormat(): interpreting: " + pFmtpItem );

	Uint32		p = pFmtpItem.findFwd( '=' );
	Buffer		k = pFmtpItem( 0, p ).toLower();
	Buffer		v = pFmtpItem( p + 1 );

	if ( k == "streamtype" ) {
		codcCtxt->strmType = v.toUint32();
	}
	else if ( k == "profile-level-id" ) {
		codcCtxt->profLevl = v.toUint32();
	}
	else if ( k == "mode" ) {
		codcCtxt->strmMode = v;
	}
	else if ( k == "config" ) {
		codcCtxt->codcCnfg = v;
	}
	else if ( k == "sizelength" ) {
		codcCtxt->sizeLgth = v.toUint32();
	}
	else if ( k == "indexlength" ) {
		codcCtxt->indxLgth = v.toUint32();
	}
	else if ( k == "indexdeltalength" ) {
		codcCtxt->dltaLgth = v.toUint32();
	}
//	else if ( k == "profile" ) {
//		throw NotImplemented();
//	}
//	else {
//		msgWrn( "decodeFmtpItem(): unsupported parameter \"" + k + "\"!" );
//	}

}

// ============================================================================

Uint32 RTP::TMPEG4GenericHandler::getTimeScale() const {

	return codcCtxt->smplRate;

}

// ============================================================================

RTP::TEStreamDecoderEnginePtr RTP::TMPEG4GenericHandler::makeTEStreamDecoderEngine() const {

	return new TMPEG4GenericDecoderEngine( getSEStream(), codcCtxt );

}

RTP::TEStreamEncoderEnginePtr RTP::TMPEG4GenericHandler::makeTEStreamEncoderEngine() const {

	throw NotImplemented();

//	return new TMPEG4GenericEncoder( getSEStream(), codcCtxt );

}

// ============================================================================

