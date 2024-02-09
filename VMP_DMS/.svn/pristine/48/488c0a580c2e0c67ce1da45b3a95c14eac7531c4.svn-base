// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "BFC::MT".
//
// "BFC::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "BFC::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	BFC.EmulCondition.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.MT.EmulCondition.h"
#include "BFC.MT.EmulMutex.h"

#include "BFC.Time.Delta.h"
#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

//// ============================================================================
//
//#if defined( PLATFORM_WIN32 )
//
//// ============================================================================
//
//MT::EmulCondition::EmulCondition(
//		EmulMutex &	pShrdMutx ) :
//		
//	shrdMutx	( pShrdMutx ),
//	waiterBeg	( 0 ),
//	waiterEnd	( 0 ),
//	signalEvent	( false, true ) {	// not signaled, manual reset
//
//}
//
//// ============================================================================
//
//void MT::EmulCondition::signal() {
//
//	// wake-up the first waiting thread, if any...
//
//	if ( waiterBeg ) {
//		waiterBeg->flag = true;
//	}
//
//	signalEvent.set();
//	System::usleep( 1 );
//	signalEvent.reset();
//
//	// ALL waiting threads are trying to reacquire the mutex now, but
//	// only one will notice a change in its WaiterData structure...
//
//}
//
//void MT::EmulCondition::broadcast() {
//
//	for ( WaiterData *ptr = waiterBeg ; ptr ; ptr = ptr->next ) {
//		ptr->flag = true;
//	}
//
//	signalEvent.set();
//	System::usleep( 1 );
//	signalEvent.reset();
//
//}
//
//// ============================================================================
//
//void MT::EmulCondition::insertWaiterData(
//		WaiterData	*ptr ) {
//
//	ptr->prev = waiterEnd;
//
//	if ( waiterEnd )
//		waiterEnd->next = ptr;
//	else
//		waiterBeg = ptr;
//
//	waiterEnd = ptr;
//
//}
//
//void MT::EmulCondition::removeWaiterData(
//		WaiterData	*ptr ) {
//
//	if ( ptr->prev ) {
//		ptr->prev->next = ptr->next;
//	}
//	else {
//		waiterBeg = ptr->next;
//	}
//
//	if ( ptr->next ) {
//		ptr->next->prev = ptr->prev;
//	}
//	else {
//		waiterEnd = ptr->prev;
//	}
//
//}
//
//// ============================================================================
//
//void MT::EmulCondition::wait() {
//
//	WaiterData	data;
//
//	insertWaiterData( &data );
//
//	Uint32 counter = 0;
//
//	while ( ! data.flag ) {
//		counter++;
//		shrdMutx.unlocked.signalAndWaitOn( signalEvent );
//		shrdMutx.unlocked.wait();
//	}
//
//	removeWaiterData( &data );
//
//}
//
//Bool MT::EmulCondition::timedWait(
//	const	Time::Delta&		secs ) {
//
//	Time::Clock	timeout = Time::now() + secs;
//	WaiterData	data;
//
//	insertWaiterData( &data );
//
//	while ( ! data.flag ) {
//
//		Time::Clock	now = Time::now();
//
//		if ( now >= timeout ) {
//			break;
//		}
//
//		Time::Delta	delta = timeout - now;
//
//		Int64 usecs = (Int64)delta;
//		Int32 msecs = (Int32)(usecs / 1000);
//
//		if ( usecs <= 0 ) {
//			break;
//		}
//
//		if ( ! msecs ) {
//			msecs = 1;
//		}
//
//		Bool res = shrdMutx.unlocked.signalAndTimedWaitOn( signalEvent, msecs );
//		shrdMutx.unlocked.wait();
//
//		if ( ! res ) {
//			break;
//		}
//
//	}
//
//	removeWaiterData( &data );
//
//	return data.flag;
//
//}
//
//Bool MT::EmulCondition::timedWait(
//	const	Time::Clock&		timeout ) {
//
//	return timedWait( timeout - Time::now() );
//
//}
//
//// ============================================================================
//
//#else

// ============================================================================

MT::EmulCondition::EmulCondition(
		EmulMutex&	sharedMutex )
	: shrdMutx( sharedMutex )
	, cond( sharedMutex.mutex ) {

}

//MT::EmulCondition::~EmulCondition() {
//
//}

// ============================================================================

void MT::EmulCondition::signal() {

	cond.signal();

}

void MT::EmulCondition::broadcast() {

	cond.broadcast();

}

// ============================================================================

void MT::EmulCondition::wait() {

	cond.wait();

}

Bool MT::EmulCondition::timedWait(
	const	Time::Delta&		delta ) {

	return cond.timedWait( delta );

}

Bool MT::EmulCondition::timedWait(
	const	Time::Clock&		timeout ) {

	return cond.timedWait( timeout );

}

// ============================================================================

//#endif // PLATFORM_LINUX
//
//// ============================================================================

void MT::EmulCondition::lock() {

	shrdMutx.lock();

}

void MT::EmulCondition::unlock() {

	shrdMutx.unlock();

}

// ============================================================================

