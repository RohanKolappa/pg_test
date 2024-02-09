// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.DemuxerCore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.L2.DemuxerCore.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, DemuxerCore )

// ============================================================================

L2::DemuxerCore::DemuxerCore() :

	slotCount(         0 ),
	nbrEngine(         0 ),
	countStop(         0 ),
	countBusy(         0 ),
	countWait(         0 ),
	countCond( coreMutex ),
	frameCond( coreMutex ) {

	setObjectName( "VMP.L2.DemuxerCore" );

}

// ============================================================================

L2::DemuxerCore::~DemuxerCore() {

//	if ( muxedSess ) {
//		msgWrn( "~DemuxerSlotTable(): still got input session!" );
//	}
//
//	if ( slotCount ) {
//		msgWrn( "~DemuxerSlotTable(): still got busy slots!" );
//	}

}

// ============================================================================

Uint32 L2::DemuxerCore::getSlot(
		DemuxerFilterPtr	pMuxFilter ) {

//	msgDbg( "getSlot(): --->" );

	MT::Locker locker( coreMutex );

	SlotEntry e;

	e.muxFilter = pMuxFilter;
	e.curStatus = SlotStop;
	e.sessIndex = Session::NoIndex;

	slotCount++;
	countStop++;

	Uint32 i;

	for ( i = 0 ; i < slotArray.getSize() ; i++ ) {
		if ( ! slotArray[ i ].muxFilter ) {
			break;
		}
	}

	if ( i >= slotArray.getSize() ) {
		slotArray += e;
	}
	else {
		slotArray[ i ] = e;
	}

//	msgDbg( "getSlot(): <---" );

	return i;

}

void L2::DemuxerCore::delSlot(
	const	Uint32		pSlotIndex ) {

//	msgDbg( "delSlot(): --->" );

	MT::Locker locker( coreMutex );

	switch ( slotArray[ pSlotIndex ].curStatus ) {
	case SlotBusy :
		throw InternalError( "delSlot(): still in session!" );
	case SlotStop :
		countStop--;
		break;
	case SlotWait :
		countWait--;
		break;
	}

	slotCount--;

	slotArray[ pSlotIndex ].muxFilter.kill();

	checkCounters();

//	msgDbg( "delSlot(): <---" );

}

// ============================================================================

void L2::DemuxerCore::attachPeerEngine(
		L2::PullEnginePtr	newEngine ) {

//	msgDbg( "attachPeerEngine(): --->" );

	MT::Locker locker( coreMutex );

	if ( ! curEngine ) {
		curEngine = newEngine;
	}
	else if ( curEngine != newEngine ) {
		throw InvalidCall( "Invalid Engine!" );
	}

	nbrEngine++;

//	msgDbg( "attachPeerEngine(): <---" );

}

void L2::DemuxerCore::detachPeerEngine() {

//	msgDbg( "detachPeerEngine(): --->" );

	MT::Locker locker( coreMutex );

	if ( ! nbrEngine ) {
		throw InvalidCall( "detach(): not attached!" );
	}

	if ( nbrEngine == 1
	  && slotCount
	  && ! countStop ) {
		throw InvalidCall( "detach(): not stopped!" );
	}

	nbrEngine--;

	if ( ! nbrEngine ) {
		curEngine.kill();
	}

//	msgDbg( "detachPeerEngine(): <---" );

}

// ============================================================================

