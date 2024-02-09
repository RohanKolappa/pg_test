// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Base.FastCS.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )

#if ! defined( _WIN32_WINNT )
#define _WIN32_WINNT 0x0400
#endif

#include <windows.h>

#else

#include <errno.h>
#include <semaphore.h>
#include <string.h>

#endif

#include "TBFC.Base.FastCS.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

#if defined( PLATFORM_WIN32 )

class TBFC::FastCSData {
public :
	FastCSData() { InitializeCriticalSection( &h ); }
	~FastCSData() { DeleteCriticalSection( &h ); }
	void enter() { EnterCriticalSection( &h ); }
	void leave() { LeaveCriticalSection( &h ); }
private :
	CRITICAL_SECTION	h;
};

#else

class TBFC::FastCSData {
public :
	FastCSData() { ::sem_init( &sem, 0, 1 ); }
	~FastCSData() { ::sem_destroy( &sem ); }
	void enter() { ::sem_wait( &sem ); }
	void leave() { ::sem_post( &sem ); }
private :
	sem_t			sem;
};

#endif

// ============================================================================

FastCS::FastCS() {

	data = new FastCSData;

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

