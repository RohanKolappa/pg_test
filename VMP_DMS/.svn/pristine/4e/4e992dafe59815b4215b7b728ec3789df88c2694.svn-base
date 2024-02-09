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
//	BFC.MainThread.cpp
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
#  include <errno.h>
#  include <pthread.h>
#  include <signal.h>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <sys/types.h>
#endif
//#include <iostream>

#include "Sys.MT.ThreadData.h"

#include "BFC.Base.System.h"

#include "BFC.MT.MainThread.h"

// ============================================================================

using namespace BFC;

// ============================================================================

MT::MainThread::MainThread() {

//	cerr << "MainThread(): this == "
//		<< ( void * )this
//		<< endl;

	localData = new Sys::MT::ThreadData;

	initTLS();
	setTLS( this );

	currState = Running;

	// For the debugging stuff...

	setObjectName( "BFC.MainThread" );

	DObject::setThreadFinder( new MTThreadFinder );

#if defined( PLATFORM_WIN32 )

	localData->id = ( HANDLE )-1;

	::signal( SIGTERM, centralSignalHandler );

#else

	localData->id = ::pthread_self();

	// For the signals to catch/ignore...

	sigset_t mask;
	sigemptyset( &mask );
	sigaddset( &mask, SIGUSR1 );
	pthread_sigmask( SIG_UNBLOCK, &mask, 0 );

	struct sigaction sa;
	System::bzero(
		( Uchar * )& sa,
		( Uint32 )sizeof( struct sigaction ) );

	sigfillset( &sa.sa_mask );	// let's serialize the damn thing!
	sa.sa_flags = 0;
	sa.sa_handler = centralSignalHandler;

	if ( ::sigaction(SIGHUP , &sa, 0) < 0
//	  || ::sigaction(SIGINT , &sa, 0) < 0
	  || ::sigaction(SIGQUIT, &sa, 0) < 0
	  || ::sigaction(SIGTERM, &sa, 0) < 0
	  || ::sigaction(SIGBUS , &sa, 0) < 0
//	  || ::sigaction(SIGSEGV, &sa, 0) < 0
	  || ::sigaction(SIGUSR1, &sa, 0) < 0 ) {
		msgDbg( "MainThread::MainThread(): "
			"!!! CAN'T INSTALL SIGNAL HANDLER !!!" );
		msgDbg( ::strerror( errno ) );
	}

#endif

}

MT::MainThread::~MainThread() {

	// This destructor is called __before__ the one in Thread. Here, we
	// should ensure that the later will not try to cancel the main thread.

	currState = Stopped;

	delete localData;
	localData = 0;

}

// ============================================================================

MT::MainThread & MT::MainThread::instance() {

	static MainThread t;

	return t;

}

// ============================================================================

void MT::MainThread::start() {

	throw InternalError( "Can't start main thread!" );

}

void MT::MainThread::stop() {

	throw InternalError( "Can't stop main thread!" );

}

Exception * MT::MainThread::join() {

	throw InternalError( "Can't join main thread!" );

}

// ============================================================================

void MT::MainThread::installSignalHandler(
		SignalEnum	sig,
		SignalHandler	handler) {

	if ( sig != Hangup
	  && sig != Quit
	  && sig != Interrupt
	  && sig != Term ) {
		throw InternalError( "Unsupported signal: " + Buffer( sig ) + "!" );
	}

#if defined( PLATFORM_WIN32 )
	if (sig == Hangup ||sig == Quit) {
		msgDbg("Signal(" + Buffer(sig) + ") not supported on WIN32!");
	}
#endif

//	msgDbg("MainThread::isntallSignalHandler(" + Buffer(sig) + ")");

	if (handlers.exists(sig)) {
		handlers.del(sig);
	}

	handlers += HandlersMap::NodeType(sig, handler);

}

void MT::MainThread::centralSignalHandler(
	int sig) {

	instance().realSignalHandler(sig);

}

static Uint32 segvs = 0;

void MT::MainThread::realSignalHandler(
	int sig) {

#if defined(PLATFORM_WIN32)
	msgDbg("Got signal '" + Buffer(sig) + "'");
#endif

	// No need to reinstall this handler (no SA_RESETHAND, ...)

	// 1 - emergency signals are caught in every thread...

	if (sig == SIGSEGV
#if !defined(PLATFORM_WIN32)
	 || sig == SIGBUS
#endif
	) {
		if (!segvs) {
			segvs = 1;
			msgDbg(
				"Signal "
				+ Buffer(sig)
				+ " (PID: "
#if defined( PLATFORM_WIN32 )
				+ (Uint32)::_getpid()
#else
				+ (Uint32)::getpid()
#endif
				+ ")");
		}
		::exit(-sig);
//		::abort();
	}

#if defined(PLATFORM_WIN32)
	signal(sig, centralSignalHandler);
#endif

	// 2 - standard signals are caught in the master thread only!

	if ( self() != &instance() ) {
//		mainThread.msgDbg(
//			"Signal "
//			+ Buffer(sig)
//			+ " (PID: "
//			+ (Uint32)::getpid()
//			+ " / child of `"
//			+ self()->getParent()->getObjectName()
//			+ "')");
		return;		
	}

//	mainThread.msgDbg(
//		"Signal "
//		+ Buffer(sig)
//		+ " (PID: " + (Uint32)::getpid() + ")");

	Uint32 index;

	SignalEnum ssig = (SignalEnum)sig;

	if (handlers.exists(ssig, index)) {
		SignalHandler handler = handlers.value(index);
		if (handler) {
//			mainThread.msgDbg("---> dispatched to user handler!");
			(*handler)(ssig);
		}
		return;
	}

	// Default behavior is to stop on int, hup, term and quit!

	if (
#if !defined(PLATFORM_WIN32)
		sig == SIGHUP ||
		sig == SIGQUIT ||
#endif
//		sig == SIGINT ||
		sig == SIGTERM) {
		self()->stop();
	}

}

// ============================================================================

void MT::MainThread::run() {

}

// ============================================================================
// BFC::MTThreadFinder
// ============================================================================

const Buffer & MT::MTThreadFinder::getThreadName() const {

//	cerr << "MTThreadFinder: thread: "
//		<< ( void * )Thread::self()
//		<< endl;

	return Thread::self()->getObjectName();

}

Bool MT::MTThreadFinder::selfShouldStop() const {

	return Thread::self()->shouldStop();

}

// ============================================================================

MT::MainThread & MT::mainThread = MT::MainThread::instance();

// ============================================================================

