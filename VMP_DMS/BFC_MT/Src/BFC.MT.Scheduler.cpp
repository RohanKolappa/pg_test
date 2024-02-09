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
//	BFC.MT.Scheduler.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

//#include "BFC.Base.StackTrace.h"

#include "BFC.MT.Locker.h"
#include "BFC.MT.Scheduler.h"
#include "BFC.MT.TaskletRunner.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( MT, Scheduler )

// ============================================================================

MT::Scheduler::Scheduler() {

	wrk	= new Worker;

}

// ============================================================================

void MT::Scheduler::addTask(
		TaskletPtr	pTask,
	const	Bool		pMaster ) {

	addTask( Time::now(), pTask, pMaster );

}

void MT::Scheduler::addTask(
	const	Time::Clock	pWhen,
		TaskletPtr	pTask,
	const	Bool		pMaster ) {

	EntryPtr e = new Entry;

	e->when = pWhen;
	e->thrd = new TaskletRunner( pTask );
	e->task = pTask;
	e->mstr = pMaster;

	wrk->insertEntry( e );

}

void MT::Scheduler::addTask(
	const	Time::Delta	pWhen,
		TaskletPtr	pTask,
	const	Bool		pMaster ) {

	addTask( Time::now() + pWhen, pTask, pMaster );

}

// ============================================================================

void MT::Scheduler::addThread(
		ThreadPtr	pThrd,
	const	Bool		pMaster ) {

	addThread( Time::now(), pThrd, pMaster );

}

void MT::Scheduler::addThread(
	const	Time::Clock	pWhen,
		ThreadPtr	pThrd,
	const	Bool		pMaster ) {

	EntryPtr e = new Entry;

	e->when = pWhen;
	e->thrd = pThrd;
	e->mstr = pMaster;

	wrk->insertEntry( e );

}

void MT::Scheduler::addThread(
	const	Time::Delta	pWhen,
		ThreadPtr	pThrd,
	const	Bool		pMaster ) {

	addThread( Time::now() + pWhen, pThrd, pMaster );

}

// ============================================================================

void MT::Scheduler::kill() {

	wrk->kill();

}

// ============================================================================

Bool MT::Scheduler::isEmpty() const {

	return wrk->isEmpty();

}

Bool MT::Scheduler::isDead() const {

	return wrk->isDead();

}

void MT::Scheduler::waitEmpty() {

	wrk->waitEmpty();

}

// ============================================================================
// BFC::MT::Scheduler::Entry
// ============================================================================

BFC_PTR_IMPL_NS_CC( MT, Scheduler, Entry )

// ============================================================================

MT::Scheduler::Entry::~Entry() {

	if ( thrd ) {
		thrd->safeStopAndJoin();
	}

}

// ============================================================================
// BFC::MT::Scheduler::Worker
// ============================================================================

BFC_PTR_IMPL_NS_CC( MT, Scheduler, Worker )

// ============================================================================

MT::Scheduler::Worker::Worker() :

	schedCond	( dataMutex ),
	emptyCond	( dataMutex ),
	finishing	( false ) {

	setObjectName( "BFC.MT.Scheduler.Worker" );

	start();

}

MT::Scheduler::Worker::~Worker() {

	safeStopAndJoin();

}

// ============================================================================

void MT::Scheduler::Worker::insertEntry(
		EntryPtr	e ) {

	Locker locker( dataMutex );

	if ( finishing ) {
		throw InternalError( "Finalized!" );
	}

	Uint32 i = 0;

	while ( i < waiting.getSize() && waiting[ i ]->when <= e->when ) {
		i++;
	}

	if ( i < waiting.getSize() ) {
		waiting.insert( i, e );
	}
	else {
		waiting.add( e );
	}

	schedCond.signal();

}

void MT::Scheduler::Worker::kill() {

//	msgDbg( "kill():\n" + StackTrace::dump( 0 ) );

	Locker locker( dataMutex );

	finishing = true;

	schedCond.signal();

}

Bool MT::Scheduler::Worker::isEmpty() const {

	return ( waiting.isEmpty()
	      && running.isEmpty() );

}

Bool MT::Scheduler::Worker::isDead() const {

	return ( finishing
	      && waiting.isEmpty()
	      && running.isEmpty() );

}

