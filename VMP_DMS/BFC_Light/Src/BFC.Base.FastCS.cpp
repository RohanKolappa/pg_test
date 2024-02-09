// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.FastCS.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

//#if ! defined( _WIN32_WINNT )
//#define _WIN32_WINNT 0x0400
//#endif

#include <windows.h>

#elif defined( PLATFORM_DARWIN )

#include <errno.h>
#include <pthread.h>

#else

#include <errno.h>
#include <semaphore.h>
#include <string.h>

#endif

#include "BFC.Base.FastCS.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_WIN32 )

class BFC::FastCS::Data {
public :
	Data() { InitializeCriticalSection( &h ); }
	~Data() { DeleteCriticalSection( &h ); }
	void enter() { EnterCriticalSection( &h ); }
	void leave() { LeaveCriticalSection( &h ); }
private :
	CRITICAL_SECTION	h;
};

#elif defined( PLATFORM_DARWIN )

class BFC::FastCS::Data {
	public :
	Data() { pthread_mutex_init(&mHandle, NULL); }
	~Data() { pthread_mutex_destroy(&mHandle); }
	void enter() { pthread_mutex_lock(&mHandle); }
	void leave() { pthread_mutex_unlock(&mHandle); }
	private :
	pthread_mutex_t mHandle;
};

#else

class BFC::FastCS::Data {
public :
	Data() { ::sem_init( &sem, 0, 1 ); }
	~Data() { ::sem_destroy( &sem ); }
	void enter() { ::sem_wait( &sem ); }
	void leave() { ::sem_post( &sem ); }
private :
	sem_t			sem;
};

#endif

// ============================================================================

FastCS::FastCS() {

	data = new Data;

}

FastCS::~FastCS() {

	delete data;

}

// ============================================================================

void FastCS::enter() {

	data->enter();

}

void FastCS::leave() {

	data->leave();

}

// ============================================================================

