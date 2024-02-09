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
//	VMP.RTP.TSharedClock.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.RTP.TClockLine.h"
#include "VMP.RTP.TClockPoint.h"
#include "VMP.RTP.TSharedClock.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TSharedClock )

// ============================================================================

RTP::TSharedClock::TSharedClock(
	const	Uint32		pClckPrio ) :

	clckPrio	( pClckPrio ) {

	setObjectName( "VMP.RTP.TSharedClock[ " + Buffer( pClckPrio ) + " ]" );

}

// ============================================================================

void RTP::TSharedClock::configClock(
		TClockPointCPtr	pSyncTime ) {

//	msgDbg( "configClock(): syncTime: " + pSyncTime->toBuffer() );

	mstrClck.kill();
	syncTime = pSyncTime;

	if ( currLine ) {
		currLine->resetPoints( syncTime->getSourceTime() );
	}

	prevLoop = 0;
	prevResp.kill();

}

void RTP::TSharedClock::followClock(
		TSharedClockPtr	pMstrClck ) {

//	msgDbg( "followClock(): old sync: " + ( syncTime ? syncTime->toBuffer() : "n/a" ) );

	mstrClck = pMstrClck;
	syncTime = pMstrClck->getSyncTime();

//	msgDbg( "followClock(): new sync: " + syncTime->toBuffer() );

	if ( currLine ) {
		currLine->resetPoints( syncTime->getSourceTime() );
	}

	prevLoop = 0;
	prevResp.kill();

}

// ============================================================================

Time::Clock RTP::TSharedClock::addPacket(
	const	Time::Clock &	pSelfTime,
	const	Time::Clock &	pPeerTime,
	const	Bool		pTimeDisc ) {

	// Sanity checks.

	if ( ! syncTime ) {
		throw NotInitialized();
	}

	TClockPointCPtr	curPoint = new TClockPoint( pSelfTime, pPeerTime );

	// Record for future use...

	lastSelf = pSelfTime;

//	msgDbg( "addPacket(): curPoint: " + curPoint->toBuffer()
//		+ ", disc: " + Buffer( pTimeDisc ) );

	// Make sure we don't miss any discontinuity!

	Bool	timeDisc = ( pTimeDisc || hasDiscontinuity( pSelfTime, pPeerTime ) );

	// If we have to restart after a discontinuity, forget any previous
	// result.

	if ( timeDisc ) {
//		msgWrn( "addPacket(): time discontinuity!" );
		if ( currLine ) {
//			msgWrn( "addPacket(): currLine: " + currLine->toBuffer() );
			currLine->extendLastPoint( pSelfTime );
//			msgWrn( "addPacket(): currLine: " + currLine->toBuffer() );
//			msgWrn( "addPacket(): prevLoop: " + prevLoop.toBuffer() );
			prevLoop += currLine->getTargetDelta();
//			msgWrn( "addPacket(): prevLoop: " + prevLoop.toBuffer() );
			currLine.kill();
		}
		prevResp.kill();
	}

	if ( currLine
	  && pPeerTime < currLine->getLastPoint()->getTargetTime() ) {
//		msgWrn( "addPacket(): mini disc type 3 (currLine: "
//			+ currLine->toBuffer() + ", curPoint: "
//			+ curPoint->toBuffer() + ")!" );
		prevLoop += currLine->getLastPoint()->getTargetTime() - pPeerTime;
	}

	// Shortcut: return the same response for the same input!

	if ( prevResp ) {
		if ( prevResp->getSourceTime() == pPeerTime ) {
			return prevResp->getTargetTime();
		}
		prevResp.kill();
	}

	if ( currLine ) {
		Time::Clock lastSelf = currLine->getLastPoint()->getSourceTime();
		if ( pSelfTime < lastSelf ) {
			throw InternalError( "Self clock rolling back in time ("
				+ pSelfTime.dateTimeToBuffer( false, true ) + " <= "
				+ lastSelf.dateTimeToBuffer( false ,true ) + ")!" );
		}
	}

	if ( ! currLine ) {
		currLine = new TClockLine( curPoint );
	}
	else if ( currLine->getSourceDelta()
	       || currLine->getFirstPoint()->getSourceTime() != pSelfTime ) {
		currLine->setLastPoint( curPoint );
	}

	Time::Delta	dltaCorr;

	if ( mstrClck ) {
//		msgDbg( "addPacket(): master clock currLine: "
//			+ mstrClck->currLine->toBuffer() );
		dltaCorr = mstrClck->getTargetDelta( pSelfTime );
	}
	else {
		dltaCorr = getTargetDelta();
	}

	Time::Clock	fullCorr = syncTime->getTargetTime() + dltaCorr;

	prevResp = new TClockPoint( pPeerTime, fullCorr );

//	msgDbg( "addPacket(): --> " + fullCorr.dateTimeToBuffer( false ,true ) );

	return fullCorr;

}

