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
//	VMP.RTP.TL16DecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"
#include "BFC.Base.Utils.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.MemoryInputStream.h"

#include "BFC.Sound.Format.h"

#include "VMP.AudioCompression.h"
#include "VMP.AudioStream.h"
#include "VMP.BytesSession.h"

#include "VMP.AAC.AudioSpecificConfig.h"

#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.TL16Context.h"
#include "VMP.RTP.TL16DecoderEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TL16DecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TL16DecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9bf655d0-e536-4411-bfae-df1bcc9f0bcc" ),
		"VMP.RTP.TL16DecoderEngine",
		TEStreamDecoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP L16 Decoder",
			"Devices",
			"RTP L16 Decoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TL16DecoderEngine::TL16DecoderEngine(
		SEStreamCPtr	pSEStream,
		TL16ContextCPtr
				pCodcCtxt ) :

	TEStreamDecoderEngine	( getClassType() ),

	sestream	( pSEStream ),
	codcCtxt	( pCodcCtxt ) {

	setNbrOutputs( 1 );
	setOutputType( 0, AudioSession::getClassType() );

}

// ============================================================================

void RTP::TL16DecoderEngine::putSessionCallback(
		SessionPtr	/*pInptSess*/ ) {

	frmeOffs = 0;
	unitOffs = 0;

	oAudFrmt = new Sound::Format(
			codcCtxt->nbrChans,
			codcCtxt->smplRate,
			Sound::Format::S16LE );

	AudioStreamPtr		oAudStrm = new AudioStream( oAudFrmt );

	oAudStrm->setCompression( AudioCompression::Uncompressed );

	AudioSessionPtr		oAudSess = new AudioSession( oAudStrm, Session::Live );

	getPeerEngine( 0 )->putSessionCallback( oAudSess );

}

void RTP::TL16DecoderEngine::endSessionCallback() {

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TL16DecoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	TFramePtr	rtpTFrme = pInptFrme;

	if ( rtpTFrme->PT != sestream->getDataType() ) {
		throw InternalError( "Invalid payload type!" );
	}

	Buffer		pcktData = rtpTFrme->data;

	if ( pcktData.getLength() % 2 ) {
		throw InternalError();
	}

	Uint32		len = pcktData.getLength() / 2;
	Uint16 *	ptr = ( Uint16 * )pcktData.getVarPtr();

	while ( len-- ) {
		*ptr = System::BE2Host( *ptr );
//		*ptr = System::swap( *ptr );
		ptr++;
	}

	AudioFramePtr	oAudFrme = new AudioFrame( rtpTFrme, pcktData, oAudFrmt );

	oAudFrme->setOffset( frmeOffs );
	oAudFrme->setKeyFrame( true );

	frmeOffs += oAudFrme->getLength();
	unitOffs++;

	getPeerEngine( 0 )->putFrameCallback( oAudFrme );

}

void RTP::TL16DecoderEngine::flushSessionCallback() {

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

