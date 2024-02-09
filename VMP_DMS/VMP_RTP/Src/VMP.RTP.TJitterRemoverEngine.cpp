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
//	VMP.RTP.TJitterRemoverEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Time.Delta.h"

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TClockLine.h"
#include "VMP.RTP.TClockPoint.h"
#include "VMP.RTP.TConvexCurve.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TJitterRemoverEngine.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TJitterRemoverEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TJitterRemoverEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "47a272b0-b2b5-4057-9881-9be2329a8e58" ),
		"VMP.RTP.TJitterRemoverEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"TJitterRemoverEngine",
			"Utils",
			"TJitterRemoverEngine"
		)
	);

	return i;

}

// ============================================================================

RTP::TJitterRemoverEngine::TJitterRemoverEngine() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TJitterRemoverEngine::putSessionCallback(
		SessionPtr	pSession ) {

	convCurv.kill();

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void RTP::TJitterRemoverEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

	convCurv.kill();

}

void RTP::TJitterRemoverEngine::putFrameCallback(
		FramePtr	pFrame ) {

	TFramePtr	rtpTFrme = pFrame;

	if ( rtpTFrme->hasTimeDisc() ) {
		convCurv.kill();
	}

	if ( ! convCurv ) {
		convCurv = new TConvexCurve;
	}

	Time::Clock	packetRx = rtpTFrme->readTime;
	Time::Clock	packetTs = rtpTFrme->getDTS();

	TClockPointCPtr	curPoint = new TClockPoint( packetRx, packetTs );
	TClockLineCPtr	bestLine = convCurv->addPoint( curPoint );

	Time::Clock	initTime = curPoint->getSourceTime();
	Time::Clock	corrTime = bestLine->getSourceForTarget( curPoint->getTargetTime() );
	Time::Delta	corrDlta = initTime - corrTime;

//	msgDbg( "putFrameCallback(): correction: " + corrDlta.toBuffer() );

	if ( corrDlta < 0
	  || corrTime < prevCorr ) {
		msgErr( "putFrameCallback(): convCurv: " + convCurv->toBuffer() );
		msgErr( "putFrameCallback(): bestLine: " + bestLine->toBuffer() );
		msgErr( "putFrameCallback(): curPoint: " + curPoint->toBuffer() );
		msgErr( "putFrameCallback(): prevCorr: " + prevCorr.dateTimeToBuffer( false, true ) );
		msgErr( "putFrameCallback(): corrTime: " + corrTime.dateTimeToBuffer( false, true ) );
		msgErr( "putFrameCallback(): corrDlta: " + corrDlta.toBuffer() );
		throw InternalError( "Dejitterer failed!" );
//		for ( Uint32 i = 0 ; i < lneArray.getSize() ; i++ ) {
//			msgDbg( "putFrameCallback(): line[ "
//				+ Buffer( i, Buffer::Base10, 2, ' ' ) + " ]: "
//				+ lneArray[ i ]->toBuffer() );
//		}
//		corrTime = prevCorr + Time::Delta( 0, 1 );
	}

	prevCorr = corrTime;

	rtpTFrme->readTime = corrTime;

	msgDbg( "putFrameCallback(): rx: " + packetRx.dateTimeToBuffer( false, true )
		+ " / " + corrDlta.toBuffer() + ", DTS: "
		+ packetTs.dateTimeToBuffer( false, true ) );

	try {
		getPeerEngine( 0 )->putFrameCallback( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		throw;
	}

}

void RTP::TJitterRemoverEngine::flushSessionCallback() {

	convCurv.kill();

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

static L2::EngineRegisterer< RTP::TJitterRemoverEngine > registerer;

// ============================================================================

