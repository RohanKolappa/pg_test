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
//	VMP.RTP.TDTSComputerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.EngineRegisterer.h"

#include "VMP.RTP.TDTSComputerEngine.h"
#include "VMP.RTP.TFrame.h"
#include "VMP.RTP.TSession.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TDTSComputerEngine )

// ============================================================================

const DL::TypeCPtr & RTP::TDTSComputerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1ed5e3b5-d25a-4840-be1b-592eb8901ec9" ),
		"VMP.RTP.TDTSComputerEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"TDTSComputerEngine",
			"Utils",
			"TDTSComputerEngine"
		)
	);

	return i;

}

// ============================================================================

RTP::TDTSComputerEngine::TDTSComputerEngine() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, TSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, TSession::getClassType() );

}

// ============================================================================

void RTP::TDTSComputerEngine::putSessionCallback(
		SessionPtr	pSession ) {

	resetState();

	getPeerEngine( 0 )->putSessionCallback( pSession );

	setInSession( true );

}

void RTP::TDTSComputerEngine::endSessionCallback() {

	setInSession( false );

	getPeerEngine( 0 )->endSessionCallback();

	resetState();

}

void RTP::TDTSComputerEngine::putFrameCallback(
		FramePtr	pFrame ) {

	try {
		processFrame( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		resetState();
		throw;
	}

}

void RTP::TDTSComputerEngine::flushSessionCallback() {

	flushFrames();

	getPeerEngine( 0 )->flushSessionCallback();

}

// ============================================================================

void RTP::TDTSComputerEngine::resetState() {

	initDone = false;
	buffSize = 0;
	xtsDelta = 0;
	inptTime.kill();
	inptFrme.kill();
	sortTime.kill();
	prevTime = 0;
	currCorr = 0;

}

// ============================================================================

void RTP::TDTSComputerEngine::processFrame(
		FramePtr	pInptFrme ) {

//	msgDbg( "procFrame(): " + pInptFrme->toBuffer() );
//	msgDbg( "processFrame(): PTS/DTS: "
//		+ pInptFrme->getPTS().dateTimeToBuffer( false, true )
//		+ " / "
//		+ pInptFrme->getDTS().dateTimeToBuffer( false, true ) );

	TFramePtr	rtpTFrme = pInptFrme;

	if ( initDone && rtpTFrme->hasDisc() ) {
		flushFrames();
		if ( rtpTFrme->hasTimeDisc() ) {
			// Reset our corrector ONLY if a time discontinuity has
			// been detected. Otherwise, we could introduce a real
			// time discontinuity without flag in the packet!
			prevTime = 0;
			currCorr = 0;
		}
	}

	// Shortcut if pass-thru mode...

	if ( initDone && ! buffSize ) {
		// In pass-thru mode, we have to make sure we catch any clock
		// adjustement causing the DTS/PTS to go back in time.
		Time::Clock	currTime = pInptFrme->getDTS() + currCorr;
		Time::Delta	currDlta = ( prevTime ? currTime - prevTime : Time::Delta() );
		if ( currDlta < - Time::OneSecond ) {
			msgWrn( "processFrame(): clock rewind: " + currDlta.toBuffer() );
			rtpTFrme->setTimeDisc();
		}
		else if ( currDlta < 0 ) {
			Time::Delta	prevCorr = currCorr;
			currCorr += prevTime - currTime;
			currTime = prevTime;
			msgWrn( "processFrame(): increased pass-thru correction: "
				+ prevCorr.toBuffer() + " --> "
				+ currCorr.toBuffer() );
		}
		else if ( currDlta > 0 && currCorr ) {
			if ( currDlta >= currCorr ) {
				msgWrn( "processFrame(): resetting pass-thru correction: "
					+ currCorr.toBuffer() + " --> 0" );
				currTime -= currCorr;
				currCorr = 0;
			}
			else {
				Time::Delta	prevCorr = currCorr;
				currCorr -= currDlta;
				currTime = prevTime;
				msgWrn( "processFrame(): decreased pass-thru correction: "
					+ prevCorr.toBuffer() + " --> "
					+ currCorr.toBuffer() );
			}
		}
		if ( currCorr ) {
			pInptFrme->setPTS( pInptFrme->getPTS() + currCorr );
			pInptFrme->setDTS( pInptFrme->getDTS() + currCorr );
		}
		prevTime = currTime;
		sendFrame( pInptFrme );
		return;
	}

	Time::Clock	pts = pInptFrme->getPTS();
	Uint32		sze = inptTime.getSize();
//	Uint32		idx;

// Following test: what if NTP correction happens and new PTS has already been
// seen in the past ? Handle this!
//	if ( inptTime.contains( pts, idx ) ) {
//		// Example: IDR after SPS, PPS...
//		if ( idx != sze - 1 ) {
//			throw InternalError( "Duplicate PTS!" );
//		} 
//		inptFrme[ idx ] += pInptFrme;
//		return;
//	}
	if ( ! inptTime.isEmpty() && inptTime.getLast() == pts ) {
		inptFrme.getLast() += pInptFrme;
		return;
	}

	inptTime.resize( sze + 1 );
	inptFrme.resize( sze + 1 );

	inptTime[ sze ] = pts;
	inptFrme[ sze ] += pInptFrme;

	sortTime += pts;

	sze++;

	if ( ! initDone ) {
		if ( sortTime[ sze - 1 ] - sortTime[ 0 ] >= Time::Delta( 0, 500000 ) ) {
//			msgDbg( "processFrame(): init with 0.5 sec of data!" );
		}
		else if ( sze >= 15 ) {
//			msgDbg( "processFrame(): init with 15 frames!" );
		}
		else {
			return;
		}
		buffSize = 0;
		for ( Uint32 i = 0 ; i + 1 < sze ; i++ )
		for ( Uint32 j = i + 1 ; j < sze ; j++ ) {
			Uint32 d = j - i;
			if ( inptTime[ j ] < inptTime[ i ] && d > buffSize ) {
				buffSize = d;
			}
		}
		Uint32	nbrRfFrm = 0;
		for ( Uint32 i = 1 ; i < sze ; i++ ) {
			Uint32	tmpRfFrm = 0;
			for ( Uint32 j = 0 ; j < i ; j++ ) {
				if ( inptTime[ j ] > inptTime[ i ] ) {
					tmpRfFrm++;	// j is referenced by i
				}
			}
			if ( tmpRfFrm > nbrRfFrm ) {
				nbrRfFrm = tmpRfFrm;
			}
		}
		initDone = true;
		if ( ! buffSize ) {
//			msgDbg( "processFrame(): init --> flushing!" );
			flushFrames();
			return;
		}
		buffSize++;
		xtsDelta = ( sortTime[ sze - 1 ] - sortTime[ 0 ] ) * nbrRfFrm / ( sze - 1 );
//		msgDbg( "processFrame(): init --> buffSize == " + Buffer( buffSize )
//			+ ", xtsDelta: " + Buffer( xtsDelta )
//			+ ", nbrRfFrm: " + Buffer( nbrRfFrm ) );
	}

	flushFrames();

}

void RTP::TDTSComputerEngine::flushFrames() {

	while ( inptTime.getSize() > buffSize ) {
		Time::Clock		pts = inptTime[ 0 ];
		inptTime.popFirst();
		Time::Clock		dts = sortTime[ 0 ];
		sortTime.pop( 0 );
		Array< FramePtr >	lst = inptFrme.popFirst();
//		msgDbg( "flushFrames(): PTS: " + pts.dateTimeToBuffer( false, true )
//			+ ", DTS: " + dts.dateTimeToBuffer( false, true )
//			+ ", #frames: " + Buffer( lst.getSize() ) );
		for ( Uint32 i = 0 ; i < lst.getSize() ; i++ ) {
			lst[ i ]->setDTS( dts - xtsDelta );
			sendFrame( lst[ i ] );
		}
	}

}

void RTP::TDTSComputerEngine::sendFrame(
		FramePtr	pInptFrme ) {

//	msgDbg( "sendFrame(): " + pInptFrme->toBuffer() );
//	msgDbg( "sendFrame(): PTS/DTS: "
//		+ pInptFrme->getPTS().dateTimeToBuffer( false, true )
//		+ " / "
//		+ pInptFrme->getDTS().dateTimeToBuffer( false, true ) );

	getPeerEngine( 0 )->putFrameCallback( pInptFrme );

}

// ============================================================================

static L2::EngineRegisterer< RTP::TDTSComputerEngine > registerer;

// ============================================================================

