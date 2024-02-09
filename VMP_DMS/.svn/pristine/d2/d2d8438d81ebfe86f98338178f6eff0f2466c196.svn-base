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
//	VMP.HLS.LevelInfo.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.HLS.FragmentData.h"
#include "VMP.HLS.FragmentInfo.h"
#include "VMP.HLS.LeafManifest.h"
#include "VMP.HLS.LevelInfo.h"
#include "VMP.HLS.TimeStamp.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( HLS, LevelInfo )

// ============================================================================

HLS::LevelInfo::LevelInfo() {

	setObjectName( "VMP.HLS.LevelInfo" );

	trgtDrtn = 0.0;
	frstSqNo = InvalidSeqNo;
	lastSqNo = InvalidSeqNo;
	totlDrtn = 0.0;
	progrmId = 0;
	bandwdth = 0;
	vdeoWdth = 0;
	vdeoHght = 0;

}

// ============================================================================

void HLS::LevelInfo::mergeManifest(
		LeafManifestPtr	pManifest ) {

	// Sanity checks...

	// ... must come from the same URL.

	Buffer	tempLink = pManifest->getUrl();

	if ( levelUrl.isEmpty() ) {
		levelUrl = tempLink;
	}
	else if ( levelUrl != tempLink ) {
// FIXME: The only case where the url could be different is when we use the
// FIXME: "forceReload" option, which appends an extra "&time=..." field to
// FIXME: the link. Check this, just for the sake of completeness!
	}

	Double	tempDrtn = pManifest->getTargetDuration();

	// ... must have the same target duration.

	if ( trgtDrtn == 0.0 ) {
		trgtDrtn = tempDrtn;
	}
	else if ( trgtDrtn != tempDrtn ) {
		throw InternalError( "LevelInfo::mergeManifest(): "
			"Mutating TARGET_DURATION! New: " + Buffer( tempDrtn )
			+ ", old: " + Buffer( trgtDrtn ) );
	}

	// ... must contain some fragments.

	if ( fragList.getSize() && ! pManifest->hasFragments() ) {
		throw InternalError( "LevelInfo::mergeManifest(): "
			"Empty manifest!" );
	}

	// ... must not be rewinding.

	Uint64	frshFrst = pManifest->getFirstSeqNo();
	Uint64	frshLast = pManifest->getLastSeqNo();

// FIXME: If the server restarted, we could end up with frshFrst == 1. For the
// FIXME: time being, we don't provide any workaround for this.

	if ( frstSqNo != InvalidSeqNo && frshFrst < frstSqNo ) {
		throw InternalError( "LevelInfo::mergeManifest(): "
			"Sequence number rewinding! First current: "
			+ Buffer( frstSqNo ) + ", new: " + Buffer( frshFrst ) );
	}

	if ( lastSqNo != InvalidSeqNo && frshLast < lastSqNo ) {
		throw InternalError( "LevelInfo::mergeManifest(): "
			"Sequence number rewinding! Last current: "
			+ Buffer( lastSqNo ) + ", new: " + Buffer( frshLast ) );
	}

	// ... must have some common fragments with previous list.

	Int64	commSize = lastSqNo - frshFrst + 1;

//	if ( lastSqNo != InvalidSeqNo && commSize <= 0 ) {
//		throw InternalError( "LeveInfo", "mergeManifest",
//			"No common fragments!" );
//	}

	// Merge timing info from current list into new list...

	if ( frstSqNo != InvalidSeqNo ) {
		Uint64	dropSize = frshFrst - frstSqNo;
		for ( Uint64 i = 0 ; i < dropSize && fragList.getSize() ; i++ ) {
			fragList.pop( 0 );
		}
	}

	Array< FragmentInfoPtr > frshList = pManifest->getFragments();

	for ( Int64 i = 0 ; i < commSize ; i++ ) {
		if ( ! frshList[ i ]->sameAs( fragList[ i ] ) ) {
			throw InternalError( "LevelInfo::mergeManifest(): "
				"Mutating manifest! New: " + frshList[ i ]->toString()
				+ " / Old: " + fragList[ i ]->toString() );
		}
		frshList[ i ] = fragList[ i ];
	}

	// Use new list...

	fragList = frshList;
	frstSqNo = frshFrst;
	lastSqNo = frshLast;

	updateStartTime();

}

