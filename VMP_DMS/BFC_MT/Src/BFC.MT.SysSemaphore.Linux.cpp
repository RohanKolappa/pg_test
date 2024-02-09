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
//	BFC.SysSemaphore.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined(PLATFORM_LINUX)

// ============================================================================

#include <errno.h>
#include <semaphore.h>
#include <string.h>

#include "BFC.MT.SysSemaphore.h"

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysSemaphoreData {
	sem_t			sem;
};

// ============================================================================

MT::SysSemaphore::SysSemaphore(
	const	Uint32		initialValue) {

	data = new SysSemaphoreData;

	sem_init( &data->sem, 0, initialValue );

}

MT::SysSemaphore::~SysSemaphore() {

	sem_destroy( &data->sem );

	delete data;

}

// ============================================================================

void MT::SysSemaphore::wait() {

	int res;

	if ( ( res = sem_wait( &data->sem ) ) ) {
		throw CantWait( strerror( errno ) );
	}

}

Bool MT::SysSemaphore::tryWait() {

	return ( sem_trywait( &data->sem ) == 0 );

}

void MT::SysSemaphore::post() {

	if ( sem_post( &data->sem ) ) {
		throw CantPost( strerror( errno ) );
	}

}

// ============================================================================

#endif // PLATFORM_LINUX

// ============================================================================

