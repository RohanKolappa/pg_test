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
//	VMP.RTP.TAVCEncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.Base64.Encoder.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.Frame.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.Session.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.TAVCEncoderEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TAVCEncoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TAVCEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9c75dbfc-8001-45f6-ba7b-5c3bf9ca1bb0" ),
		"VMP.RTP.TAVCEncoderEngine",
		RTP::TEStreamEncoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP AVC EStream Encoder",
			"Devices",
			"RTP AVC EStream Encoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TAVCEncoderEngine::TAVCEncoderEngine() :

	TEStreamEncoderEngine	( getClassType() ) {

	setInputType( 0, AVC::Session::getClassType() );

}

// ============================================================================

void RTP::TAVCEncoderEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

	AVC::SessionPtr		naluSess = pInptSess;
	VideoStreamPtr		iVidStrm = naluSess->getContent();

	if ( iVidStrm->getCompression() != VideoCompression::H264 ) {
		throw InternalError();
	}

// FIXME: horrible hack here! Put this nicely in a complete SProgram (i.e. an
// FIXME: SDP) and propagate using some other mechanism than stderr!

	AVC::CodecConfigCPtr	cnfgRcrd;

	if ( naluSess->hasConfigInfo() ) {
		cnfgRcrd = naluSess->getConfigInfo();
	}
	else if ( iVidStrm->getExtraData().hasContent() ) {
		AVC::CodecConfigPtr tempRcrd = new AVC::CodecConfig;
		tempRcrd->decode( iVidStrm->getExtraData() );
		cnfgRcrd = tempRcrd;
	}
	else {
		msgWrn( "putSessionCallback(): Input AVC Session/Stream doesn't contain SPS/PPS info!" );
	}

	if ( cnfgRcrd ) {
		BufferArray		propList;
		Base64::EncoderPtr	b64Codec = new Base64::Encoder;
		for ( Uint32 i = 0 ; i < cnfgRcrd->getNbrSPSs() ; i++ ) {
			propList += b64Codec->encode( cnfgRcrd->getSPS( i )->getData() );
		}
		for ( Uint32 i = 0 ; i < cnfgRcrd->getNbrPPSs() ; i++ ) {
			propList += b64Codec->encode( cnfgRcrd->getPPS( i )->getData() );
		}
		Buffer			propLine = propList.join( ',' );
		msgDbg( "putSession(): suggested: sprop-parameter-sets=" + propLine );
	}

	frmeOffs = 0;
	unitOffs = 0;

	TSessionPtr		rtpTSess = new TSession( naluSess->getFlags(), iVidStrm, 0, 0 );

	getPeerEngine( 0 )->putSessionCallback( rtpTSess );

	setInSession( true );

}

void RTP::TAVCEncoderEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TAVCEncoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrame(): --->" );

	AVC::FramePtr		naluFrme = pInptFrme;
	Time::Clock		frmeTstm = naluFrme->getPTS();
	AVC::NALUCPtr		naluStrc = naluFrme->getNALU();
	Buffer			naluBuff = naluStrc->getData();
	Uint32			buffSize = naluBuff.getLength();

	const Uint32		maxiSize = 1500 - 20 - 8 - 12; // (MTU - IP - UDP - RTP)

	if ( buffSize <= maxiSize ) {

		TFramePtr	pckt = makeTFrame();

		pckt->readTime	= Time::now();
// FIXME: Set 'M' bit for the very last packet of the access unit indicated by
// FIXME: the RTP timestamp.
		pckt->M		= false;
		pckt->PT	= 96;
		pckt->TS	= ( Uint32 )( ( ( Uint64 )naluFrme->getPTS() * 9 ) / 100 );
		pckt->data	= naluBuff;

		getPeerEngine( 0 )->putFrameCallback( pckt );

		return;

	}

	// Let's split the NALU into multiple RTP packets using FU-A mode.
	// The NALU to split is 'buffSize' long, of which 1 byte is used for
	// the prefix (incl. NALU type) and the rest is the payload itself.
	// An FU-A unit is 'maxiSize' long, of which 2 bytes are used for the
	// prefix and the rest is used for the payload.
	// So, we have to split 'buffSize - 1' bytes into chunks of maximum
	// 'maxiSize - 2' bytes.
 
	const Uint32	fUntSize = maxiSize - 2;
	Uint32		nbrFUnts = ( ( buffSize - 1 + fUntSize - 1 ) / fUntSize );
	Uchar		naluHder = naluBuff[ 0 ]; // Original header.

	naluBuff = naluBuff( 1 );	// Payload to split.

	for ( Uint32 i = 0 ; i < nbrFUnts ; i++ ) {

		Buffer		data;

		data.resize( 2 );

		data[ 0 ] = ( naluHder & 0x60 ) | TypeFuA;	// FU indicator
		data[ 1 ] = ( i == 0 ? 0x80 : 0x00 )
		          | ( i == nbrFUnts - 1 ? 0x40 : 0x00 )
		          | ( naluHder & 0x1F );		// FU header

		data += naluBuff( 0, fUntSize );

		naluBuff = naluBuff( fUntSize );

		TFramePtr	pckt = makeTFrame();

		pckt->readTime	= Time::now();
// FIXME: Set 'M' bit for the very last packet of the access unit indicated by
// FIXME: the RTP timestamp.
		pckt->M		= false;
		pckt->PT	= 96;
		pckt->TS	= ( Uint32 )( ( ( Uint64 )naluFrme->getPTS() * 9 ) / 100 );
		pckt->data	= data;

		getPeerEngine( 0 )->putFrameCallback( pckt );

	}

}

void RTP::TAVCEncoderEngine::flushSessionCallback() {

//	msgDbg( "flushSessionCallback(): --->" );

	getPeerEngine( 0 )->flushSessionCallback();

//	msgDbg( "flushSessionCallback(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TAVCEncoderEngine > clientRegisterer;

// ============================================================================

