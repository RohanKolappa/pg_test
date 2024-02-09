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
//	BFC.RWMutex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.RWMutex.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::RWMutex::RWMutex()
	: mainSem       (1)
	, readerSem     (0)
	, writerSem     (0)
	, waitingReaders(0)
	, activeReaders (0)
	, waitingWriters(0)
	, activeWriters (0) {



}

MT::RWMutex::~RWMutex() {

}

// ============================================================================

void MT::RWMutex::readLock() {

	mainSem.wait();

	if (activeWriters || waitingWriters) {
		waitingReaders++;
		mainSem.post();
		readerSem.wait();
	} else {
		activeReaders++;
		mainSem.post();
	}

}

void MT::RWMutex::readUnlock() {

	mainSem.wait();

	activeReaders--;

	if (waitingWriters && !activeReaders) {
		activeWriters++;
		waitingWriters--;
		writerSem.post();
	}

	mainSem.post();

}

void MT::RWMutex::writeLock() {

	mainSem.wait();

	if (activeWriters || activeReaders) {
		waitingWriters++;
		mainSem.post();
		writerSem.wait();
	} else {
		activeWriters++;
		mainSem.post();
	}

}

void MT::RWMutex::writeUnlock() {

	mainSem.wait();

	activeWriters--;

	if (waitingWriters) {
		activeWriters++;
		waitingWriters--;
		writerSem.post();
	} else {
		while (waitingReaders) {
			waitingReaders--;
			activeReaders++;
			readerSem.post();
		}
	}

	mainSem.post();

}

// ============================================================================

