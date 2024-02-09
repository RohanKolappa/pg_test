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
//	BFC.EmulMutex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.EmulMutex.h"

// ============================================================================

using namespace BFC;

//// ============================================================================
//
//#if defined( PLATFORM_WIN32 )
//
//// ============================================================================
//
//MT::EmulMutex::EmulMutex()
//	: unlocked ( true, false )
//	, cache( false ) {		// signaled, auto reset
//
//}
//
//MT::EmulMutex::~EmulMutex() {
//
//}
//
//// ============================================================================
//
//void MT::EmulMutex::lock() {
//
//	unlocked.wait();
//	cache = true;
//
//}
//
//void MT::EmulMutex::unlock() {
//
//	cache = false;
//	unlocked.set();
//
//}
//
//// ============================================================================
//
//#else

// ============================================================================

MT::EmulMutex::EmulMutex() :

	mutex(
#if defined( PLATFORM_LINUX )
		SysMutex::Fast ) {
#else
		SysMutex::Normal ) {
#endif

}

MT::EmulMutex::~EmulMutex() {

}

// ============================================================================

void MT::EmulMutex::lock() {

	mutex.lock();

}

void MT::EmulMutex::unlock() {

	mutex.unlock();

}

//// ============================================================================
//
//#endif // PLATFORM_LINUX

// ============================================================================

