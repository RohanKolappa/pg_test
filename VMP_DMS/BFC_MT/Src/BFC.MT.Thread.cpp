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
//	BFC.Thread.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )
#  include <process.h>
#  include <signal.h>
#  include <windows.h>
#else
#  include <pthread.h>
#  include <signal.h>
#  include <string.h>
#  include <unistd.h>
#  include <sys/types.h>
#endif
//#include <iostream>

#include "Sys.MT.ThreadCreator.h"
#include "Sys.MT.ThreadData.h"

//#include "BFC.Base.StackTrace.h"
#include "BFC.Base.System.h"

#include "BFC.MT.MainThread.h"
#include "BFC.MT.RWLocker.h"
#include "BFC.MT.SysLocker.h"
#include "BFC.MT.Thread.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( MT, Thread )

// ============================================================================

MT::Thread::Thread() :

	currState	( Stopped ),
	stateCond	( stateMutx ),
	shdStpFlg	( false ),
	shdStpEvt	( false, true ) {

//	cerr << "Thread(): this == "
//		<< ( void * )this
//		<< endl;

	localData = new Sys::MT::ThreadData;

}

MT::Thread::~Thread() {

	safeStopAndJoin();

	delete localData;

}

// ============================================================================

void MT::Thread::start() {

	{
		SysLocker statusLocker( stateMutx );
		switch ( currState ) {
		case Stopped :
			currState = Launching;
			shdStpFlg = false;
			shdStpEvt.reset();
			break;
		case Launching :
		case Running :
			throw InternalError( "Already running!" );
		case Waiting :
			throw InternalError( "Waiting to be joined!" );
		}
	}

	void * arg = static_cast< void * >( this );

	try {
		Sys::MT::ThreadCreator::instance()->create(
			localData,
			launcher,
			arg );
	}
	catch ( Exception & e ) {
		{
			SysLocker locker( stateCond );
			currState = Stopped;
		}
		throw InternalError( e, "Can't create!" );
	}

	{
		SysLocker statusLocker( stateMutx );
		while ( currState == Launching ) {
			stateCond.wait();
		}
	}

}

// ============================================================================

void MT::Thread::stop() {

//	msgDbg( "stop():\n" + StackTrace::dump( 0 ) );

	// Let's send a SIGUSR1 to the relevant thread...

	switch ( getStatus() ) {

	case Stopped :
	case Waiting :
		// Nothing to be done here!...
		return;

	case Launching :
	case Running :
		shdStpFlg = true;
		shdStpEvt.broadcast();
		abort();
#if ! defined( PLATFORM_WIN32 )
		::pthread_kill( localData->id, SIGUSR1 );
#endif

	}

}

// ============================================================================

Exception * MT::Thread::join() {

	{
		SysLocker statusLocker( stateCond );
		if ( currState == Stopped ) {
			throw InternalError( "Not running!" );
		}
		if ( currState < Waiting ) {
			try {
				stateCond.wait();
			}
			catch ( Exception & e ) {
				throw InternalError( e, "Can't join!" );
			}
		}
		if ( currState < Waiting ) {
			throw InternalError( "Canceled" );
		}
		currState = Stopped;
	}

	void * ret = 0;

#if ! defined( PLATFORM_WIN32 )

	int res;

	if ( ( res = ::pthread_join( localData->id, &ret ) ) != 0 ) {
		msgWrn( "Can't join! (" + Buffer( ::strerror( res ) ) + ")" );
	}

#endif

	return ( ret ? static_cast< Exception * >( ret ) : 0 );

}

// ============================================================================

void MT::Thread::safeStopAndJoin() {

	Exception * exc = 0;

	switch ( currState ) {
	case Stopped :
		break;
	case Running :
		try {
			stop();
		}
		catch ( Exception & ) {
		}
	case Launching :
	case Waiting :
		try {
			exc = join();
			if ( exc ) {
				delete exc;
			}
		}
		catch ( Exception & ) {
		}
	}

}

// ============================================================================

