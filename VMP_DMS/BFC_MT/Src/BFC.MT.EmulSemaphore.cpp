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
//	BFC.EmulSemaphore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.EmulSemaphore.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_WIN32 ) && defined( WIN32_REALLY_EMULATE )

// ============================================================================

MT::EmulSemaphore::EmulSemaphore(
	const	Uint32		initialValue)
	: count( initialValue )
	, access( true, false )		// signaled, auto reset
	, notNull( false, true ) {	// not signaled, manual reset

}

MT::EmulSemaphore::~EmulSemaphore() {

}

// ============================================================================

void MT::EmulSemaphore::wait() {

	access.wait();

	while ( count == 0 ) {
		access.signalAndWaitOn( notNull );
		access.wait();
	}

	count--;

	access.set();

}

Bool MT::EmulSemaphore::tryWait() {

	access.wait();

	Bool res;

	if ( count == 0 ) {
		res = false;
	}
	else {
		count--;
		res = true;
	}

	access.set();

	return res;

}

void MT::EmulSemaphore::post() {

	access.wait();

	count++;

	if ( count == 1 ) {
//		notNull.pulse(); // may not be used anymore :-(
		notNull.set();
		notNull.reset();
	}

	access.set();

}

// ============================================================================

#else

// ============================================================================

MT::EmulSemaphore::EmulSemaphore(
	const	Uint32		initialValue)
	: sem( initialValue ) {

}

MT::EmulSemaphore::~EmulSemaphore() {

}

void MT::EmulSemaphore::wait() {

	sem.wait();

}

Bool MT::EmulSemaphore::tryWait() {

	return sem.tryWait();

}

void MT::EmulSemaphore::post() {

	sem.post();

}

// ============================================================================

#endif

// ============================================================================

