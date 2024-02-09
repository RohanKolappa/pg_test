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
//	VMP.RTP.NEStreamEncoderEngine.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.NEStreamEncoderEngine.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NEStreamEncoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NEStreamEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "3639f64b-9045-47d9-98ed-6625b53ce9d9" ),
		"VMP.RTP.NEStreamEncoderEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Network Encoder",
			"Devices",
			"RTP Network Encoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::NEStreamEncoderEngine::NEStreamEncoderEngine() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, NSession::getClassType() );

}

// ============================================================================

void RTP::NEStreamEncoderEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

//	msgDbg( "putSessionCallback(): --->" );

	TSessionPtr	rtpTSess = pInptSess;
	NSessionPtr	rtpNSess = new NSession(
				rtpTSess->getFlags(),
				rtpTSess->getStream(),
				rtpTSess->getSProgram(),
				rtpTSess->getSEStream() );

	getPeerEngine( 0 )->putSessionCallback( rtpNSess );

	setInSession( true );

//	msgDbg( "putSessionCallback(): <---" );

}

void RTP::NEStreamEncoderEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

//	msgDbg( "endSessionCallback(): <---" );

}

void RTP::NEStreamEncoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	TFramePtr	rtpTFrme = pInptFrme;
	NFramePtr	rtpNFrme;

	try {
		rtpNFrme = encodeTFrame( rtpTFrme );
	}
	catch ( Exception & e ) {
		setInSession( false );
		getPeerEngine( 0 )->endSessionCallback();
		throw L2::BrokenSession( e );
	}

	getPeerEngine( 0 )->putFrameCallback( rtpNFrme );

//	msgDbg( "putFrameCallback(): <---" );

}

void RTP::NEStreamEncoderEngine::flushSessionCallback() {

//	msgDbg( "flushSessionCallback(): --->" );

	getPeerEngine( 0 )->flushSessionCallback();

//	msgDbg( "flushSessionCallback(): <---" );

}

// ============================================================================

RTP::NFramePtr RTP::NEStreamEncoderEngine::encodeTFrame(
		TFramePtr	pRtpTFrme ) {

//	msgDbg( "encodeTFrame(): --->" );

	TFramePtr	rtpTFrme = pRtpTFrme;

	Buffer		buf = Buffer::chars( 12, 0x00 ) + rtpTFrme->data;
	Uchar *		ptr = buf.getVarPtr();

	ptr[ 0 ] = 0x80;	// RTP version 2.
	ptr[ 1 ] = ( rtpTFrme->M ? 0x80 : 0x00 )
	         | ( rtpTFrme->PT & 0x7f );

	*( Uint16 * )( ptr + 2 ) = System::Host2BE( ( Uint16 )rtpTFrme->SN );
	*( Uint32 * )( ptr + 4 ) = System::Host2BE( ( Uint32 )rtpTFrme->TS );
	*( Uint32 * )( ptr + 8 ) = System::Host2BE( ( Uint32 )rtpTFrme->SSRC );

	return new NFrame( rtpTFrme->srcSAddr, rtpTFrme->tgtSAddr, buf, rtpTFrme->readTime );

//	msgDbg( "encodeTFrame(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::NEStreamEncoderEngine > clientRegisterer;

// ============================================================================

