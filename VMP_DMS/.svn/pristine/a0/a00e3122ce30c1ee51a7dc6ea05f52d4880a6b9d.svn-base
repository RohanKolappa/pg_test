// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.IOPushOutputDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.BytesFrame.h"

#include "VMP.L2.IOPushOutputDevice.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, IOPushOutputDevice )

// ============================================================================

L2::IOPushOutputDevice::IOPushOutputDevice(
		PushEnginePtr		__engine ) {

	setObjectName( "VMP.L2.IOPushOutputDevice" );

	engine = __engine;

}

L2::IOPushOutputDevice::~IOPushOutputDevice() {

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

void L2::IOPushOutputDevice::open() {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! bSession ) {
		bSession = new BytesSession( Session::Pausable );
	}

	engine->putSessionCallback( bSession );

	// Init private fields...

	nextToPut   = 0;
	totalLength = 0;

	// Update IO::Device fields...

	setAccess( WriteAccess
		| ( bSession->getFlags() >= Session::Seekable
		  ? RandomAccess : 0 ) );

}

void L2::IOPushOutputDevice::putSession(
		BytesSessionPtr		session ) {

	bSession = session;

}

void L2::IOPushOutputDevice::resetSession() {

	bSession.kill();

}

void L2::IOPushOutputDevice::close() {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	engine->endSessionCallback();

	// Update fields...

	resetAccess();

}

void L2::IOPushOutputDevice::putBytes(
	const	Buffer&		data ) {

//	msgDbg( "putBytes(): index == " + Buffer( nextToPut ) );

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	BytesFramePtr frame = new BytesFrame;

	frame->setOffset( nextToPut );
	frame->setData( data );

	try {
		engine->putFrameCallback( frame );
	}
	catch ( BrokenSession&) {
		resetAccess();
		throw IO::BrokenPipe();
	}
	catch ( EndOfStream& ) {
		throw IO::EndOfStream();
	}

	nextToPut += data.length();

	if ( nextToPut > totalLength ) {
		totalLength = nextToPut;
	}

}

BFC::Uint64 L2::IOPushOutputDevice::seek(
	const	BFC::Uint64		index ) {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	nextToPut = index;

	return nextToPut;

}

BFC::Uint64 L2::IOPushOutputDevice::tell() const {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	return nextToPut;

}

BFC::Uint64 L2::IOPushOutputDevice::length() const {

	if ( ! engine ) {
		throw NoAttachedEngine();
	}

	return totalLength;

}

// ============================================================================

