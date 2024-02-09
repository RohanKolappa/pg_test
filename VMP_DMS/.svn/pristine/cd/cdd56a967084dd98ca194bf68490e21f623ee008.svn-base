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
//	BFC.Semaphore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.Semaphore.h"
#include "BFC.MT.Thread.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::Semaphore::Semaphore(
	const	Uint32		initialValue ) :

	count       ( initialValue ),
	accessEvent (  true, false ),
	notNullEvent( false, false ) {

}

// ============================================================================

void MT::Semaphore::wait(
	const	Bool		checkStopped ) {

	accessEvent.wait();

	while ( count == 0 ) {
		Uint32 res;
		if ( checkStopped ) {
			res = accessEvent.signalAndWait(
				&notNullEvent,
				Thread::self()->getStoppedEvent() );
		}
		else {
			res = 0;
			accessEvent.signalAndWait(
				&notNullEvent );
		}
		accessEvent.wait();
		if ( res == 1 ) {
			accessEvent.signal();
			throw ThreadStopped();
		}
	}

	count--;

	accessEvent.signal();

}

Bool MT::Semaphore::tryWait() {

	Bool res;

	accessEvent.wait();

	if ( count == 0 ) {
		res = false;
	}
	else {
		count--;
		res = true;
	}

	accessEvent.signal();

	return res;

}

void MT::Semaphore::post() {

	accessEvent.wait();

	count++;

	if ( count == 1 ) {
		notNullEvent.pulse();
	}

	accessEvent.signal();

}

// ============================================================================