void MT::Thread::exit(
	Int32		/* code */) {

#if defined( PLATFORM_WIN32 )
	::_endthread();
#else
	::pthread_exit( 0 );
#endif

}

// ============================================================================

MT::Thread::Status MT::Thread::getStatus() const {

	SysLocker locker( stateMutx );

	return currState;

}

const char * MT::Thread::getStatusName(
	const	Status		s ) {

	static const char *table[] = {
		"Stopped",
		"Launching",
		"Running",
		"Waiting"
	};

	return table[ s ];

}

// ============================================================================

Uint32 MT::Thread::getThreadPID() const {

	return ( Uint32 )localData->pid;

}

// ============================================================================

MT::Thread * MT::Thread::self() {

	return getTLS();

}

// ============================================================================

#if defined( PLATFORM_WIN32 )
void MT::Thread::launcher(
#else
void * MT::Thread::launcher(
#endif
		void		* arg ) {

	// Now, we are in the context of the new executing thread!

	Thread * t = static_cast< Thread * >( arg );

	setTLS( t );
	
#if defined( PLATFORM_WIN32 )

//	t->localData->pid = _getpid();
	t->localData->pid = GetCurrentThreadId();

	::signal( SIGTERM, SIG_IGN );

#else

	t->localData->pid = ::getpid();

	// We don't want to be interrupted by spurious signals...

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP );
//	sigaddset(&mask, SIGINT );
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);

	::pthread_sigmask(SIG_BLOCK, &mask, 0);

#endif

//	Int32 pid = t->localData->pid;

//	t->msgDbg( "launcher(): launchind PID " + Buffer( pid ) );

	// Let's go for the real thing! The run method execution...

	{
		SysLocker locker( t->stateMutx );
		t->currState = Running;
		t->stateCond.broadcast();
	}

	Exception * exc = NULL;

	try {
		t->run();
	}
	catch ( ThreadStopped & ) {
		// no problem, just continue
//		t->msgWrn( "launcher(): got ThreadStopped!" );
	}
	catch ( Exception & e ) {
		exc = new Exception( e );
	}
	catch ( std::exception & e ) {
		exc = new Exception(
			"<std>",
			"<exception>",
			0,
			e.what() );
	}

//	t->msgDbg( "launcher(): return from PID " + Buffer( pid ) );

	{
		SysLocker locker( t->stateMutx );
		t->currState = Waiting;
		t->stateCond.broadcast();
	}

	if ( exc ) {
		t->msgExc( *exc, "Caught thread exception!" );
	}

#if defined( PLATFORM_WIN32 )
	if ( exc ) {
		delete exc;
	}
#else
	return static_cast< void * >( exc );
#endif

}

// ============================================================================

void MT::Thread::abort() {

}

// ============================================================================

// `this' thread-specific key management...
// (to retrieve self() Thread structure)

#if defined( PLATFORM_WIN32 )

DWORD		thisKey;

void MT::Thread::initTLS() {
	thisKey = ::TlsAlloc();
}

void MT::Thread::setTLS(
		Thread		* a ) {
	::TlsSetValue( thisKey, static_cast< LPVOID >( a ) );
}

MT::Thread * MT::Thread::getTLS() {
	Thread * res = static_cast< Thread * >( ::TlsGetValue( thisKey ) );
	if ( ! res ) {
		res = & MainThread::instance();
	}
	return res;
}

#else

pthread_key_t	thisKey;
pthread_once_t	thisKeyOnce = PTHREAD_ONCE_INIT;

static void thisKeyInit() {
	::pthread_key_create( & thisKey, NULL );
}

void MT::Thread::initTLS() {
	::pthread_once( & thisKeyOnce, thisKeyInit );
}

void MT::Thread::setTLS(
		Thread		* a ) {
	::pthread_setspecific( thisKey, static_cast< void * >( a ) );
}

MT::Thread * MT::Thread::getTLS() {
	Thread * res = static_cast< Thread * >( ::pthread_getspecific( thisKey ) );
	if ( ! res ) {
		res = & MainThread::instance();
	}
	return res;
}

#endif

// ============================================================================

