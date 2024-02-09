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
//	BFC.SysSyncEvent.cpp
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

#include <iostream>

#include "BFC.Base.Buffer.h"

#include "BFC.MT.SysSyncEvent.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

struct MT::SysSyncEvent::Data {

	HANDLE		h;

};

// ============================================================================

MT::SysSyncEvent::SysSyncEvent(
	const	Bool		pSignaled,
	const	Bool		pManReset ) {

	data = new Data;

	data->h = CreateEvent(
		NULL,		// no security attributes
		pManReset,	// manual reset needed ?
		pSignaled,	// initially signaled ?
		NULL );		// no event name

	if ( data->h == NULL ) {
		cerr	<< "INTERNAL ERROR" << endl
			<< "Can't create event!" << endl
			<< strerror( GetLastError() ) << endl;
	}

}

MT::SysSyncEvent::~SysSyncEvent() {

	if ( data->h ) {
		CloseHandle( data->h );
	}

	delete data;

}

// ============================================================================

void MT::SysSyncEvent::set() {

	if ( ! SetEvent( data->h ) ) {
		throw InternalError( Buffer( "Can't SetEvent()! " ) + strerror( GetLastError() ) );
	}

}

void MT::SysSyncEvent::reset() {

	if ( ! ResetEvent( data->h ) ) {
		throw InternalError( Buffer( "Can't ResetEvent()! " ) + strerror( GetLastError() ) );
	}

}

// ============================================================================

void MT::SysSyncEvent::wait() {

	switch ( WaitForSingleObject( data->h, INFINITE ) ) {

	case WAIT_OBJECT_0 :
		return;

	case WAIT_FAILED :
		throw InternalError( Buffer( "Can't WaitForSingleObject()! " ) + strerror( GetLastError() ) );

	default :
		throw InternalError( Buffer( "Can't WaitForSingleObject()! " ) );

	}

}

Uint32 MT::SysSyncEvent::wait(
		SysSyncEvent &	ev1,
		SysSyncEvent &	ev2 ) {

	const HANDLE handles[ 2 ] = { ev1.data->h, ev2.data->h };

	switch ( WaitForMultipleObjects( 2, handles, FALSE, INFINITE ) ) {

	case WAIT_OBJECT_0 :
		return 0;

	case WAIT_OBJECT_0 + 1 :
		return 1;

	default :
		throw InternalError( Buffer( "Can't WaitForMultipleObjects()! " ) );

	}

}

Bool MT::SysSyncEvent::timedWait( 
	const	Uint32		pWaitMSec ) {

	if ( ! pWaitMSec ) {
		throw InvalidArgument();
	}

	switch ( WaitForSingleObject( data->h, pWaitMSec ) ) {

	case WAIT_OBJECT_0 :
		return true;

	case WAIT_TIMEOUT :
		return false;

	case WAIT_FAILED :
		throw InternalError( Buffer( "Can't WaitForSingleObject()! " ) + strerror( GetLastError() ) );

	default :
		throw InternalError( Buffer( "Can't WaitForSingleObject()!" ) );

	}

}

void MT::SysSyncEvent::signalAndWaitOn( 
		SysSyncEvent &	pToWaitOn ) {

	switch ( SignalObjectAndWait( data->h, pToWaitOn.data->h, INFINITE, FALSE ) ) {

	case WAIT_OBJECT_0 :
		return;

	case WAIT_FAILED :
		throw InternalError( Buffer( "Can't SignalObjectAndWait()! " ) + strerror( GetLastError() ) );

	default :
		throw InternalError( Buffer( "Can't SignalObjectAndWait()!" ) );

	}

}

Bool MT::SysSyncEvent::signalAndTimedWaitOn( 
		SysSyncEvent &	pToWaitOn,
	const	Uint32		pWaitMSec ) {

	if ( ! pWaitMSec ) {
		throw InvalidArgument();
	}

	switch ( SignalObjectAndWait( data->h, pToWaitOn.data->h, pWaitMSec, FALSE ) ) {

	case WAIT_OBJECT_0 :
		return true;

	case WAIT_TIMEOUT :
		return false;

	case WAIT_FAILED :
		throw InternalError( Buffer( "Can't SignalObjectAndWait()! " ) + strerror( GetLastError() ) );

	default :
		throw InternalError( Buffer( "Can't SignalObjectAndWait()!" ) );

	}

}

// ============================================================================

void MT::SysSyncEvent::lock() {

	wait();

}

void MT::SysSyncEvent::unlock() {

	set();

}

// ============================================================================

#endif // PLATFORM_WIN32

// ============================================================================

