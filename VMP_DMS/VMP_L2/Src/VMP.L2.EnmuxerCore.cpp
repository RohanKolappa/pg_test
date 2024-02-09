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
//	VMP.L2.EnmuxerCore.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.MuxedFrame.h"
#include "VMP.MuxedSession.h"

#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.PushEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, EnmuxerCore )

// ============================================================================

L2::EnmuxerCore::EnmuxerCore() :

	useGrTme	( true ),
	blckPutS	( true ),
	blckPutF	( true ),
	frceOrdr	( false ),
	ordrMain	( ( Uint32 )-1 ),
	statCond	( statMutx ),
	coreStat	( InitStat ),
	slotCntr	( 0 ),
	linkCntr	( 0 ),
	busyCntr	( 0 ),
	nbrStrms	( 0 ),
	frmeCntr	( 0 ) {

	setObjectName( "VMP.L2.EnmuxerCore" );

}

L2::EnmuxerCore::~EnmuxerCore() {

//	if ( coreStat != InitStat ) {
//		msgErr( "~EnmuxerCore(): wrong state!" );
//	}
//
//	if ( slotCntr ) {
//		msgErr( "~EnmuxerCore(): still got slots!" );
//	}
//
//	if ( peerEngn ) {
//		msgErr( "~EnmuxerCore(): still got peer engine!" );
//	}
//
//	if ( linkCntr ) {
//		msgErr( "~EnmuxerCore(): still got attached engines!" );
//	}
//
//	if ( busyCntr ) {
//		msgErr( "~EnmuxerCore(): still got busy engines!" );
//	}
//
//	if ( frmeCntr ) {
//		msgErr( "~EnmuxerCore(): still got engines putting a frame!" );
//	}

}

// ============================================================================

void L2::EnmuxerCore::setGraceTime(
	const	Time::Delta &	pGrceTime ) {

	MT::Locker statLckr( statMutx );

	useGrTme = true;
	grceTime = pGrceTime;

	statCond.broadcast();

}

void L2::EnmuxerCore::resetGraceTime() {

	MT::Locker statLckr( statMutx );

	useGrTme = false;
	grceTime = Time::Delta();

	statCond.broadcast();

}

// ============================================================================

Uint32 L2::EnmuxerCore::addSlot() {

//	msgDbg( "addSlot(): --->" );

	MT::Locker statLckr( statMutx );

	Uint32		i = 0;

	while ( i < slotList.getSize() && slotList[ i ] ) {
		i++;
	}

	SlotPtr		s = new Slot;

	if ( i >= slotList.getSize() ) {
		slotList += s;
	}
	else {
		slotList[ i ] = s;
	}

	slotCntr++;

	statCond.broadcast();

//	msgDbg( "addSlot(): <---" );

	return i;

}

void L2::EnmuxerCore::delSlot(
	const	Uint32		pSlotIndx ) {

//	msgDbg( "delSlot(): --->" );

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( s->sess ) {
		throw InternalError( "Engine still in session!" );
	}

	if ( s->peer ) {
		throw InternalError( "Engine still attached!" );
	}

	slotList[ pSlotIndx ].kill();

	slotCntr--;

	statCond.broadcast();

//	msgDbg( "delSlot(): <---" );

}

// ============================================================================

void L2::EnmuxerCore::attachPeerEngine(
	const	Uint32		pSlotIndx,
		L2::PushEnginePtr
				pPeerEngn ) {

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( s->peer ) {
		throw InternalError( "Engine already attached!" );
	}

	s->peer = pPeerEngn;

	if ( ! linkCntr ) {
		if ( peerEngn ) {
			throw AssertionFailed( "linkCntr == 0, but still got peerEngn!" );
		}
//		msgDbg( "attach(): got it!" );
		peerEngn = pPeerEngn;
	}
	else if ( pPeerEngn != peerEngn ) {
		throw InternalError( "Different peer engines!" );
	}

	linkCntr++;

	statCond.broadcast();

}

void L2::EnmuxerCore::detachPeerEngine(
	const	Uint32		pSlotIndx ) {

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! s->peer ) {
		throw InternalError( "Engine not attached!" );
	}

	s->peer.kill();

	if ( ! linkCntr ) {
		throw AssertionFailed( "Got peer, but linkCntr == 0!" );
	}

	linkCntr--;

	if ( ! linkCntr ) {
		if ( ! peerEngn ) {
			throw AssertionFailed( "linkCntr gone 0, but no peer!" );
		}
//		msgDbg( "detach(): bye!" );
		peerEngn.kill();
	}

	statCond.broadcast();

}

