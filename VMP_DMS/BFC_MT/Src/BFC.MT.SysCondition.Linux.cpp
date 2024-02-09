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
//	BFC.SysCondition.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_LINUX ) || defined( PLATFORM_DARWIN )

// ============================================================================

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#include "BFC.Time.Delta.h"
#include "BFC.Time.Clock.h"

#include "BFC.MT.SysCondition.h"
#include "BFC.MT.SysMutex.h"
#include "BFC.MT.SysMutexData.h"

// ============================================================================

using namespace BFC;

// ============================================================================

struct MT::SysCondition::Data {

	pthread_cond_t		cond;
	pthread_condattr_t	attr;

};

// ============================================================================

MT::SysCondition::SysCondition(
		SysMutex &	pShrdMutx ) :
		
	shrdMutx	( pShrdMutx ) {

	condData = new Data;

	pthread_condattr_init( &condData->attr );
	pthread_cond_init( &condData->cond, &condData->attr );

}

MT::SysCondition::~SysCondition() {

	pthread_cond_destroy( &condData->cond );
	pthread_condattr_destroy( &condData->attr );

	delete condData;

}

// ============================================================================

void MT::SysCondition::signal() {

	int res;

	if ( ( res = pthread_cond_signal( &condData->cond ) ) ) {
		throw InternalError( Buffer( "Can't pthread_cond_signal()! " ) + ::strerror( res ) );
	}

}

void MT::SysCondition::broadcast() {

	int res;

	if ( ( res = pthread_cond_broadcast( &condData->cond ) ) ) {
		throw InternalError( Buffer( "Can't pthread_cond_broadcast()! " ) + ::strerror( res ) );
	}

}

// ============================================================================

void MT::SysCondition::wait() {

	int res = pthread_cond_wait(
		&condData->cond,
		&shrdMutx.data->mutex );

	switch ( res ) {

	case 0 :
		return;

	default :
		throw InternalError( Buffer( "Can't pthread_cond_wait()!" ) );

	}

}

Bool MT::SysCondition::timedWait(
	const	Time::Delta &	d ) {

	return timedWait( Time::now() + d );

}

Bool MT::SysCondition::timedWait(
	const	Time::Clock &	limit ) {

	int	res;

	Uint64 u = (Uint64)limit;

	struct timespec to; // tv_sec: secs, tv_nsec: nanos

	to.tv_sec  = (Uint32)(( u / 1000000 )       );
	to.tv_nsec = (Uint32)(( u % 1000000 ) * 1000);

	res = pthread_cond_timedwait(
		&condData->cond,
		&shrdMutx.data->mutex,
		&to );

	switch ( res ) {

	case 0 :	// condition has been signaled!
		return true;

	case EINTR :
	case ETIMEDOUT :
		return false;

	default :
		throw InternalError( Buffer( "Can't pthread_cond_timedwait()!" ) );

	}

}

// ============================================================================

void MT::SysCondition::lock() {

	shrdMutx.lock();

}

void MT::SysCondition::unlock() {

	shrdMutx.unlock();

}

// ============================================================================

#endif // PLATFORM_LINUX || PLATFORM_DARWIN

// ============================================================================
