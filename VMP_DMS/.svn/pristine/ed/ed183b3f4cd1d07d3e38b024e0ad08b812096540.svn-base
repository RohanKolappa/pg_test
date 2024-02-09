// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.RWLocker.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.MT.RWLocker.h"
#include "BFC.MT.RWMutex.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::RWLocker::RWLocker(
		RWMutex&		__mutex,
		Mode			__mode )
	: mutex(__mutex)
	, mode (__mode) {

	try {
		if (mode == Write)
			mutex.writeLock();
		else
			mutex.readLock();
	}
	catch (Exception& e) {
		msgErr( "RWLocker(): can't auto-lock mutex!" );
		msgErr( e.what() );
	}

}

MT::RWLocker::~RWLocker() {

	try {
		if (mode == Write)
			mutex.writeUnlock();
		else
			mutex.readUnlock();
	}
	catch (Exception& e) {
		msgErr( "~RWLocker(): can't auto-unlock mutex!" );
		msgErr( e.what() );
	}

}

// ============================================================================