// ============================================================================

void L2::EnmuxerCore::putSession(
	const	Uint32		pSlotIndx,
		SessionPtr	pSlotSess ) {

//	msgDbg( "putSession(): --->" );

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! s->peer ) {
		throw InternalError( "Slot not attached!" );
	}

	if ( s->sess ) {
		throw InternalError( "Slot already in session!" );
	}

	while ( coreStat != InitStat ) {
//		msgDbg( "putSession(): waiting for init state..." );
		statCond.wait();
	}

	if ( linkCntr != slotCntr ) {
		throw InternalError( "Not all engines attached!" );
	}

	busyCntr++;

	s->sess = pSlotSess;
	s->cntr++;

	if ( busyCntr == slotCntr ) {
		// Last input entered the dance! Let's forward the muxed session!
//		msgDbg( "putSession(): forwarding..." );
		try {
			peerEngn->putSessionCallback( computeOutputSession() );
			coreStat = BusyStat;
			nbrStrms = busyCntr;
		}
		catch ( NotSuitable & e ) {
			msgExc( e, "Can't put session!" );
			coreStat = DoneStat;
		}
		statCond.broadcast();
	}

	if ( ! blckPutS ) {
		return;
	}

	while ( coreStat == InitStat ) {
//		msgDbg( "putSession(): waiting core busy..." );
		statCond.wait();
	}

	if ( coreStat == DoneStat ) {
		// Transition failed, or already past the Busy/Grace state...
		s->sess.kill();
		busyCntr--;
		if ( ! busyCntr ) {
			coreStat = InitStat;
			statCond.broadcast();
		}
		throw NotSuitable();
	}

//	msgDbg( "putSession(): <---" );

}

void L2::EnmuxerCore::endSession(
	const	Uint32		pSlotIndx ) {

//	msgDbg( "endSession(): --->" );

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! s->peer ) {
		throw InternalError( "Slot not attached!" );
	}

	if ( ! s->sess ) {
		throw InternalError( "Slot not in session!" );
	}

	if ( coreStat == InitStat ) {
		throw AssertionFailed( "init state ???" );
	}

	s->sess.kill();

	if ( busyCntr == nbrStrms ) {
//		msgDbg( "endSession(): first input to leave..." );
		if ( coreStat != BusyStat ) {
			throw AssertionFailed( "First leaving, but not busy ?" );
		}
		frstStop = Time::now();
		coreStat = GrceStat;
	}

	busyCntr--;

	if ( coreStat < GrceStat ) {
		throw AssertionFailed( "Should be finishing now..." );
	}

	if ( coreStat == GrceStat
	  && ( ! useGrTme
	    || ( grceTime && Time::now() >= frstStop + grceTime )
	    || ! busyCntr ) ) {
//		msgDbg( "endSession(): forwarding..." );
		peerEngn->endSessionCallback();
		coreStat = DoneStat;
	}

	if ( ! busyCntr ) {
		coreStat = InitStat;
	}

//	msgDbg( "endSession(): state: " + Buffer( ( Uint32 )coreStat ) );

	statCond.broadcast();

//	msgDbg( "endSession(): <---" );

}

