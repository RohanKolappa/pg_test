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
//	BFC.SysMutex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

#include <windows.h>

#include "BFC.MT.SysMutex.h"
#include "BFC.MT.SysMutexData.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::SysMutex::SysMutex(
	const	Type		pMutxType ) :
	
	type	( pMutxType ),
	data	( 0 ) {

	try {
		initMutex();
	}
	catch ( Exception & ) {
		// Do something useful here!
	}

}

MT::SysMutex::~SysMutex() {

	if ( data ) {
		DeleteCriticalSection( &data->critSect );
		delete data;
	}

}

// ============================================================================

void MT::SysMutex::lock() {

	if ( ! data ) {
		throw InternalError( "lock(): not initialized!" );
	}

	EnterCriticalSection( &data->critSect );

}

void MT::SysMutex::unlock() {

	if ( ! data ) {
		throw InternalError( "unlock(): not initialized!" );
	}

	LeaveCriticalSection( &data->critSect );

}

// ============================================================================

void MT::SysMutex::initMutex() {

	if ( type != Normal ) {
		throw InternalError( "Invalid type!" );
	}

	data = new Data;

	InitializeCriticalSection ( &data->critSect );

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

