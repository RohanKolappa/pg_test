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
//	VMP.RTP.TLATMDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.MemoryInputStream.h"

#include "VMP.AudioCompression.h"
#include "VMP.AudioStream.h"
#include "VMP.BytesSession.h"

#include "VMP.AAC.AudioSpecificConfig.h"

#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.TLATMContext.h"
#include "VMP.RTP.TLATMDecoderEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TLATMDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TLATMDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "fec3207a-baa9-4bf7-b339-338e81ff658e" ),
		"VMP.RTP.TLATMDecoderEngine",
		TEStreamDecoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP LATM Decoder",
			"Devices",
			"RTP LATM Decoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TLATMDecoderEngine::TLATMDecoderEngine(
		SEStreamCPtr	pSEStream,
		TLATMContextCPtr
				pCodcCtxt ) :

	TEStreamDecoderEngine	( getClassType() ),

	sestream	( pSEStream ),
	codcCtxt	( pCodcCtxt ) {

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

}

// ============================================================================

void RTP::TLATMDecoderEngine::putSessionCallback(
		SessionPtr	/*pInptSess*/ ) {

	// Decode AudioSpecificConfig...

	if ( codcCtxt->codcCnfg.isEmpty() ) {
		throw InternalError( "Missing AAC \"config\" parameter!" );
	}

	codcCnfg = new AAC::AudioSpecificConfig;

	codcCnfg->decode( Utils::fromHex( codcCtxt->codcCnfg, false ) );

//	msgDbg( "getSession(): " + codcCnfg->toBuffer() );

	if ( codcCnfg->getSamplingFrequency() != codcCtxt->smplRate ) {
		throw InternalError( "Sampling frequency mismatch!" );
	}

	if ( codcCtxt->nbrChans && codcCnfg->getChannelConfiguration() != codcCtxt->nbrChans ) {
		throw InternalError( "Channel configuration mismatch! Cfg: "
			+ Buffer( codcCnfg->getChannelConfiguration() )
			+ ", ctx: " + Buffer( codcCtxt->nbrChans ) );
	}

	smplRate = codcCtxt->smplRate;

	frmeOffs = 0;
	unitOffs = 0;

	BytesSessionPtr		oBytSess = new BytesSession( Session::Live );
	AudioStreamPtr		oAudStrm = new AudioStream;

	oBytSess->setContent( oAudStrm );
	oBytSess->setConfigInfo( codcCnfg );

	oAudStrm->setSampleRate( codcCnfg->getSamplingFrequency() );
	oAudStrm->setNbrChannels( codcCnfg->getChannelConfiguration() );
	oAudStrm->setCompression( AudioCompression::AAC );

	if ( sestream->hasBandwidth() ) {
		oBytSess->setBitRate( sestream->getBandwidth()->getValue() * 1024 );
	}

	getPeerEngine( 0 )->putSessionCallback( oBytSess );

}

void RTP::TLATMDecoderEngine::endSessionCallback() {

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TLATMDecoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

	TFramePtr	rtpTFrme = pInptFrme;

//	msgDbg( "getFrame(): --->" );

	if ( rtpTFrme->PT != sestream->getDataType() ) {
		throw InternalError( "Invalid payload type!" );
	}

	if ( ! rtpTFrme->M ) {
		throw InternalError( "Invalid 'M' bit!" );
	}

	Buffer		pcktData = rtpTFrme->data;

//	msgDbg( "decodePacket(): " + Utils::toHex( pcktData ) );

	IO::MemoryInputStreamPtr	iMStream = new IO::MemoryInputStream( pcktData );
	IO::ByteInputStreamPtr		iBStream = new IO::ByteInputStream( iMStream );

	iBStream->open();

	Uint32		unitSize = 0;
	Uchar		c;

	do {
		c = iBStream->getChar();
		unitSize += ( Uint32 )c;
	} while ( c == 0xFF );

	if ( iBStream->tell() + unitSize != iBStream->length() ) {
		throw InternalError( "Invalid unit size!" );
	}

//	Time::Clock	framePts = rtpTFrme->getPTS();;
//	Time::Clock	frameDts = rtpTFrme->getDTS();;

	Buffer		unitData = iBStream->getBytes( unitSize );

	BytesFramePtr	oBytFrme = new BytesFrame( rtpTFrme, unitData );

	oBytFrme->setOffset( frmeOffs );
	oBytFrme->setUnitOffset( unitOffs );
	oBytFrme->setKeyFrame( true );

//	msgDbg( "decodePacket(): PTS / DTS: " + framePts.dateTimeToBuffer( false, true )
//		+ " / " + frameDts.dateTimeToBuffer( false, true ) );

	frmeOffs += unitData.getLength();
	unitOffs++;

//	msgDbg( "decodePacket(): " + oBytFrme->toBuffer() );

//	msgDbg( "getFrame(): PTS / DTS: " + res->getPTS().dateTimeToBuffer( false, true )
//		+ " / " + res->getDTS().dateTimeToBuffer( false, true ) );

	getPeerEngine( 0 )->putFrameCallback( oBytFrme );

}

void RTP::TLATMDecoderEngine::flushSessionCallback() {

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

