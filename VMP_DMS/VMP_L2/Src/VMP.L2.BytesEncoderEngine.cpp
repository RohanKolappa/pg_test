// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.BytesEncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.BytesEncoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, BytesEncoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::BytesEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e782f92a-78ee-4f87-b732-ceadca543db1" ),
		"VMP.L2.BytesEncoderEngine",
		L2::ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::BytesEncoderEngine::BytesEncoderEngine(
		L3::BytesEncoderBackendPtr	pBackend ) :

	ProxyPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	backend = pBackend;

	setBackend( backend );

}

// ============================================================================

void L2::BytesEncoderEngine::putSessionCallback(
		SessionPtr		pSession ) {

	iSession	= pSession;
	oStream		= iSession->getContent();

	setInSession( true );

}

void L2::BytesEncoderEngine::endSessionCallback() {

	iSession.kill();
	oStream.kill();

	setInSession( false );

}

void L2::BytesEncoderEngine::putFrameCallback(
		FramePtr		pFrame ) {

	BytesFramePtr oFrame;

	try {
		oFrame = backend->encodeFrame( pFrame, iSession );
	}
	catch ( L3::EncoderBackend::NotSuitable & e ) {
		iSession.kill();
		oStream.kill();
		setInSession( false );
		throw BrokenSession( e );
	}

	BytesSessionPtr oSession = new BytesSession( Session::Pausable );

	oSession->setLength( oFrame->getData().getLength() );
	oSession->setContent( oStream );

	PushEnginePtr pEngine = getPeerEngine( 0 );

	try {
		pEngine->putSessionCallback( oSession );
		pEngine->putFrameCallback( oFrame );
		pEngine->endSessionCallback();
	}
	catch ( Exception & e ) {
		iSession.kill();
		oStream.kill();
		setInSession( false );
		throw BrokenSession( e );
	}

}

// ============================================================================

