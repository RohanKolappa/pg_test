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
//	VMP.RTP.TAVCDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.VideoCompression.h"

#include "VMP.AVC.CodecConfig.h"
#include "VMP.AVC.Frame.h"
#include "VMP.AVC.NALU.h"
#include "VMP.AVC.Session.h"

#include "VMP.RTP.SBandwidth.h"
#include "VMP.RTP.SEStream.h"
#include "VMP.RTP.TAVCDecoderEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TAVCDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TAVCDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "4fc66458-43a4-485b-9881-0d3140116f5a" ),
		"VMP.RTP.TAVCDecoderEngine",
		TEStreamDecoderEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP AVC Decoder",
			"Devices",
			"RTP AVC Decoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::TAVCDecoderEngine::TAVCDecoderEngine(
		SEStreamCPtr	pSEStream,
	const	Uint32		pPackMode,
	const	Buffer &	pProfLevl,
		AVC::CodecConfigCPtr
				pCnfgRcrd ) :

	TEStreamDecoderEngine	( getClassType() ),

	sestream	( pSEStream ),
	packMode	( pPackMode ),
	profLevl	( pProfLevl ),
	cnfgRcrd	( pCnfgRcrd ) {

	setNbrOutputs( 1 );
	setOutputType( 0, AVC::Session::getClassType() );

}

// ============================================================================

void RTP::TAVCDecoderEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

	TSessionPtr		rtpSessn = pInptSess;
	VideoStreamPtr		iVidStrm = rtpSessn->getStream();

	if ( iVidStrm->getCompression() != VideoCompression::Unknown
	  && iVidStrm->getCompression() != VideoCompression::H264 ) {
		throw InvalidArgument();
	}

	AVC::SessionPtr		naluSess = new AVC::Session( Session::Live, AVC::NALUPacket, AVC::ModeNALU );
	VideoStreamPtr		oVidStrm = iVidStrm->clone();

	naluSess->setContent( oVidStrm );

	oVidStrm->setCompression( VideoCompression::H264 );

	if ( cnfgRcrd ) {
		naluSess->setConfigInfo( cnfgRcrd );
		Buffer b = cnfgRcrd->encode();
//		msgDbg( "getSession(): DCR: " + Utils::toHex( b ) );
		oVidStrm->setExtraData( b );
	}

	if ( sestream->hasBandwidth() ) {
		naluSess->setBitRate( sestream->getBandwidth()->getValue() * 1024 );
	}

	if ( sestream->hasFrameRate() ) {
		oVidStrm->setFrameRate( sestream->getFrameRate() );
	}

	frmeOffs = 0;
	unitOffs = 0;
	fuaBuffr.kill();
	dataDisc = false;
	timeDisc = false;

	getPeerEngine( 0 )->putSessionCallback( naluSess );

}

void RTP::TAVCDecoderEngine::endSessionCallback() {

	getPeerEngine( 0 )->endSessionCallback();

}

void RTP::TAVCDecoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	TFramePtr	rtpTFrme = pInptFrme;

	if ( rtpTFrme->hasDataDisc() ) {
//		msgWrn( "putFrameCallback(): data discontinuity!" );
		dataDisc = true;
		flushSessionCallback();
	}

	if ( rtpTFrme->hasTimeDisc() ) {
//		msgWrn( "putFrameCallback(): time discontinuity!" );
		timeDisc = true;
	}

//	msgDbg( "putFrameCallback(): packet: " + rtpTFrme->toBuffer() );

	// Sanity checks...

	if ( rtpTFrme->PT != sestream->getDataType() ) {
		throw InternalError( "Invalid payload type (packet: "
			+ Buffer( rtpTFrme->PT ) + ", metadata: "
			+ Buffer( sestream->getDataType() ) + ")!" );
	}

	// Packet decoding...

	Buffer		data = rtpTFrme->data;
	Uchar		type = ( data[ 0 ] & 0x1F );

	if ( ( type >= AVC::NALU::TypeSlice && type <= AVC::NALU::TypeSubSPS )
	  || ( type >= AVC::NALU::TypeAuxSlice && type <= AVC::NALU::TypeSliceExt ) ) {
//		msgDbg( "putFrameCallback(): single NALU!" );
		// Single NAL Unit Packet
		sendNalu( pInptFrme, data );
		return;
	}

	if ( type >= TypeStapA && type <= TypeMtap24 ) {
		// Aggreagation Packets
		throw InternalError( "Aggregation packets not supported!" );
	}

	if ( type == TypeFuA ) {
//		msgDbg( "get(): FU-A!" );
		// Fragmentation Unit
		Uchar			flgs = data[ 1 ];
		if ( flgs & 0x80 ) {	// Start bit.
			Uchar hder = ( ( data[ 0 ] & 0xE0 ) | ( flgs & 0x1F ) );
			fuaBuffr.kill();
			fuaBuffr += hder;
//			msgDbg( "get(): FU-A: start bit! type: "
//				+ Buffer( ( Uint32 )type, Buffer::Base16 ) + ", flgs: "
//				+ Buffer( ( Uint32 )flgs, Buffer::Base16 ) + " --> "
//				+ Buffer( ( Uint32 )hder, Buffer::Base16 ) );
		}
		else if ( fuaBuffr.isEmpty() ) {
//			msgWrn( "putFrameCallback(): skipping partial FU-A..." );
			return;
		}
		fuaBuffr += data( 2 );
		if ( flgs & 0x40 ) {	// End bit.
//			msgDbg( "get(): FU-A: end bit!" );
			sendNalu( pInptFrme, fuaBuffr );
			fuaBuffr.kill();
			return;
		}
		return;
	}

	else {
		throw InternalError( "Unsupported RTP packet type!" );
	}

}

void RTP::TAVCDecoderEngine::flushSessionCallback() {

	fuaBuffr.kill();

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

void RTP::TAVCDecoderEngine::sendNalu(
		FrameCPtr	pTmplFrme,
	const	Buffer &	pNaluData ) {

	AVC::NALUPtr	naluPckt = new AVC::NALU( pNaluData );

	AVC::FramePtr	naluFrme = new AVC::Frame( pTmplFrme, naluPckt );

	naluFrme->setOffset( frmeOffs );
	naluFrme->setUnitOffset( unitOffs );
	naluFrme->setKeyFrame( naluPckt->isIDR() );
	naluFrme->setDataDisc( dataDisc );
	naluFrme->setTimeDisc( timeDisc );

	if ( dataDisc || timeDisc ) {
//		msgDbg( "sendNalu(): dataDisc: " + Buffer( dataDisc )
//			+ ", timeDisc: " + Buffer( timeDisc ) );
		dataDisc = false;
		timeDisc = false;
	}

//	msgDbg( "putFrameCallback(): rx time: " + packetRx.dateTimeToBuffer( false, true )
//		+ ", PTS / DTS: " + framePts.dateTimeToBuffer( false, true )
//		+ " / " + frameDts.dateTimeToBuffer( false, true )
//		+ ", type: " + AVC::NALU::getTypeName( naluPckt->getType() ) );

	frmeOffs += naluFrme->getLength();
	unitOffs++;

//	msgDbg( "putFrameCallback(): <--- nalu: " + Utils::toHex( naluPckt->getData()( 0, 100 ) ) );

	getPeerEngine( 0 )->putFrameCallback( naluFrme );

}

// ============================================================================

