// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.SessionPuller.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.SessionConsumer.h"
#include "VMP.L2.SessionProducer.h"
#include "VMP.L2.SessionPuller.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, SessionPuller )

// ============================================================================

L2::SessionPuller::SessionPuller(
		L2::PullEnginePtr	__producer,
		L2::PushEnginePtr	__consumer,
	const	Uint32			__maxFrames ) {

	setObjectName( "VMP.L2.SessionPuller" );

	producer = __producer;
	consumer = __consumer;
	maxFrames = __maxFrames;

}

L2::SessionPuller::~SessionPuller() {



}

// ============================================================================

void L2::SessionPuller::stopNow() {

	softStop = true;

}

// ============================================================================

void L2::SessionPuller::run() {

//	msgWrn( "Started!" );

	doLoop();

//	msgWrn( "Stopped!" );

}

// ============================================================================

void L2::SessionPuller::doLoop() {

	// Init...

	softStop = false;

	// Open session...

//	msgDbg( "doLoop(): getting new session..." );

	SessionPtr s;

	try {
		s = producer->getSessionCallback();
	}
	catch ( NotSuitable & e ) {
		throw WrongUsage( e );
	}
	catch ( Exception& e ) {
		throw WrongUsage( e, "Can't get new session!" );
	}

//	msgDbg( "doLoop(): putting new session..." );

	try {
		consumer->putSessionCallback( s );
	}
	catch ( NotSuitable& e ) {
		throw WrongUsage( e );
	}
	catch ( Exception& e ) {
		throw WrongUsage( e, "Can't put new session!" );
	}

	// Exchange frames...

	Bool	needToCloseConsumer = true;
	Bool	needToCloseProducer = true;

	for ( Uint32 c = 0
	    ; ! softStop && ( ! maxFrames || c < maxFrames )
	    ; c++ ) {

		FramePtr f;

		try {
			f = producer->getFrameCallback(
				Frame::NoOffset /* c */ );
		}
		catch ( L2::EndOfStream& ) {
//			msgDbg( "EOS!" );
			break;
		}
		catch ( L2::BrokenSession& ) {
//			msgWrn( "Input broken!" );
			needToCloseProducer = false;
			break;
		}
		catch ( Exception& e ) {
			throw WrongUsage( e, "Can't get new frame!" );
		}

		try {
			consumer->putFrameCallback( f );
		}
		catch ( Exception& e ) {
			throw WrongUsage( e, "Can't put frame!" );
		}

	}

	// Close session...

	if ( needToCloseProducer ) {
		producer->endSessionCallback();
	}

	if ( needToCloseConsumer ) {
		consumer->endSessionCallback();
	}

}

// ============================================================================

