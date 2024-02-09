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
//	BFC.Locker.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.MT.Lockable.h"
#include "BFC.MT.Locker.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::Locker::Locker(
		Lockable&	toBeLocked )
	: lockable( toBeLocked ) {

	try {
		lockable.lock( true );
	}
	catch ( ThreadStopped& ) {
//		msgErr( "Locker(): Thread Stopped!!!" );
//		msgErr( "Locker(): ---> re-trying to lock!!!" );
		try {
			lockable.lock( false );
//			msgErr( "Locker(): ---> Got lock!!!" );
		}
		catch ( Exception& e ) {
			msgErr( "Locker(): 2nd chance error!" );
			msgErr( e.what() );
		}
	}
	catch ( Exception& e ) {
		msgErr( "Locker(): can't auto-lock mutex!" );
		msgErr( e.what() );
	}

}

MT::Locker::~Locker() {

	try {
		lockable.unlock();
	}
	catch ( Exception& e ) {
		msgErr( "~Locker(): can't auto-unlock mutex!" );
		msgErr( e.what() );
	}

}

// ============================================================================

