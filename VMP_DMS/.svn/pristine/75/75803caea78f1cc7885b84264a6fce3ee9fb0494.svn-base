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
//	VMP.RTP.TRepository.cpp
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

#include "VMP.AudioStream.h"
#include "VMP.VideoStream.h"

#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.TAVCEncoderEngine.h"
#include "VMP.RTP.TAVCHandler.h"
#include "VMP.RTP.TL16Handler.h"
#include "VMP.RTP.TLATMHandler.h"
#include "VMP.RTP.TMPEG4GenericEncoder.h"
#include "VMP.RTP.TMPEG4GenericHandler.h"
#include "VMP.RTP.TRepository.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TRepository )

// ============================================================================

RTP::TRepository::TRepository() {

	setObjectName( "VMP.RTP.TRepository" );

}

// ============================================================================

RTP::TRepositoryPtr RTP::TRepository::instance() {

	static TRepositoryPtr i = new TRepository;

	return i;

}

// ============================================================================

RTP::THandlerPtr RTP::TRepository::makeTHandler(
		SEStreamCPtr	pCurrStrm ) const {

	THandlerPtr res;

	if ( pCurrStrm->getDataType() < 96 ) {
		return res;
	}

	Buffer		metaType = pCurrStrm->getMetaType();	// "video", ...
	Buffer		mimeType = pCurrStrm->getEncoding();	// "MP4V-ES/90000", ...

	BufferArray	itemList = BufferParser::tokenize( mimeType, "/" );
	Buffer		codcName = itemList[ 0 ].toLower();

	if ( metaType == "audio" ) {
		if ( codcName == "l16" ) {
			res = new TL16Handler;
		}
		else if ( codcName == "mp4a-latm" ) {
			res = new TLATMHandler;
		}
		else if ( codcName == "mpeg4-generic" ) {
			res = new TMPEG4GenericHandler;
		}
	}
	else if ( metaType == "video" ) {
		if ( codcName == "h264" ) {
			res = new TAVCHandler;
		}
	}
	else if ( metaType == "application"
	       || metaType == "data"
	       || metaType == "control" ) {
	}
	else {
		throw InternalError( "Unsupported metatype: \"" + metaType + "\"!" );
	}

	if ( res ) {
		res->setSEStream( pCurrStrm );
		res->decodeMimeType( itemList );
		if ( pCurrStrm->hasFormat() ) {
			itemList = BufferParser::tokenize( pCurrStrm->getFormat(), "; " ) ;
			for ( Uint32 i = 0 ; i < itemList.getSize() ; i++ ) {
				res->decodeFormat( itemList[ i ] );
			}
		}
	}

	return res;

}

RTP::TEStreamEncoderEnginePtr RTP::TRepository::makeTEStreamEncoderEngine(
		StreamCPtr	pInptStrm ) const {

	TEStreamEncoderEnginePtr res;

	if ( pInptStrm->inherits( AudioStream::getClassType() ) ) {
		AudioStreamCPtr	pIAudStrm = pInptStrm;
		if ( pIAudStrm->getCompression() == AudioCompression::AAC ) {
			res = new TMPEG4GenericEncoder;
		}
	}
	else if ( pInptStrm->inherits( VideoStream::getClassType() ) ) {
		VideoStreamCPtr	pIVidStrm = pInptStrm;
		if ( pIVidStrm->getCompression() == VideoCompression::H264 ) {
			res = new TAVCEncoderEngine;
		}
	}

	if ( ! res ) {
		throw InternalError( "Unsupported stream!" );
	}

//	if ( res ) {
//		res->setSEStream( pInptStr );
//		res->decodeMimeType( itemList );
//		if ( pInptStr->hasFormat() ) {
//			itemList = BufferParser::tokenize( pInptStr->getFormat(), "; " ) ;
//			for ( Uint32 i = 0 ; i < itemList.getSize() ; i++ ) {
//				res->decodeFormat( itemList[ i ] );
//			}
//		}
//	}

	return res;

}

// ============================================================================

