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
//	BFC.EmulSynchronizer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.MT.EmulCondition.h"
#include "BFC.MT.EmulMutex.h"
#include "BFC.MT.EmulSynchronizer.h"

#include "BFC.Time.Delta.h"
#include "BFC.Time.Clock.h"

// ============================================================================

using namespace BFC;

// ============================================================================

/// \brief Represents the data associated to a single waiting thread.
///
/// An EmulSynchronizer::Data structure is created each and every time a thread
/// enters one of the blocking methods of the EmulSynchronizer class.
///
/// It is then used to wait for a condition to happen, and to record which
/// condition happened, when multiple conditions were waited on.

struct MT::EmulSynchronizer::Data {

	EmulMutex *		mut;
	EmulCondition *		con;
	EmulSynchronizer *	src;

	Data(
	) :
		mut	( 0 ),
		con	( 0 ),
		src	( 0 ) {
	}

	~Data(
	) {
		if ( mut ) {
			delete con;
			delete mut;
		}
	}

	Bool setUnlocker(
			EmulSynchronizer *	unlocker
	) {
		Bool res = false;
		mut->lock();
		if ( ! src ) {
			src = unlocker;
			con->signal();
			res = true;
		}
		mut->unlock();
		return res;
	}

};

// ============================================================================

MT::EmulSynchronizer::EmulSynchronizer(
	const	Bool	pSignaled ,
	const	Bool	pManReset ) :

	signaled	( pSignaled ),
	manReset	( pManReset ),
	lastUsed	( 0 ) {

}

// ============================================================================

void MT::EmulSynchronizer::broadcast() {

	cs.lock();

	signaled = true;

	Uint32	checked = 0;
	Uint32	restarted = 0;

	Data *data;

	for ( Uint32 i = 0 ; i < lastUsed ; i++ ) {
		if ( ( data = blocked[ i ] ) ) {
			checked++;
			if ( data->setUnlocker( this ) ) {
				restarted++;
			}
		}
	}

	if ( ! checked ) {
		lastUsed = 0;
	}

	if ( restarted && ! manReset ) {
		signaled = false;
	}

	cs.unlock();

}

void MT::EmulSynchronizer::signal() {

	cs.lock();

	signaled = true;

	Uint32	checked = 0;
	Uint32	restarted = 0;

	Data *data;

	for ( Uint32 i = 0 ; i < lastUsed ; i++ ) {

		if ( ( data = blocked[i] ) ) {
			checked++;
			if ( data->setUnlocker( this ) ) {
				restarted++;
			}
			if ( restarted ) {
				break;
			}
		}

	}

	if ( ! checked ) {
		lastUsed = 0;
	}

	if ( restarted && ! manReset ) {
		signaled = false;
	}

	cs.unlock();

}

void MT::EmulSynchronizer::pulse() {

	cs.lock();

	signaled = true;

	Uint32	checked = 0;
	Uint32	restarted = 0;

	Data *data;

	for ( Uint32 i = 0 ; i < lastUsed ; i++ ) {

		if ( ( data = blocked[i] ) ) {
			checked++;
			if ( data->setUnlocker( this ) ) {
				restarted++;
			}
			if ( restarted ) {
				break;
			}
		}

	}

	if ( ! checked ) {
		lastUsed = 0;
	}

	signaled = false;

	cs.unlock();

}

void MT::EmulSynchronizer::reset() {

	cs.lock();

	signaled = false;

	cs.unlock();

}

// ============================================================================

void MT::EmulSynchronizer::wait() {

	Data data;

	if ( ! addBlocked( &data, true ) ) {
		data.mut->lock();
		if ( ! data.src ) {
			data.con->wait();
		}
		data.mut->unlock();
		delBlocked( &data );
	}

}

Bool MT::EmulSynchronizer::tryWait() {

	cs.lock();

	Bool res;

	if ( signaled ) {
		if ( ! manReset ) {
			signaled = false;
		}
		res = true;
	}
	else {
		res = false;
	}

	cs.unlock();

	return res;

}

#if defined( PLATFORM_WIN32 )

Bool MT::EmulSynchronizer::timedWait(
	const	Time::Clock &	timeout ) {

	return timedWait( timeout - Time::now() );

}

#else

Bool MT::EmulSynchronizer::timedWait(
	const	Time::Delta &	timeout ) {

	return timedWait( Time::now() + timeout );

}

#endif

Bool MT::EmulSynchronizer::timedWait(
#if defined( PLATFORM_WIN32 )
	const	Time::Delta &	timeout ) {
#else
	const	Time::Clock &	timeout ) {
#endif

	Data data;

	if ( addBlocked( &data, true ) ) {
		return true;
	}

	data.mut->lock();

	Bool res = true;

	if ( ! data.src ) {
		res = data.con->timedWait( timeout );
	}

	if ( ! res ) {
		// Timeout! Prevent another from updating 'src' between the
		// moment we unlock 'data.mut', and the moment we call
		// the 'delBlocked()'!
		data.src = this;
	}

	data.mut->unlock();

	delBlocked( &data );

	return res;

}

