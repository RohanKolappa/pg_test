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
//	VMP.RTP.TClockSharer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.RTP.TSharedClock.h"
#include "VMP.RTP.TClockPoint.h"
#include "VMP.RTP.TClockSharer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TClockSharer )

// ============================================================================

RTP::TClockSharer::TClockSharer() :

	mstrIndx	( 0 ) {

	setObjectName( "VMP.RTP.TClockSharer" );

}

// ============================================================================

void RTP::TClockSharer::addClock(
		TSharedClockPtr	pEstrClck,
	const	Time::Clock &	pSelfTime ) {

//	msgDbg( "addClock(): adding \"" + pEstrClck->getObjectName() + "\"..." );

	// Insert the new clock into the list...

	Uint32		clckPrio = pEstrClck->getPriority();
	Uint32		clckIndx = 0;
	Uint32		listSize = clckList.getSize();

	TClockPointCPtr	syncTime = ( listSize
				? clckList[ mstrIndx ]->getSyncTime( pSelfTime )
				: new TClockPoint( pSelfTime, pSelfTime ) );

	while ( clckIndx < listSize ) {
		Uint32 currPrio = clckList[ clckIndx ]->getPriority();
		if ( clckPrio == currPrio ) {
			throw InternalError( "Already got a clock for priority "
				+ Buffer( clckPrio ) + "!" );
		}
		if ( clckPrio > currPrio ) {
			break;
		}
		clckIndx++;
	}

//	msgDbg( "addClock(): inserting at position " + Buffer( clckIndx )
//		+ " in a list of " + Buffer( listSize ) + " clock(s)..." );

	clckList.insert( clckIndx, pEstrClck );

	if ( clckIndx <= mstrIndx ) {
		mstrIndx++;
	}

	// Elect best clock now...

	mstrIndx = 0;

	clckList[ 0 ]->configClock( syncTime );

	for ( Uint32 i = 1 ; i < clckList.getSize() ; i++ ) {
		clckList[ i ]->followClock( clckList[ 0 ] );
	}

}

// ============================================================================

void RTP::TClockSharer::checkMaster(
	const	Time::Clock &	pSelfTime ) {

	Uint32	bestIndx = 0;

	while ( bestIndx < clckList.getSize() && clckList[ bestIndx ]->getFreshness( pSelfTime ) >= Time::OneSecond ) {
		bestIndx++;
	}

	if ( bestIndx >= clckList.getSize() ) {
		// We should have been called just after some clock has been
		// updated...
		throw InternalError( "No fresh clock ?!?" );
	}

	if ( bestIndx == mstrIndx ) {
		return;
	}

	msgWrn( "checkMaster(); Electing new master \""
		+ clckList[ bestIndx ]->getObjectName()
		+ "\" at time " + pSelfTime.dateTimeToBuffer( false, true ) );

//	for ( Uint32 i = 0 ; i < clckList.getSize() ; i++ ) {
//		msgDbg( "checkMaster(): " + clckList[ i ]->getObjectName()
//			+ " ---> "
//			+ clckList[ i ]->getFreshness( pSelfTime ).toBuffer() );
//	}

	TClockPointCPtr	syncTime = clckList[ mstrIndx ]->getSyncTime( pSelfTime );

	mstrIndx = bestIndx;

	clckList[ mstrIndx ]->configClock( syncTime );

	for ( Uint32 i = 0 ; i < clckList.getSize() ; i++ ) {
		if ( i != mstrIndx ) {
			clckList[ i ]->followClock( clckList[ mstrIndx ] );
		}
	}

}

// ============================================================================

