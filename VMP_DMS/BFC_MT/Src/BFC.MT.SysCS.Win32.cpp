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
//	BFC.SysCS.cpp
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

#include "BFC.MT.SysCS.h"

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysCSData {

	CRITICAL_SECTION		h;

};

// ============================================================================

MT::SysCS::SysCS() {

	data = new SysCSData;

	InitializeCriticalSection( &data->h );

}

MT::SysCS::~SysCS() {

	DeleteCriticalSection( &data->h );

	delete data;

}

// ============================================================================

void MT::SysCS::lock() {

	EnterCriticalSection( &data->h );

}

Bool MT::SysCS::tryLock() {

	return ( TryEnterCriticalSection( &data->h ) != 0 );

}

void MT::SysCS::unlock() {

	LeaveCriticalSection( &data->h );

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

