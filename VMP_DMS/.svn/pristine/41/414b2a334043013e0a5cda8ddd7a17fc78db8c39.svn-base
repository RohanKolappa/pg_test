// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.IOPullInputDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.BytesFrame.h"

#include "VMP.L2.IOPullInputDevice.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, IOPullInputDevice )

// ============================================================================

L2::IOPullInputDevice::IOPullInputDevice(
		PullEnginePtr		__engine ) {

	setObjectName( "VMP.L2.IOPullInputDevice" );

	engine = __engine;

}

L2::IOPullInputDevice::~IOPullInputDevice() {

//	if ( isConnected() ) {
//		try {
//			close();
//		}
//		catch ( Exception& e ) {
//			msgExc( e, "Can't auto-close device!" );
//		}
//	}

}

// ============================================================================

void L2::IOPullInputDevice::open() {

//	msgDbg( "open()" );

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	SessionPtr session = engine->getSessionCallback();

	if ( ! session->inherits( BytesSession::getClassType() ) ) {
		throw InvalidSession( "Not a BytesSession, but a "
			+ session->getType()->getName()
//			+ Buffer( session.typeName() )
			+ "!" );
	}

	// Init private fields...

	totalLength = session->getLength();
	nextToAsk   = 0;
	bSession    = session;

	// Update IO::Device fields...

	setAccess( ReadAccess
		| ( session->getFlags() >= Session::Seekable
		  ? RandomAccess : 0 ) );

}

void L2::IOPullInputDevice::close() {

//	msgDbg( "close()" );

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	engine->endSessionCallback();

	// Update fields...

	resetAccess();

	bSession.kill();

}

Buffer L2::IOPullInputDevice::getBytes() {

//	msgDbg( "getBytes(): --->" );
//	msgDbg( "getBytes(): nextToAsk == " + Buffer( nextToAsk ) );

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	FramePtr f;

	try {
		f = engine->getFrameCallback( nextToAsk );
	}
	catch ( BrokenSession& ) {
		resetAccess();
		bSession.kill();
		throw IO::BrokenPipe();
	}
	catch ( EndOfStream& ) {
		throw IO::EndOfStream();
	}

//	msgDbg( "getBytes(): received offset " + Buffer( f->getOffset() ) );

	if ( ! f->inherits( BytesFrame::getClassType() ) ) {
		throw InvalidFrame( "Type: " + Buffer( f.typeName() ) );
	}

	if ( f->getOffset() == Frame::NoOffset ) {
		f->setOffset( nextToAsk );
	}
	else if ( f->getOffset() != nextToAsk ) {
		msgWrn( "Discontinuity!" );
		msgWrn( "... expected: " + Buffer( nextToAsk ) );
		msgWrn( "... received: " + Buffer( f->getOffset() ) );
		engine->endSessionCallback();
		resetAccess();
		bSession.kill();
		throw IO::BrokenPipe();
	}

	BytesFramePtr bf = f;

	Buffer b = bf->getData();

	nextToAsk += b.length();

//	msgDbg( "getBytes(): <---" );

	return b;

}

BFC::Uint64 L2::IOPullInputDevice::seek(
	const	BFC::Uint64		offset ) {

//	msgDbg( "seek(): " + Buffer( tell() ) + " ---> " + Buffer( offset ) );

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( offset != nextToAsk
	  && ! hasRandomAccess() ) {
		throw NoRandomAccess();
	}

	nextToAsk = offset;

	return nextToAsk;

}

BFC::Uint64 L2::IOPullInputDevice::tell() const {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return nextToAsk;

}

BFC::Uint64 L2::IOPullInputDevice::length() const {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return totalLength;

}

// ============================================================================

