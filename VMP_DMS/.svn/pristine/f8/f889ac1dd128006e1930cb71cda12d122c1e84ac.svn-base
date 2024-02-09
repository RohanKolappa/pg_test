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
//	BFC.Condition.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.StackTrace.h"

#include "BFC.Light.LibConfig.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"
#include "BFC.MT.Thread.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

using namespace BFC;

// ============================================================================

void MT::Condition::signal() {

	shrdMutx.critSect.wait();

	if ( ! shrdMutx.owner ) {
		shrdMutx.critSect.signal();
		throw NotLocked();
	}

	if ( shrdMutx.owner != Thread::self() ) {
		shrdMutx.critSect.signal();
		throw NotOwner();
	}

	signaled.pulse();

	shrdMutx.critSect.signal();

}

void MT::Condition::broadcast() {

	shrdMutx.critSect.wait();

	if ( ! shrdMutx.owner ) {
		shrdMutx.critSect.signal();
		throw NotLocked();
	}

	if ( shrdMutx.owner != Thread::self() ) {
		shrdMutx.critSect.signal();
		throw NotOwner();
	}

	signaled.broadcast();
	signaled.reset();

	shrdMutx.critSect.signal();

}

// ============================================================================

//void MT::Condition::wait(
//	const	Buffer &	pLockMesg ) {
//
//	Uint32		cnt = 0;
//	Uint32		lmt = 10;
//	Time::Clock	lck = Time::now();
//
//	while ( ! timedWait( Time::Delta( 1, 0 ), true ) ) {
//		if ( ++cnt >= lmt ) {
//			Light::LibConfigCPtr l = Light::LibConfig::instance();
//			if ( l && l->getTraceDeadLocks() ) {
//				msgDbg( "wait(): blocked for " + Buffer( cnt ) + " seconds!\n"
//					+ "wait(): started: " + lck.dateTimeToBuffer( false, true ) + "\n"
//					+ "wait(): message: " + pLockMesg + "\n"
//					+ "wait(): stacktrace:\n" + StackTrace::dump( 0 ) );
//			}
//			lmt *= 10;
//		}
//	}
//
//}

void MT::Condition::wait(
	const	Bool		pChckStpd ) {

//	Uint32 cnt = 0;
//	Uint32 lmt = 10;
//
//	while ( ! timedWait( Time::Delta( 1, 0 ), pChckStpd ) ) {
//		if ( ++cnt >= lmt ) {
//			Light::LibConfigCPtr l = Light::LibConfig::instance();
//			if ( l && l->getTraceDeadLocks() ) {
//				msgDbg( "wait(): blocked for " + Buffer( cnt ) + " seconds!\n"
//					+ "wait(): stacktrace:\n" + StackTrace::dump( 0 ) );
//			}
//			lmt *= 10;
//		}
//	}

	shrdMutx.critSect.wait();

	if ( ! shrdMutx.owner ) {
		shrdMutx.critSect.signal();
		throw NotLocked();
	}

	if ( shrdMutx.owner != Thread::self() ) {
		shrdMutx.critSect.signal();
		throw NotOwner();
	}

	Uint32 oldCount = shrdMutx.count;

	shrdMutx.owner = 0;
	shrdMutx.count = 0;

	shrdMutx.unlocked.pulse();

	Uint32 res;

	if ( pChckStpd ) {
		res = shrdMutx.critSect.signalAndWait(
			&signaled,
			Thread::self()->getStoppedEvent() );
	}
	else {
		res = 0;
		shrdMutx.critSect.signalAndWait(
			&signaled );
	}

	for (;;) {
		shrdMutx.critSect.wait();
		if ( ! shrdMutx.owner ) {
			shrdMutx.owner = Thread::self();
			shrdMutx.count = oldCount;
			break;
		}
		shrdMutx.critSect.signalAndWait(
			&shrdMutx.unlocked );
	}

	shrdMutx.critSect.signal();

	if ( res == 1 || Thread::self()->shouldStop() ) {
		throw ThreadStopped();
	}

}

// ============================================================================

#if defined( PLATFORM_WIN32 )

Bool MT::Condition::timedWait(
	const	Time::Clock &	limit,
	const	Bool		pChckStpd ) {

	return timedWait( limit - Time::now(), pChckStpd );

}

#else

Bool MT::Condition::timedWait(
	const	Time::Delta &	limit,
	const	Bool		pChckStpd ) {

	return timedWait( Time::now() + limit, pChckStpd );

}

#endif

Bool MT::Condition::timedWait(
#if defined( PLATFORM_WIN32 )
	const	Time::Delta &		limit,
#else
	const	Time::Clock &		limit,
#endif
	const	Bool			pChckStpd ) {

	shrdMutx.critSect.wait();

	if ( ! shrdMutx.owner ) {
		shrdMutx.critSect.signal();
		throw NotLocked();
	}

	if ( shrdMutx.owner != Thread::self() ) {
		shrdMutx.critSect.signal();
		throw NotOwner();
	}

	Uint32 oldCount = shrdMutx.count;

	shrdMutx.owner = 0;
	shrdMutx.count = 0;

	shrdMutx.unlocked.pulse();

	Int32 res;

	if ( pChckStpd ) {
		res = shrdMutx.critSect.signalAndTimedWait(
			&signaled,
			Thread::self()->getStoppedEvent(),
			limit );
	}
	else {
		res = ( shrdMutx.critSect.signalAndTimedWait(
			&signaled,
			limit )
			? 0 : -1 );
	}

	for (;;) {
		shrdMutx.critSect.wait();
		if ( ! shrdMutx.owner ) {
			shrdMutx.owner = Thread::self();
			shrdMutx.count = oldCount;
			break;
		}
		shrdMutx.critSect.signalAndWait( 
			&shrdMutx.unlocked );
	}

	shrdMutx.critSect.signal();

	if ( res == 1 || Thread::self()->shouldStop() ) {
		throw ThreadStopped();
	}

	return ( res != -1 );

}

// ============================================================================

void MT::Condition::lock(
	const	Bool		pChckStpd ) {

	shrdMutx.lock( pChckStpd );

}

Bool MT::Condition::tryLock() {

	return shrdMutx.tryLock();

}

void MT::Condition::unlock() {

	shrdMutx.unlock();

}

// ============================================================================

