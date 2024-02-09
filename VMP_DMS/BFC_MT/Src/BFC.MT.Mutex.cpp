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
//	BFC.Mutex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Mutex.h"
#include "BFC.MT.Thread.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::Mutex::Mutex(
	const	Type		pMutxType ) :

	mutxType	( pMutxType ),
	critSect	( true, false ),
	unlocked	( false, false ),
	owner		( 0 ),
	count		( 0 ) {

}

// ============================================================================

void MT::Mutex::lock(
	const	Bool		pChckStop ) {

	for (;;) {

		critSect.wait();

		if ( ! owner ) {
			owner = Thread::self();
			count = 1;
			critSect.signal();
			return;
		}

		if ( owner == Thread::self() ) {
			if ( mutxType != Mutex::Recursive ) {
				critSect.signal();
				throw WouldDeadLock();
			}
			count++;
			critSect.signal();
			return;
		}

		// Now, we enter the blocking part of this method. Let's make
		// sure we will get woken up if the thread get stopped!

		Uint32 res = 0;

		if ( pChckStop ) {
			res = critSect.signalAndWait(
				&unlocked,
				Thread::self()->getStoppedEvent() );
		}
		else {
			critSect.signalAndWait(
				&unlocked );
		}

		if ( res == 1 ) {
			throw ThreadStopped();
		}

	}

}

Bool MT::Mutex::tryLock() {

	Bool res = true;

	critSect.wait();

	if ( ! owner ) {
		owner = Thread::self();
		count = 1;
	}
	else if ( owner == Thread::self() ) {
		if ( mutxType != Mutex::Recursive ) {
			critSect.signal();
			throw WouldDeadLock();
		}
		count++;
	}
	else {
		res = false;
	}

	critSect.signal();

	return res;

}

void MT::Mutex::unlock() {

	critSect.wait();

	if ( ! owner ) {
		critSect.signal();
		throw NotLocked();
	}

	if ( owner != Thread::self() ) {
		critSect.signal();
		throw NotOwner();
	}

	count--;

	if ( count ) {
		// Recursive && still locked by us!
		critSect.signal();
		return;
	}

	owner = 0;

	unlocked.pulse();

	critSect.signal();

}

// ============================================================================

