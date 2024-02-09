// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.BytesDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.BytesSession.h"

#include "VMP.L2.BytesDecoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, BytesDecoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::BytesDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "cd4d1337-fd42-46c0-ba0e-572a4e7710f2" ),
		"VMP.L2.BytesDecoderEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::BytesDecoderEngine::BytesDecoderEngine(
		L3::BytesDecoderBackendPtr	pBackend ) :

	ProxyPullEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	backend = pBackend;

	setBackend( backend );

}

// ============================================================================

SessionPtr L2::BytesDecoderEngine::getSessionCallback() {

	constructPayload();

	BytesSessionPtr	session = new BytesSession( Session::Seekable );

	session->setLength( payload.getLength() );
	session->setContent( iStream );

	setInSession( true );

	return session;

}

void L2::BytesDecoderEngine::endSessionCallback() {

	setInSession( false );

}

FramePtr L2::BytesDecoderEngine::getFrameCallback(
	const	BFC::Uint64		index ) {

	if ( index ) {
		throw EndOfStream();
	}

	BytesFramePtr res = new BytesFrame( payload );

	res->setOffset( 0 );

	return res;

}

// ============================================================================

void L2::BytesDecoderEngine::constructPayload() {

	Buffer encoded = dumpBytesSession();

	try {
		payload = backend->decode( encoded );
	}
	catch ( L3::DecoderBackend::NotSuitable & e ) {
		throw BrokenSession( e );
	}

}

// ============================================================================

Buffer L2::BytesDecoderEngine::dumpBytesSession() {

	PullEnginePtr pEngine = getPeerEngine( 0 );

	BytesSessionPtr iSession = pEngine->getSessionCallback();

	iStream = iSession->getContent();

	// Let's prepare our output buffer...

	Buffer input;

	if ( iSession->getLength() ) {
		input.supposedMaxSize( (Uint32)iSession->getLength() );
	}

	// Dump input stream until we reach the EndOfStream!

	for (;;) {

		BytesFramePtr cell;

		try {
			cell = pEngine->getFrameCallback( input.getLength() );
		}
		catch ( BrokenSession & ) {
			// Proactive peer ;-)
//			msgWrn( "dumpBytesSession(): got broken session!" );
			break;
		}
		catch ( EndOfStream & ) {
//			msgDbg( "dumpBytesSession(): EOS @ " + Buffer( input.length() ) );
			pEngine->endSessionCallback();
			break;
		}

		Buffer data = cell->getData();

		if ( cell->getOffset() != Frame::NoOffset
		  && cell->getOffset() != input.getLength() ) {
			pEngine->endSessionCallback();
			msgWrn( "dumpBytesSession(): seeked input!" );
			input.kill();
			break;
		}

		input += data;

	}

	return input;

}

// ============================================================================