void L2::EnmuxerCore::putFrame(
	const	Uint32		pSlotIndx,
		FramePtr	pSlotFrme ) {

//	msgDbg( "putFrame(): --->" );

	MT::Locker statLckr( statMutx );

	SlotPtr		s = slotList[ pSlotIndx ];

	if ( ! s ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! s->peer ) {
		throw InternalError( "Slot not attached!" );
	}

	if ( ! s->sess ) {
		throw InternalError( "Slot not in session!" );
	}

	if ( ! blckPutS ) {

		if ( ! blckPutF && coreStat == InitStat ) {
			msgWrn( "putFrame(): dropping frame..." );
			return;
		}

		while ( coreStat == InitStat ) {
//			msgDbg( "putFrame(): waiting core busy..." );
			statCond.wait();
		}

	}

	if ( coreStat == InitStat ) {
		throw AssertionFailed( "init state ???" );
	}

	// Forwarding the frame can only be done in the Busy/Grce states, so
	// check we are not in Done state yet...

	if ( coreStat == GrceStat
	  && ( ! useGrTme
	    || ( grceTime && Time::now() >= frstStop + grceTime ) ) ) {
//		msgDbg( "putFrame(): grace period exceeded..." );
		peerEngn->endSessionCallback();
		coreStat = DoneStat;
		statCond.broadcast();
	}

	if ( coreStat == DoneStat ) {
		throw EndOfStream();
	}

	// Send the output frame once we are authorized to do so...

	MuxedFramePtr	oMuxFrme = new MuxedFrame( s->indx, pSlotFrme );

	s->frme = oMuxFrme;
	frmeCntr++;

	statCond.broadcast();

	if ( frceOrdr ) {

		for (;;) {

			while ( frmeCntr < busyCntr ) {
//				msgDbg( "putFrame(): too few putting a frame... waiting..." );
				statCond.wait();
//				statCond.wait( "frmeCntr( " + Buffer( frmeCntr )
//					+ " ) < busyCntr( " + Buffer( busyCntr )
//					+ " )! slotIndx: " + Buffer( pSlotIndx )
//					+ ", slotCntr: " + Buffer( slotCntr )
//					+ ", linkCntr: " + Buffer( linkCntr )
//					+ ", busyCntr: " + Buffer( busyCntr )
//					+ ", nbrStrms: " + Buffer( nbrStrms )
//					+ ", frmeCntr: " + Buffer( frmeCntr )
//					+ ", coreStat: " + Buffer( ( Uint32 )coreStat )
//					+ ", slot->indx: " + Buffer( s->indx )
//					+ ", slot->cntr: " + Buffer( s->cntr ) );
			}

			Time::Clock	frmeTime = oMuxFrme->getDTS();
			Bool		mustWait = false;
//			Time::Clock	bestTime;
//			Uint32		bestIndx = 0;

			for ( Uint32 i = 0 ; i < slotList.getSize() ; i++ ) {

				if ( i == pSlotIndx ) {
					continue;
				}

				SlotPtr		othrSlot = slotList[ i ];

				if ( ! othrSlot ) {
					continue;
				}

				FramePtr	othrFrme = othrSlot->frme;

				if ( ! othrFrme ) {
					continue;
				}

				Time::Clock	othrTime = othrFrme->getDTS();

				if ( othrTime < frmeTime ) {
					mustWait = true;
					break;
				}

				if ( othrTime > frmeTime ) {
					continue;
				}

				if ( i == ordrMain ) {
					mustWait = true;
					break;
				}

			}

			if ( ! mustWait ) {
				break;
			}

//			msgDbg( "putFrame(): our frame too new... waiting..." );
			statCond.wait();
//			statCond.wait( "bestTime( " + bestTime.dateTimeToBuffer( false, true )
//				+ " ) < frmeTime( " + frmeTime.dateTimeToBuffer( false, true )
//				+ " )! bestIndx: " + Buffer( bestIndx )
//				+ ", slotIndx: " + Buffer( pSlotIndx )
//				+ ", slotCntr: " + Buffer( slotCntr )
//				+ ", linkCntr: " + Buffer( linkCntr )
//				+ ", busyCntr: " + Buffer( busyCntr )
//				+ ", nbrStrms: " + Buffer( nbrStrms )
//				+ ", frmeCntr: " + Buffer( frmeCntr )
//				+ ", coreStat: " + Buffer( ( Uint32 )coreStat )
//				+ ", slot->indx: " + Buffer( s->indx )
//				+ ", slot->cntr: " + Buffer( s->cntr ) );

		}

	}

	frmeCntr--;
	s->frme.kill();

	Bool forceEnd = false;

	try {
		peerEngn->putFrameCallback( oMuxFrme );
	}
	catch ( EndOfStream & ) {
		coreStat = DoneStat;
		forceEnd = true;
	}
	catch ( BrokenSession & ) {
		coreStat = DoneStat;
	}

	if ( coreStat == DoneStat ) {
		if ( forceEnd ) {
			peerEngn->endSessionCallback();
		}
		statCond.broadcast();
		throw EndOfStream();
	}

//	msgDbg( "putFrame(): <---" );

}

// ============================================================================

SessionPtr L2::EnmuxerCore::computeOutputSession() {

	MuxedSessionPtr res = new MuxedSession;

	for ( Uint32 i = 0 ; i < slotList.getSize() ; i++ ) {
		if ( slotList[ i ] ) {
			slotList[ i ]->indx = res->addComponent(
				slotList[ i ]->sess );
		}
	}

	return res;

}

// ============================================================================

BFC_PTR_IMPL_NS_CC( L2, EnmuxerCore, Slot )

// ============================================================================