SessionPtr L2::DemuxerCore::getOutputSession(
	const	Uint32		pSlotIndex ) {

//	msgDbg( "getOutputSession(): --->" );

//	msgDbg( "getOutputSession(): slot index == " + Buffer( pSlotIndex ) );

	MT::Locker locker( coreMutex );

	for (;;) {

		if ( slotArray[ pSlotIndex ].curStatus == SlotBusy ) {
			throw InternalError( "getOutputSession(): already in session!" );
		}

//		while ( ! countStop ) {
		while ( slotArray[ pSlotIndex ].curStatus != SlotStop ) {
			countCond.wait();
		}

//		msgDbg( "getOutputSession(): checking session for " + Buffer( pSlotIndex ) );

//		if ( slotArray[ pSlotIndex ].curStatus != SlotStop ) {
//			throw InternalError( "getOutputSession(): logical problem!" );
//		}

		if ( ! muxedSess ) {
			muxedSess = curEngine->getSessionCallback();
//			msgDbg( "getOutputSession(): session: " );
//			msgDbg( muxedSess->toBuffer() );
			Uint32 nbr = muxedSess->getNbrComponents();
			strmArray.resize( nbr );
			for ( Uint32 i = 0 ; i < nbr ; i++ ) {
				strmArray[ i ] = 0;
			}
			muxedIndx = muxedSess->getMuxedIndex();
			if ( muxedIndx ) {
//				msgDbg( "getOutputSession(): index: " );
//				msgDbg( muxedIndx->toBuffer( 10 ) );
			}
			nxtOffset = 0;
		}

		Uint32	sessIndex = slotArray[ pSlotIndex ].muxFilter->filter(
				muxedSess );

		countStop--;
		countCond.broadcast();

		if ( sessIndex == DemuxerFilter::NoSessionId ) {
			slotArray[ pSlotIndex ].curStatus = SlotWait;
			countWait++;
			checkCounters();
		}
		else {
//			msgDbg( "getOutputSession(): found session index == "
//				+ Buffer( sessIndex ) );
			slotArray[ pSlotIndex ].curStatus = SlotBusy;
			slotArray[ pSlotIndex ].sessIndex = sessIndex;
			strmArray[ sessIndex ]++;
			countBusy++;
			break;
		}

	}

//	msgDbg( "getOutputSession(): <---" );

	return muxedSess->getComponent( slotArray[ pSlotIndex ].sessIndex );

}

void L2::DemuxerCore::endOutputSession(
	const	Uint32		pSlotIndex ) {

//	msgDbg( "endOutputSession(): --->" );

//	msgDbg( "endOutputSession(): slot index == " + Buffer( pSlotIndex ) );

	MT::Locker locker( coreMutex );

	if ( slotArray[ pSlotIndex ].curStatus != SlotBusy ) {
		throw InternalError( "endOutputSession(): not in session!" );
	}

	slotArray[ pSlotIndex ].curStatus = SlotWait;

	countBusy--;
	countWait++;
	countCond.broadcast();

	Uint32 sessIndex = slotArray[ pSlotIndex ].sessIndex;

	strmArray[ sessIndex ]--;

	checkCounters();
	purgeCurFrame( sessIndex );

	frameCond.broadcast();

//	msgDbg( "endOutputSession(): <---" );

}

//Bool L2::DemuxerCore::isInSession(
//	const	Uint32		pSlotIndex ) {
//
//	MT::Locker locker( coreMutex );
//
//	return ( slotArray[ pSlotIndex ].curStatus == SlotBusy );
//
//}

