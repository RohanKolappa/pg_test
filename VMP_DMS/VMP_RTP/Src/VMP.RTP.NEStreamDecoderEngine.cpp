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
//	VMP.RTP.NEStreamDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.NEStreamDecoderEngine.h"
#include "VMP.RTP.NEStreamDecoderWorker.h"
#include "VMP.RTP.NFrame.h"
#include "VMP.RTP.NSession.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NEStreamDecoderEngine )

// ============================================================================

const DL::TypeCPtr & RTP::NEStreamDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1650d482-659f-472d-b1a3-d53149061e73" ),
		"VMP.RTP.NEStreamDecoderEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"RTP Network Decoder",
			"Devices",
			"RTP Network Decoder" /*,
			RTP::readerUserInterface */
		)
	);

	return i;

}

// ============================================================================

RTP::NEStreamDecoderEngine::NEStreamDecoderEngine() :

	L2::ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, NSession::getClassType() );
	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

	dcdrWrkr = new NEStreamDecoderWorker;

}

// ============================================================================

void RTP::NEStreamDecoderEngine::putSessionCallback(
		SessionPtr	pInptSess ) {

//	msgDbg( "putSessionCallback(): --->" );

	NSessionPtr	netSessn = pInptSess;
	TSessionPtr	rtpSessn = new TSession(
				netSessn->getFlags(),
				netSessn->getStream(),
				netSessn->getSProgram(),
				netSessn->getSEStream() );

	getPeerEngine( 0 )->putSessionCallback( rtpSessn );

//	msgDbg( "putSessionCallback(): <---" );

}

void RTP::NEStreamDecoderEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	getPeerEngine( 0 )->endSessionCallback();

//	msgDbg( "endSessionCallback(): <---" );

}

void RTP::NEStreamDecoderEngine::putFrameCallback(
		FramePtr	pInptFrme ) {

//	msgDbg( "putFrameCallback(): --->" );

	NFramePtr	netFrame = pInptFrme;
	TFramePtr	rtpFrame;

	try {
		rtpFrame = dcdrWrkr->decodeNFrame( netFrame );
	}
	catch ( Exception & e ) {
		msgExc( e, "putFrameCallback(): Can't decode incoming packet!" );
		return;
//		getPeerEngine( 0 )->endSessionCallback();
//		throw L2::BrokenSession( e );
	}

	getPeerEngine( 0 )->putFrameCallback( rtpFrame );

//	msgDbg( "putFrameCallback(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< RTP::NEStreamDecoderEngine > clientRegisterer;

// ============================================================================

