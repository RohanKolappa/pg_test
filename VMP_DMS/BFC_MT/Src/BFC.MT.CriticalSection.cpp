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
//	BFC.CriticalSection.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.CriticalSection.h"
#include "BFC.MT.Thread.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::CriticalSection::CriticalSection()
	: accessEvent  (    true, false )
	, unlockedEvent(   false, false )
	, owner        (              0 ) {

}

MT::CriticalSection::~CriticalSection() {

}

// ============================================================================

void MT::CriticalSection::lock(
	const	Bool		checkStopped ) {

	for ( ;; ) {

		accessEvent.wait();

		if ( ! owner ) {
			owner = Thread::self();
			accessEvent.signal();
			return;
		}

		if ( owner == Thread::self() ) {
			accessEvent.signal();
			throw WouldDeadLock();
		}

		// Now, we enter the blocking part of this method. Let's make
		// sure we will get woken up if the thread get stopped!

		Uint32 res = 0;

		if ( checkStopped ) {
			res = accessEvent.signalAndWait(
				&unlockedEvent,
				Thread::self()->getStoppedEvent() );
		}
		else {
			accessEvent.signalAndWait(
				&unlockedEvent );
		}

		if ( res == 1 ) {
			throw ThreadStopped();
		}

	}

}

Bool MT::CriticalSection::tryLock() {

	Bool res = true;

	accessEvent.wait();

	if ( ! owner ) {
		owner = Thread::self();
	}

	else if ( owner == Thread::self() ) {
		accessEvent.signal();
		throw WouldDeadLock();
	}

	else {
		res = false;
	}

	accessEvent.signal();

	return res;

}

void MT::CriticalSection::unlock() {

	accessEvent.wait();

	if ( ! owner ) {
		accessEvent.signal();
		throw NotLocked();
	}

	if ( owner != Thread::self() ) {
		accessEvent.signal();
		throw NotOwner();
	}

	owner = 0;

	unlockedEvent.pulse();

	accessEvent.signal();

}

// ============================================================================