FramePtr L2::DemuxerCore::getOutputFrame(
	const	Uint32		pSlotIndex ) {

//	msgDbg( "getOutputFrame(): --->" );

	MT::Locker locker( coreMutex );

	if ( slotArray[ pSlotIndex ].curStatus != SlotBusy ) {
		throw InternalError( "endOutputSession(): not in session!" );
	}

	Uint32 sessIndex = slotArray[ pSlotIndex ].sessIndex;

//	msgDbg( "getOutputFrame(): slotIndex[ "
//		+ Buffer( pSlotIndex )
//		+ " ], sessIndex[ "
//		+ Buffer( sessIndex )
//		+ " ]" );

//	msgDbg( "getOutputFrame(): sessIndex == " + Buffer( sessIndex ) );

	for (;;) {

//		msgDbg( "getOutputFrame(): loop..." );

		while ( muxedSess && ! currFrame ) {
//			msgWrn( "getOutputFrame(): asking: "
//				+ Buffer( nxtOffset ) );
			MuxedFramePtr newFrame;
			try {
				newFrame = curEngine->getFrameCallback( nxtOffset );
			}
			catch ( L2::BrokenSession & ) {
				muxedSess.kill();
				break;
			}
			nxtOffset += newFrame->getLength();
			if ( strmArray[ newFrame->getSubIndex() ] ) {
//				msgDbg( "getOutputFrame(): pushing frame for stream "
//					+ Buffer( newFrame->getSubIndex() ) );
				currFrame = newFrame;
			}
			else {
//				msgDbg( "getOutputFrame(): dropping frame for stream "
//					+ Buffer( newFrame->getSubIndex() ) );
			}
		}

		if ( ! muxedSess ) {
//			msgWrn( "getOutputFrame(): no more input session! Breaking pipe!" );
			slotArray[ pSlotIndex ].curStatus = SlotWait;
			countBusy--;
			countWait++;
			countCond.broadcast();
			strmArray[ sessIndex ]--;
			checkCounters();
			purgeCurFrame( sessIndex );
			throw BrokenSession();
		}

		MuxedFramePtr topFrame = currFrame;

		if ( topFrame->getSubIndex() == sessIndex ) {
//			msgDbg( "getOutputFrame(): <---" );
			currFrame.kill();
			return topFrame->getSubFrame();
		}

//		msgDbg( "getOutputFrame(): top frame not for us (frame: "
//			+ Buffer( topFrame->getSubIndex() )
//			+ ", asked: "
//			+ Buffer( sessIndex )
//			+ ")" );

		frameCond.broadcast();
		frameCond.wait();

	}

}

void L2::DemuxerCore::seekOutputFrame(
	const	Uint32		pSlotIndex,
	const	Uint64		byteOffset ) {

	if ( byteOffset == Frame::NoOffset ) {
		return;
	}

//	msgDbg( "seekOutputFrame(): --->" );

	MT::Locker locker( coreMutex );

	Uint32 sessIndex = slotArray[ pSlotIndex ].sessIndex;

//	msgDbg( "seekOutputFrame(): slotIndex[ "
//		+ Buffer( pSlotIndex )
//		+ " ], byteOffset[ "
//		+ ( byteOffset == Frame::NoOffset ? Buffer( "n/a" ) : Buffer( byteOffset ) )
//		+ " ], sessIndex[ "
//		+ Buffer( sessIndex )
//		+ " ]" );

	if ( ! muxedSess ) {
		return;
	}

	if ( ! muxedIndx ) {
		msgWrn( "Not seekable!!!" );
		return;
	}

	currFrame.kill();

	Uint32	muxedFrameIndex = muxedIndx->lookup( sessIndex, byteOffset );

//	msgDbg( "seekOutputFrame(): seeking peer engine: ( "
//		+ Buffer( sessIndex )
//		+ ", "
//		+ Buffer( byteOffset )
//		+ " ) --> "
//		+ Buffer( muxedFrameIndex ) );

	nxtOffset = muxedFrameIndex;

//	msgDbg( "seekOutputFrame(): <---" );

}

// ============================================================================

void L2::DemuxerCore::purgeCurFrame(
	const	Uint32		pSessionIndex ) {

//	msgDbg( "purgeCurFrame(): checking..." );

	if ( currFrame
	  && currFrame->getSubIndex() == pSessionIndex
	  && ! strmArray[ pSessionIndex ] ) {
//		msgDbg( "purgeCurFrame(): purging..." );
		currFrame.kill();
		frameCond.broadcast();
	}

}

void L2::DemuxerCore::checkCounters() {

//	msgDbg( "checkCounters(): ( "
//		+ Buffer( countWait )
//		+ ", "
//		+ Buffer( slotCount )
//		+ " )" );

	if ( countWait < slotCount ) {
//		msgDbg( "checkCounters(): nothing to do..." );
		return;
	}

//	msgDbg( "checkCounters(): resetting counters!" );

	for ( Uint32 i = 0 ; i < slotArray.getSize() ; i++ ) {
		slotArray[ i ].curStatus = SlotStop;
	}

	countWait = 0;
	countStop = slotCount;
	countCond.broadcast();

	if ( curEngine && curEngine->isInSession() ) {
		curEngine->endSessionCallback();
	}

	muxedSess.kill();
	currFrame.kill();

//	msgDbg( "checkCounters(): counters reset!" );

}

// ============================================================================