void HLS::LevelInfo::updateFragment(
		FragmentInfoPtr	pFragInfo,
		FragmentDataPtr	pFragData ) {

	Uint64	fragSqNo = pFragInfo->getSeqNo();
	Uint32	fragIndx = getIndexFromSeqNo( fragSqNo );

	if ( fragIndx == ( Uint32 )-1 ) {
		msgWrn( "updateFragment(): Obsolete Fragment!" );
		return;
	}

	if ( fragList[ fragIndx ] != pFragInfo ) {
		throw InternalError( "LevelInfo::updateFragment(): Unknown Fragment!" );
	}

	// Linearize PTS and adjust durations...

	// ... look for a previous fragment we know the PTS of.

	Int32	prevIndx = fragIndx - 1;	// Index of the previous fragment.
	Double	prevDrtn = 0.0;			// Duration (m3u8-based) between prevIndx and fragIndx.

	while ( prevIndx >= 0 && ! fragList[ prevIndx ]->hasRealPTS() ) {
		prevIndx--;
	}

	if ( prevIndx >= 0 ) {
		for ( Uint32 i = ( Uint32 )prevIndx ; i < fragIndx ; i++ ) {
			prevDrtn += fragList[ i ]->getDuration();
		}
	}

	// ... look for a next fragment we know the PTS of.

	Uint32	nextIndx = fragIndx + 1;	// Index of the next fragment.
	Double	nextDrtn = 0;			// Duration (m3u8-based) between fragIndx and nextIndx.

	while ( nextIndx < fragList.getSize() && ! fragList[ nextIndx ]->hasRealPTS() ) {
		nextIndx++;
	}

	if ( nextIndx < fragList.getSize() ) {
		for ( Uint32 i = fragIndx ; i < nextIndx ; i++ ) {
			nextDrtn += fragList[ i ]->getDuration();
		}
	}

	TimeStampCPtr	lineStmp;		// Theoretical (approximate) fragData PTS.

	// ... make sure the new PTS is on the same timeline.

	if ( prevIndx >= 0 ) {
		lineStmp = fragList[ prevIndx ]->getRealPTS()->addNumber( prevDrtn );
		msgDbg( "LevelInfo::updateFragment(): "
			"Linearizing PTS: prevIndx: " + Buffer( prevIndx )
			+ ", prevSqNo: " + Buffer( fragList[ prevIndx ]->getSeqNo() )
			+ ", fragIndx: " + Buffer( fragIndx )
			+ ", fragSqNo: " + Buffer( pFragInfo->getSeqNo() )
			+ ", prev PTS: " + fragList[ prevIndx ]->getRealPTS()->toString()
			+ ", frag PTS: " + pFragData->getMinPTS()->toString()
			+ ", prev drtn: " + Buffer( prevDrtn )
			+ ", comp PTS: " + lineStmp->toString() );
	}
	else if ( nextIndx < fragList.getSize() ) {
		lineStmp = fragList[ nextIndx ]->getRealPTS()->subNumber( nextDrtn );
		msgDbg( "LevelInfo::updateFragment(): "
			"Linearizing PTS: nextIndx: " + Buffer( nextIndx )
			+ ", nextSqNo: " + Buffer( fragList[ nextIndx ]->getSeqNo() )
			+ ", fragIndx: " + Buffer( fragIndx )
			+ ", fragSqNo: " + Buffer( pFragInfo->getSeqNo() )
			+ ", next PTS: " + fragList[ nextIndx ]->getRealPTS()->toString()
			+ ", frag PTS: " + pFragData->getMinPTS()->toString()
			+ ", next drtn: " + Buffer( nextDrtn )
			+ ", comp PTS: " + lineStmp->toString() );
	}

	if ( lineStmp ) {
		pFragData->ensureCloseTo( lineStmp );
	}

	pFragInfo->setRealPTS( pFragData->getMinPTS() );

	// ... fix manifest durations based on real PTS.

	TimeStampCPtr	prevStmp;
	TimeStampCPtr	currStmp;
	TimeStampCPtr	nextStmp;
	Double		realDrtn = 0.0;

	if ( prevIndx >= 0 ) {
		prevStmp = fragList[ prevIndx ]->getRealPTS();
		currStmp = fragList[ fragIndx ]->getRealPTS();
		realDrtn = TimeStamp::sub( currStmp, prevStmp )->toDouble();
		if ( prevDrtn != realDrtn ) {
			msgDbg( "LevelInfo::updateFragment(): "
				"Fixing prev durations: m3u8 drtn: "
				+ Buffer( prevDrtn ) + ", real drtn: "
				+ Buffer( realDrtn ) );
			for ( Uint32 i = ( Uint32 )prevIndx ; i < fragIndx ; i++ ) {
				fragList[ i ]->setDuration( fragList[ i ]->getDuration() / prevDrtn * realDrtn );
			}
		}
		else {
			msgDbg( "LevelInfo::updateFragment(): "
				"Keeping prev durations: m3u8 drtn: "
				+ Buffer( prevDrtn ) + ", real drtn: "
				+ Buffer( realDrtn ) );
		}
	}

	if ( nextIndx < fragList.getSize() ) {
		currStmp = fragList[ fragIndx ]->getRealPTS();
		nextStmp = fragList[ nextIndx ]->getRealPTS();
		realDrtn = TimeStamp::sub( nextStmp, currStmp )->toDouble();
		if ( nextDrtn != realDrtn ) {
			msgDbg( "LevelInfo::updateFragment(): "
				"Fixing next durations: m3u8 drtn: "
				+ Buffer( nextDrtn ) + ", real drtn: "
				+ Buffer( realDrtn ) );
			for ( Uint32 i = fragIndx ; i < nextIndx ; i++ ) {
				fragList[ i ]->setDuration( fragList[ i ]->getDuration() / nextDrtn * realDrtn );
			}
		}
		else {
			msgDbg( "LevelInfo::updateFragment(): "
				"Keeping next durations: m3u8 drtn: "
				+ Buffer( nextDrtn ) + ", real drtn: "
				+ Buffer( realDrtn ) );
		}
	}

	updateStartTime();

}