// ============================================================================

RTP::TClockPointCPtr RTP::TSharedClock::getSyncTime() const {

//	msgDbg( "getSyncTime()" );

	return syncTime;

}

RTP::TClockPointCPtr RTP::TSharedClock::getSyncTime(
	const	Time::Clock &	pSelfTime ) {

//	msgDbg( "getSyncTime(): selfTime: " + pSelfTime.dateTimeToBuffer( false, true ) );

	if ( ! currLine ) {
		throw InternalError();
	}

	currLine->extendLastPoint( pSelfTime );

	return new TClockPoint( pSelfTime, syncTime->getTargetTime() + getTargetDelta() );

}

// ============================================================================

Time::Delta RTP::TSharedClock::getFreshness(
	const	Time::Clock &	pSelfTime ) const {

	return ( pSelfTime - lastSelf );

}

// ============================================================================

Time::Delta RTP::TSharedClock::getTargetDelta() const {

	return ( prevLoop + currLine->getTargetDelta() );

}

Time::Delta RTP::TSharedClock::getTargetDelta(
	const	Time::Clock &	pSelfTime ) {

	if ( pSelfTime < currLine->getLastPoint()->getSourceTime() ) {
		throw InternalError();
	}

//	currLine->extendLastPoint( pSelfTime );
//
//	return getTargetDelta();

	return ( prevLoop + ( currLine->getTargetForSource( pSelfTime ) - currLine->getFirstPoint()->getTargetTime() ) );

}

// ============================================================================

Bool RTP::TSharedClock::hasDiscontinuity(
	const	Time::Clock &	pSelfTime,
	const	Time::Clock &	pPeerTime ) const {

	if ( ! currLine ) {
		return false;
	}

//	msgDbg( "hasDiscontinuity(): selfTime: " + pSelfTime.dateTimeToBuffer( false, true )
//		+ ", peerTime: " + pPeerTime.dateTimeToBuffer( false, true )
//		+ ", syncTime: " + syncTime->toBuffer()
//		+ ", prevLoop: " + prevLoop.toBuffer()
//		+ ", currLine: " + currLine->toBuffer() );

//	TClockPointCPtr	frsPoint = currLine->getFirstPoint();
	TClockPointCPtr	lstPoint = currLine->getLastPoint();
	TClockPointCPtr	tmpPoint = new TClockPoint( pSelfTime, pPeerTime );

	if ( lstPoint->getSourceTime() == tmpPoint->getSourceTime() ) {
		return false;
	}

	TClockLineCPtr	tempLine = new TClockLine( lstPoint, tmpPoint );

	Double		curSpeed = currLine->getClockSpeed();
	Double		tmpSpeed = tempLine->getClockSpeed();
	Double		speedCor = tmpSpeed / curSpeed;

	if ( tempLine->getSourceDelta() >= Time::Delta( 0, 500000 )
	  && ( speedCor <= 0.5 || speedCor >= 2.0 ) ) {
		msgWrn( "hasDisc(): disc type 1 (currLine: "
			+ currLine->toBuffer() + ", tempLine: "
			+ tempLine->toBuffer() + ")!" );
		return true;
	}

	Time::Delta	diffSelf = currLine->getSourceDelta( tmpPoint );

	if ( diffSelf <= Time::Delta( -1, 0 )
	  || diffSelf >= Time::Delta(  1, 0 ) ) {
		msgWrn( "hasDisc(): disc type 2 (currLine: "
			+ currLine->toBuffer() + ", tmpPoint: "
			+ tmpPoint->toBuffer() + ", diffSelf: "
			+ diffSelf.toBuffer() + ")!" );
		return true;
	}

//	Time::Clock	lastPeer = lstPoint->getTargetTime();
//
//	if ( pPeerTime < lastPeer ) {
//		msgWrn( "hasDisc(): disc type 3 (frsPoint: "
//			+ frsPoint->toBuffer() + ", lstPoint: "
//			+ lstPoint->toBuffer() + ", tmpPoint: "
//			+ tmpPoint->toBuffer() + ")!" );
//		return true;
//	}

	return false;

}

// ============================================================================

