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
//	BFC.MT.SysSemaphore.Win32.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Christophe Herreman
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

// ============================================================================

//#ifndef _WIN32_WINNT
//#define _WIN32_WINNT 0x0400
//#endif

#include <iostream>
#include <windows.h>

#include "BFC.MT.SysSemaphore.h"

#define MAX_SEMAPHORE_VALUE 2000

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysSemaphoreData {
	HANDLE		h;
};

// ============================================================================

MT::SysSemaphore::SysSemaphore(
	const	Uint32		initialValue) {

	data = new SysSemaphoreData;

	data->h = CreateSemaphore(NULL, initialValue, MAX_SEMAPHORE_VALUE, NULL);

	if ( data->h == NULL ) {
		std::cerr << "INTERNAL ERROR" << std::endl;
		std::cerr << "Can't create semaphore!" << std::endl;
		std::cerr << strerror(GetLastError()) << std::endl;
	}

}

MT::SysSemaphore::~SysSemaphore() {

	CloseHandle( data->h );

	data->h = NULL;

	delete data;

}

// ============================================================================

void MT::SysSemaphore::wait() {

	Uint32 res;

	res = WaitForSingleObject( data->h, INFINITE );

	if (res != WAIT_OBJECT_0) {
		throw CantWait( strerror( GetLastError() ) );	
	}

}

Bool MT::SysSemaphore::tryWait() {

	Uint32 res;

	res = WaitForSingleObject( data->h, 0 );

	switch (res) {
	case WAIT_OBJECT_0:
		return true;
	case WAIT_TIMEOUT:
		return false;
	default:
		throw CantWait( strerror( GetLastError() ) );	
	}

}

void MT::SysSemaphore::post() {

	if ( ReleaseSemaphore( data->h, 1, NULL ) == 0 ) {
		throw CantPost( strerror( GetLastError() ) );
	}

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