// ============================================================================

HLS::FragmentInfoPtr HLS::LevelInfo::getFragmentFromSeqNo(
	const	Uint64		pFragSqNo ) const {

	Uint32 fragIndx = getIndexFromSeqNo( pFragSqNo );

	return ( fragIndx != ( Uint32 )-1 ? fragList[ fragIndx ] : FragmentInfoPtr() );

}

HLS::FragmentInfoPtr HLS::LevelInfo::getFragmentByPosition(
	const	Uint64		pCrsrOffs ) const {

	return getFragmentFromSeqNo( getSeqNoBeforePosition( pCrsrOffs ) );

}

HLS::FragmentInfoPtr HLS::LevelInfo::getFirstFragment() const {

	return fragList[ 0 ];

}

HLS::FragmentInfoPtr HLS::LevelInfo::getLastFragment() const {

	return fragList[ fragList.getSize() - 1 ];

}

// ============================================================================

Uint64 HLS::LevelInfo::getSeqNoBeforePosition(
	const	Uint64		position ) const {

	if ( position < fragList[ 0 ]->getRelativeStartTime() ) {
		return frstSqNo;
	}

	for ( Uint32 i = 0 ; i < fragList.getSize() ; i++ ) {
		if ( position >= fragList[ i ]->getRelativeStartTime()
		  && position < fragList[ i ]->getRelativeStopTime() ) {
			return ( frstSqNo + i );
		}
	}

	return lastSqNo;

}

Uint32 HLS::LevelInfo::getIndexFromSeqNo(
	const	Uint64		pFragSqNm ) const {

	return ( pFragSqNm >= frstSqNo && pFragSqNm <= lastSqNo
		? pFragSqNm - frstSqNo
		: ( Uint32 )-1 );

}

// ============================================================================

void HLS::LevelInfo::updateStartTime() {

//	Double	prevTotl = totlDrtn;

	totlDrtn = 0.0;

	for ( Uint32 i = 0 ; i < fragList.getSize() ; i++ ) {
		fragList[ i ]->setRelativeStartTime( totlDrtn );
		totlDrtn += fragList[ i ]->getDuration();
	}

//	if ( totlDrtn != prevTotl ) {
//		hlsPlayr.dispatchEvent( new HLSEvent( HLSEvent.DURATION_CHANGED, totlDrtn ) );
//	}

}

// ============================================================================