void MT::Scheduler::Worker::waitEmpty() {

	Locker locker( dataMutex );

	while ( waiting.hasContent()
	     || running.hasContent() ) {
		emptyCond.wait();
	}

}

// ============================================================================

void MT::Scheduler::Worker::run() {

//	msgDbg( "run(): --->" );

	try {

		for (;;) {

//			msgDbg( "run(): loop!" );

			if ( shouldStop() ) {
//				msgDbg( "run(): stopped!" );
				break;
			}

			Locker locker( dataMutex );

			if ( finishing ) {
//				msgDbg( "run(): finalizing now..." );
				break;
			}

			cleanup();

			while ( ! shouldStop()
			     && ! finishing
			     && ! waiting.isEmpty()
			     && waiting[ 0 ]->when <= Time::now() ) {
				EntryPtr e = waiting[ 0 ];
				waiting.pop( 0 );
//				msgDbg( "Starting thread \""
//					+ e->thrd->getObjectName()
//					+ "\"..." );
				e->thrd->start();
				running += e;
			}

			while ( ! shouldStop()
			     && ! finishing
			     && waiting.isEmpty() ) {
				try {
					schedCond.timedWait( Time::Delta( 1, 0 ) );
				}
				catch ( ThreadStopped & ) {
//					msgDbg( "run(): stopped while waiting 1 sec!" );
					break;
				}
				cleanup();
			}

			while ( ! shouldStop()
			     && ! finishing
			     && ! waiting.isEmpty()
			     && waiting[ 0 ]->when > Time::now() ) {
				Time::Clock maxWait = Time::now() + Time::Delta( 1, 0 );
				if ( waiting[ 0 ]->when < maxWait ) {
					maxWait = waiting[ 0 ]->when;
				}
				try {
					schedCond.timedWait( maxWait );
				}
				catch ( ThreadStopped & ) {
//					msgDbg( "run(): stopped while waiting on item!" );
					break;
				}
			}

		}

		Locker locker( dataMutex );

//		msgDbg( "run(): finalizing..." );

		if ( waiting.hasContent() ) {
//			msgDbg( "run(): removing " + Buffer( waiting.getSize() )
//					+ " waiting tasks..." );
			waiting.kill();
		}

		if ( running.hasContent() ) {
//			msgDbg( "run(): killing " + Buffer( running.getSize() )
//					+ " running tasks..." );
			for ( Uint32 i = 0 ; i < running.getSize() ; i++ ) {
				if ( running[ i ] && running[ i ]->thrd ) {
//					msgDbg( "run(): killing \"" + running[ i ]->thrd->getObjectName() + "\"..." );
					running[ i ]->thrd->safeStopAndJoin();
				}
			}
			running.kill();
		}

		finishing = true;
		emptyCond.signal();

	}
	catch ( ThreadStopped & ) {
	}
	catch ( Exception & e ) {
		msgExc( e, "Scheduler problem!" );
	}

//	msgDbg( "run(): <---" );

}

// ============================================================================

void MT::Scheduler::Worker::cleanup() {

	if ( running.isEmpty() ) {
		return;
	}

	for ( Uint32 i = 0 ; i < running.getSize() ; ) {

		EntryPtr e = running[ i ];

		if ( e->thrd->isWaiting() ) {
//			msgDbg( "cleanup(): Joining thread \""
//				+ e->thrd->getObjectName()
//				+ "\"..." );
			Exception * exc = 0;
			try {
				exc = e->thrd->join();
			}
			catch ( Exception & e ) {
				exc = 0;
				msgExc( e, "Can't join runner!" );
			}
			if ( exc ) {
				msgExc( *exc, "Runner exception!" );
				delete exc;
			}
//			msgDbg( "cleanup(): Joined thread \""
//				+ e->thrd->getObjectName()
//				+ "\"..." );
			if ( e->mstr ) {
//				msgDbg( "cleanup(): master exited --> finalizing!" );
				finishing = true;
			}
			running.pop( i );
		}
		else {
			i++;
		}
	}

	if ( waiting.isEmpty()
	  && running.isEmpty() ) {
//		msgDbg( "cleanup(): nothing left behind!" );
		emptyCond.signal();
	}

}

// ============================================================================