// ============================================================================

Uint32 MT::EmulSynchronizer::wait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1 ) {

	Data data;

	// Ask all sync events to warn me if they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false; // keeps compiler happy!

	Uint32 res;

	if ( ( gotSync0 = sync0->addBlocked( &data, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &data, false ) ) ) {
		res = 1;
	}
	else {
		// Now, start waiting on multiple objects!!!
		data.mut->lock();
		if ( ! data.src ) {
			data.con->wait();
		}
		data.mut->unlock();
		EmulSynchronizer *src = data.src;
		if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &data );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &data );
		}
	}

	return res;

}

#if defined( PLATFORM_WIN32 )

Int32 MT::EmulSynchronizer::timedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
	const	Time::Clock&		timeout ) {

	return timedWait( sync0, sync1, timeout - Time::now() );

}

#else

Int32 MT::EmulSynchronizer::timedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
	const	Time::Delta&		timeout ) {

	return timedWait( sync0, sync1, Time::now() + timeout );

}

#endif

Int32 MT::EmulSynchronizer::timedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
#if defined( PLATFORM_WIN32 )
	const	Time::Delta&		timeout ) {
#else
	const	Time::Clock&		timeout ) {
#endif

	Data data;

	// Ask all sync events to warn me if they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false; // keeps compiler happy!

	Int32 res;

	if ( ( gotSync0 = sync0->addBlocked( &data, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &data, false ) ) ) {
		res = 1;
	}
	else {
		// Now, start waiting on multiple objects!!!
		data.mut->lock();
		Bool nto = true;
		if ( ! data.src ) {
			nto = data.con->timedWait( timeout );
		}
		if ( ! nto ) {
			data.src = sync0;
		}
		data.mut->unlock();
		EmulSynchronizer *src = data.src;
		if ( ! nto )
			res = -1;
		else if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &data );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &data );
		}
	}

	return res;

}

// ============================================================================

Uint32 MT::EmulSynchronizer::wait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
		EmulSynchronizer *	sync2 ) {

	Data data;

	// Ask all sync events to warn me if they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false;
	Bool gotSync2 = false;

	Uint32 res;

	if ( ( gotSync0 = sync0->addBlocked( &data, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &data, false ) ) ) {
		res = 1;
	}
	else if ( ( gotSync2 = sync2->addBlocked( &data, false ) ) ) {
		res = 2;
	}
	else {
		// Now, start waiting on multiple objects!!!
		data.mut->lock();
		if ( ! data.src ) {
			data.con->wait();
		}
		data.mut->unlock();
		EmulSynchronizer *src = data.src;
		if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else if ( src == sync2 )
			res = 2;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &data );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &data );
			if ( ! gotSync2 ) {
				sync2->delBlocked( &data );
			}
		}
	}

	return res;

}

// ============================================================================

void MT::EmulSynchronizer::signalAndWait(
		EmulSynchronizer *	sync0 ) {

	Data waitData;

	// Prepare all the sync events to warn me when they get signaled!

	Bool gotSync0;

	Bool gotIt = false;

	if ( ( gotSync0 = sync0->addBlocked( &waitData, true ) ) ) {
		gotIt = true;
	}

	// Now, signal the condition!

	signal();

	if ( ! gotIt ) {
		// Now, start waiting on multiple objects!!!
		waitData.mut->lock();
		if ( ! waitData.src ) {
			waitData.con->wait();
		}
		waitData.mut->unlock();
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &waitData );
	}

}

#if defined( PLATFORM_WIN32 )

Bool MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
	const	Time::Clock&		timeout ) {

	return signalAndTimedWait( sync0, timeout - Time::now() );

}

#else

Bool MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
	const	Time::Delta&		timeout ) {

	return signalAndTimedWait( sync0, Time::now() + timeout );

}

#endif

Bool MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
#if defined( PLATFORM_WIN32 )
	const	Time::Delta&		timeout ) {
#else
	const	Time::Clock&		timeout ) {
#endif

	Data waitData;

	// Prepare all the sync events to warn me when they get signaled!

	Bool gotSync0;
	Bool res = true;
	Bool gotIt = false;

	if ( ( gotSync0 = sync0->addBlocked( &waitData, true ) ) ) {
		gotIt = true;
	}

	// Now, signal the condition!

	signal();

	if ( ! gotIt ) {
		// Now, start waiting on multiple objects!!!
		waitData.mut->lock();
		if ( ! waitData.src ) {
			res = waitData.con->timedWait( timeout );
		}
		if ( ! res ) {
			waitData.src = sync0;
		}
		waitData.mut->unlock();
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &waitData );
	}

	return res;

}

// ============================================================================

