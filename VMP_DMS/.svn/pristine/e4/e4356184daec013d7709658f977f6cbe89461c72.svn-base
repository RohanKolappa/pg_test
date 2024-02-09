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

#if defined( PLATFORM_LINUX ) || defined( PLATFORM_CYGWIN ) || defined( PLATFORM_DARWIN )

// ============================================================================

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "BFC.Base.Buffer.h"

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

		pthread_mutex_destroy( &data->mutex );
		pthread_mutexattr_destroy( &data->attr );

		delete data;

	}

}

// ============================================================================

void MT::SysMutex::lock() {

	if ( ! data ) {
		throw InternalError( "lock(): not initialized!" );
	}

	int res;

	switch ( ( res = pthread_mutex_lock( &data->mutex ) ) ) {

	case 0 :
		break;

	case EDEADLK :
		throw WouldDeadLock();

	default :
		throw InternalError( strerror( res ) );

	}

}

void MT::SysMutex::unlock() {

	if ( ! data ) {
		throw InternalError( "unlock(): not initialized!" );
	}

	int res;

	switch ( ( res = pthread_mutex_unlock( &data->mutex ) ) ) {

	case 0 :
		break;

	case EPERM :
		throw NotOwner();

	default :
		throw InternalError( strerror( res ) );

	}

}

// ============================================================================

void MT::SysMutex::initMutex() {

	int ptype[] = {
		PTHREAD_MUTEX_NORMAL,
		PTHREAD_MUTEX_RECURSIVE,
		PTHREAD_MUTEX_ERRORCHECK
#if defined( PLATFORM_LINUX )
		, PTHREAD_MUTEX_FAST_NP
#endif
	};

	data = new Data;

	int pt = ptype[type];
	int res;

	if ( ( res = pthread_mutexattr_init(
			&data->attr ) ) ) {
		throw InternalError( Buffer( "Can't pthread_mutexattr_init()! " ) + strerror( res ) );
	}

	if ( ( res = pthread_mutexattr_settype(
			&data->attr,
			pt ) ) ) {
		throw InternalError( Buffer( "Can't pthread_mutexattr_settype()! " ) + strerror( res ) );
	}

	if ( ( res = pthread_mutex_init(
			&data->mutex,
			&data->attr ) ) ) {
		throw InternalError( Buffer( "Can't pthread_mutex_init()! " ) + strerror( res ) );
	}

}

// ============================================================================

#endif // PLATFORM_LINUX || PLATFORM_CYGWIN || PLATFORM_DARWIN

// ============================================================================

