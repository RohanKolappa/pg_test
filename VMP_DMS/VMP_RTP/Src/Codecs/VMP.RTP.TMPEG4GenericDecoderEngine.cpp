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
//	VMP.RTP.TMPEG4GenericDecoderEngine.cpp
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
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TMPEG4GenericContext.h"
#include "VMP.RTP.TMPEG4GenericDecoderEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TMPEG4GenericDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TMPEG4GenericDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e7539b17-6f16-429d-8afc-131ee16087a0" ),
		"VMP.RTP.TMPEG4GenericDecoderEngine",
		TEStreamDecoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP MPEG4-Generic Decoder",
			"Devices",
			"RTP MPEG4-Generic Decoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TMPEG4GenericDecoderEngine::TMPEG4GenericDecoderEngine(
		SEStreamCPtr	pSEStream,
		TMPEG4GenericContextCPtr
				pCodcCtxt ) :

	TEStreamDecoderEngine	( getClassType() ),

	sestream	( pSEStream ),
	codcCtxt	( pCodcCtxt ) {

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

}

// ============================================================================

void RTP::TMPEG4GenericDecoderEngine::putSessionCallback(
		SessionPtr	/*pInptSess*/ ) {

// FIXME: SDP missing a "streamType", which is a required parameter, are
// FIXME: handled anyway.

	if ( codcCtxt->strmType != M4S::AudioStream ) {
		if ( codcCtxt->strmType == M4S::Forbidden
		  && codcCtxt->strmMode == "AAC-hbr" ) {
			msgWrn( "Missing required \"streamType\" parameter for AAC-hbr!" );
		}
		else {
			throw NotImplemented( "Stream type \"" + Buffer( codcCtxt->strmType ) + "\" not supported!" );
		}
	}

	if ( codcCtxt->strmMode != "AAC-hbr" ) {
		throw NotImplemented( "Stream mode \"" + codcCtxt->strmMode + "\" not supported!" );
	}

	if ( codcCtxt->sizeLgth != 13 ) {
		throw InternalError( "Invalid sizeLength ("
			+ Buffer( codcCtxt->sizeLgth ) + ") for this mode!" );
	}

	if ( codcCtxt->indxLgth != 3 ) {
		throw InternalError( "Invalid indexLength ("
			+ Buffer( codcCtxt->indxLgth ) + ") for this mode!" );
	}

	if ( codcCtxt->dltaLgth != 3 ) {
		throw InternalError( "Invalid indexDeltaLength ("
			+ Buffer( codcCtxt->dltaLgth ) + ") for this mode!" );
	}

	// Decode AudioSpecificConfig...

	if ( codcCtxt->codcCnfg.isEmpty() ) {
		throw InternalError( "Missing AAC \"config\" parameter!" );
	}

	codcCnfg = new AAC::AudioSpecificConfig;

	codcCnfg->decode( Utils::fromHex( codcCtxt->codcCnfg, false ) );

//	msgDbg( "getSession(): codec config (from SDP): " + codcCnfg->toBuffer() );

	if ( codcCnfg->getSamplingFrequency() != codcCtxt->smplRate ) {
		throw InternalError( "Sampling frequency mismatch!" );
	}

	if ( codcCnfg->getChannelConfiguration() != codcCtxt->nbrChans ) {
		throw InternalError( "Channel configuration mismatch!" );
	}

	smplRate = codcCtxt->smplRate;

	frmeOffs = 0;
	unitOffs = 0;

	BytesSessionPtr		oBytSess = new BytesSession( Session::Live );
	AudioStreamPtr		oAudStrm = new AudioStream;

	oBytSess->setContent( oAudStrm );
	oBytSess->setConfigInfo( codcCnfg );

	oAudStrm->setSampleRate( codcCtxt->smplRate );
	oAudStrm->setNbrChannels( codcCtxt->nbrChans );
	oAudStrm->setCompression( AudioCompression::AAC );

	if ( sestream->hasBandwidth() ) {
		oBytSess->setBitRate( sestream->getBandwidth()->getValue() * 1024 );
	}

	getPeerEngine( 0 )->putSessionCallback( oBytSess );

}

void RTP::TMPEG4GenericDecoderEngine::endSessionCallback() {

	flushSessionCallback();

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TMPEG4GenericDecoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "getFrame(): --->" );

	TFramePtr	rtpTFrme = pInptFrme;

	if ( rtpTFrme->PT != sestream->getDataType() ) {
		throw InternalError( "Invalid payload type!" );
	}

	if ( ! rtpTFrme->M ) {
		throw InternalError( "Invalid 'M' bit!" );
	}

	Buffer				pcktData = rtpTFrme->data;

//	msgDbg( "decodePacket(): " + Utils::toHex( pcktData ) );

	IO::MemoryInputStreamPtr	iMStream = new IO::MemoryInputStream( pcktData );
	IO::ByteInputStreamPtr		iBStream = new IO::ByteInputStream( iMStream );

	iBStream->open();

	Uint32	hderLgth = iBStream->getBEUint16();

//	msgDbg( "decodePacket(): AU-headers-length: " + Buffer( hderLgth ) );

	if ( hderLgth != 16 ) {
		throw InternalError( "Invalid AU-headers-length ("
			+ Buffer( hderLgth ) + ") for this mode!" );
	}

	Uint32	auHeader = iBStream->getBEUint16();
	Uint32	unitSize = ( auHeader >> 3 );
	Uint32	unitDlta = ( auHeader & 0x0007 );

	if ( iBStream->tell() + unitSize != iBStream->length() ) {
		throw InternalError( "Invalid AU-size!" );
	}

	if ( unitDlta ) {
		throw InternalError( "Invalid AU-Index(-delta)!" );
	}

//	Time::Clock	framePts = rtpTFrme->getPTS();
//	Time::Clock	frameDts = rtpTFrme->getDTS();

	Buffer		unitData = iBStream->getBytes( unitSize );

	BytesFramePtr	oBytFrme = new BytesFrame( rtpTFrme, unitData );

	oBytFrme->setOffset( frmeOffs );
	oBytFrme->setUnitOffset( unitOffs );
	oBytFrme->setKeyFrame( true );

//	msgDbg( "decodePacket(): rx time: " + packetRx.dateTimeToBuffer( false, true )
//		+ ", PTS / DTS: " + framePts.dateTimeToBuffer( false, true )
//		+ " / " + frameDts.dateTimeToBuffer( false, true ) );
//	msgDbg( "decodePacket(): diff: " + Buffer( frameDts - packetRx ) );

	frmeOffs += unitData.getLength();
	unitOffs++;

//	msgDbg( "decodePacket(): " + oBytFrme->toBuffer() );

//	msgDbg( "getFrame(): <---" );

	getPeerEngine( 0 )->putFrameCallback( oBytFrme );

//	msgDbg( "getFrame(): PTS / DTS: " + res->getPTS().dateTimeToBuffer( false, true )
//		+ " / " + res->getDTS().dateTimeToBuffer( false, true ) );

}

void RTP::TMPEG4GenericDecoderEngine::flushSessionCallback() {

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