Uint32 MT::EmulSynchronizer::signalAndWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1 ) {

	Data waitData;

	// Prepare all the sync events to warn me when they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false;

	Bool gotIt = true;

	Uint32 res = (Uint32)-1; // keeps compiler happy!

	if ( ( gotSync0 = sync0->addBlocked( &waitData, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &waitData, false ) ) ) {
		res = 1;
	}
	else {
		gotIt = false;
	}

	// Now, signal the condition!

	signal();

	if ( ! gotIt ) {
		// Now, start waiting on multiple objects!!!
		waitData.mut->lock();
		if ( ! waitData.src ) {
			waitData.con->wait();
		}
		waitData.mut->unlock();
		EmulSynchronizer *src = waitData.src;
		if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &waitData );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &waitData );
		}
	}

	return res;

}

#if defined( PLATFORM_WIN32 )

Int32 MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
	const	Time::Clock&		timeout ) {

	return signalAndTimedWait( sync0, sync1, timeout - Time::now() );

}

#else

Int32 MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
	const	Time::Delta&		timeout ) {

	return signalAndTimedWait( sync0, sync1, Time::now() + timeout );

}

#endif

Int32 MT::EmulSynchronizer::signalAndTimedWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
#if defined( PLATFORM_WIN32 )
	const	Time::Delta&		timeout ) {
#else
	const	Time::Clock&		timeout ) {
#endif

	Data waitData;

	// Prepare all the sync events to warn me when they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false;

	Bool gotIt = true;

	Int32 res = -1; // keeps compiler happy!

	if ( ( gotSync0 = sync0->addBlocked( &waitData, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &waitData, false ) ) ) {
		res = 1;
	}
	else {
		gotIt = false;
	}

	// Now, signal the condition!

	signal();

	if ( ! gotIt ) {
		// Now, start waiting on multiple objects!!!
		waitData.mut->lock();
		Bool nto = true;
		if ( ! waitData.src ) {
			nto = waitData.con->timedWait( timeout );
		}
		if ( ! nto ) {
			waitData.src = sync0;
		}
		waitData.mut->unlock();
		EmulSynchronizer *src = waitData.src;
		if ( ! nto )
			res = -1;
		else if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &waitData );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &waitData );
		}
	}

	return res;

}

// ============================================================================

Uint32 MT::EmulSynchronizer::signalAndWait(
		EmulSynchronizer *	sync0,
		EmulSynchronizer *	sync1,
		EmulSynchronizer *	sync2 ) {

	Data waitData;

	// Prepare all the sync events to warn me when they get signaled!

	Bool gotSync0;
	Bool gotSync1 = false;
	Bool gotSync2 = false;

	Bool gotIt = true;

	Uint32 res = (Uint32)-1;

	if ( ( gotSync0 = sync0->addBlocked( &waitData, true ) ) ) {
		res = 0;
	}
	else if ( ( gotSync1 = sync1->addBlocked( &waitData, false ) ) ) {
		res = 1;
	}
	else if ( ( gotSync2 = sync2->addBlocked( &waitData, false ) ) ) {
		res = 2;
	}
	else {
		gotIt = false;
	}

	// Now, signal the condition!

	signal();

	// Now, start waiting on multiple objects!!!

	if ( ! gotIt ) {
		waitData.mut->lock();
		if ( ! waitData.src ) {
			waitData.con->wait();
		}
		waitData.mut->unlock();
		EmulSynchronizer *src = waitData.src;
		if ( src == sync0 )
			res = 0;
		else if ( src == sync1 )
			res = 1;
		else if ( src == sync2 )
			res = 2;
		else
			throw InternalError( "Can't find source anymore!" );
	}

	// Remove all references to me...

	if ( ! gotSync0 ) {
		sync0->delBlocked( &waitData );
		if ( ! gotSync1 ) {
			sync1->delBlocked( &waitData );
			if ( ! gotSync2 ) {
				sync2->delBlocked( &waitData );
			}
		}
	}

	return res;

}

// ============================================================================

Bool MT::EmulSynchronizer::addBlocked(
		Data *		data,
		Bool		first ) {

	cs.lock();

	if ( signaled ) {
		if ( first ) {
			if ( ! manReset ) {
				signaled = false;
			}
			cs.unlock();
			return true;
		}
		data->mut->lock();
		if ( ! data->src ) {
			data->src = this;
			if ( ! manReset ) {
				signaled = false;
			}
			data->mut->unlock();
			cs.unlock();
			return true;
		}
		data->mut->unlock();
	}
	else if ( first ) {
		data->mut = new EmulMutex;
		data->con = new EmulCondition( *data->mut );
	}

	Uint32 i;

	for ( i = 0 ; i < lastUsed ; i++ ) {
		if ( ! blocked[i] ) {
			blocked[i] = data;
			break;
		}
	}

	if ( i >= lastUsed ) {
		if ( lastUsed >= MAX_BLOCKED ) {
			cs.unlock();
			throw InternalError("Too many waiters!");
		}
		blocked[lastUsed++] = data;
	}

	cs.unlock();

	return false;

}

void MT::EmulSynchronizer::delBlocked(
		Data *		data ) {

	cs.lock();

	for ( Uint32 i = 0 ; i < lastUsed ; i++ ) {
		if ( blocked[i] == data ) {
			blocked[i] = 0;
			break;
		}
	}

	cs.unlock();

}

// ============================================================================

