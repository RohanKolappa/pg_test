// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::RTP".
// 
// "VMP::RTP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::RTP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::RTP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.TDriftRemoverEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TClockSharer.h"
#include "VMP.RTP.TDriftRemoverEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"
#include "VMP.RTP.TSharedClock.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TDriftRemoverEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TDriftRemoverEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "82ccd7d7-dcc2-4c10-bb04-e4873a4bc239" ),
		"VMP.RTP.TDriftRemoverEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"TDriftRemoverEngine",
			"Utils",
			"TDriftRemoverEngine"
		)
	);

	return i;

}

// ============================================================================

RTP::TDriftRemoverEngine::TDriftRemoverEngine() :

	ProxyPushEngine	( getClassType() ),

	clckPrio	( 0 ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TDriftRemoverEngine::putSessionCallback(
		SessionPtr	pSession ) {

	if ( ! clckShrr ) {
		throw NotInitialized();
	}

	shrdClck.kill();

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void RTP::TDriftRemoverEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

	shrdClck.kill();
}

void RTP::TDriftRemoverEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrameCallback(): " + pFrame->toBuffer() );
//	msgDbg( "putFrameCallback(): PTS/DTS: "
//		+ pFrame->getPTS().dateTimeToBuffer( false, true )
//		+ " / "
//		+ pFrame->getDTS().dateTimeToBuffer( false, true ) );

	TFramePtr	rtpTFrme = pFrame;

	if ( ! shrdClck ) {
		shrdClck = new TSharedClock( clckPrio );
		clckShrr->addClock( shrdClck, rtpTFrme->readTime );
	}

	Time::Clock	oldFrPts = rtpTFrme->getPTS();
	Time::Clock	oldFrDts = rtpTFrme->getDTS();

	Time::Clock	newFrDts = shrdClck->addPacket(
						rtpTFrme->readTime,
						oldFrDts,
						rtpTFrme->hasTimeDisc() );

	rtpTFrme->setPTS( newFrDts + oldFrPts - oldFrDts );
	rtpTFrme->setDTS( newFrDts );

	// Make sure we use the best master...

	clckShrr->checkMaster( rtpTFrme->readTime );

	try {
		getPeerEngine( 0 )->putFrameCallback( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		shrdClck.kill();
		throw;
	}

}

// ============================================================================

static L2::EngineRegisterer< RTP::TDriftRemoverEngine > registerer;

// ============================================================================

