// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PictureEncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PictureEncoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PictureEncoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::PictureEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "47ea7504-0ffa-470b-a5c6-dff3febf0a04" ),
		"VMP.L2.PictureEncoderEngine",
		L2::ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::PictureEncoderEngine::PictureEncoderEngine(
		L3::PictureEncoderBackendPtr	__backend ) :

	ProxyPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, VideoSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	backend = __backend;

	setBackend( backend );

}

// ============================================================================

void L2::PictureEncoderEngine::putSessionCallback(
		SessionPtr		iSession ) {

	VideoSessionPtr ivSession = iSession;
	VideoStreamPtr ivStream = ivSession->getContent();

	ovStream = new VideoStream;

	if ( ivStream ) {
		isFlipped = ivStream->isFlipped();
		*ovStream = *ivStream;
	}
	else {
		isFlipped = false;
	}

	ovStream->setFrameRate( 0 );
	ovStream->setFrameScale( 1 );
	ovStream->setCompression( backend->getVideoCompression() );
//	ovStream->setFormat( Image::PictureFormat::Unknown );

	setInSession( true );

}

void L2::PictureEncoderEngine::endSessionCallback() {

	ovStream.kill();

	setInSession( false );

}

void L2::PictureEncoderEngine::putFrameCallback(
		FramePtr		data ) {

	VideoFramePtr vfp = data;
	Image::Picture pic = vfp->getPic();

	if ( isFlipped ) {
		pic.flipTB();
	}

	Buffer	res;

	try {
		res = backend->encode( pic );
	}
	catch ( L3::PictureEncoderBackend::NotSuitable & e ) {
		setInSession( false );
		throw BrokenSession( e );
	}

	// Construct output session, put frame, and close session!

	ovStream->setWidth( pic.getWidth() );
	ovStream->setWidth( pic.getHeight() );
	ovStream->setFormat( pic.getFormat() );

	BytesSessionPtr obsession = new BytesSession( Session::Seekable );

	obsession->setLength( res.length() );
	obsession->setContent( ovStream );

	PushEnginePtr peerEngine = getPeerEngine( 0 );

	try {
		peerEngine->putSessionCallback( obsession );
		peerEngine->putFrameCallback( new BytesFrame( res ) );
		peerEngine->endSessionCallback();
	}
	catch ( Exception & e ) {
		setInSession( false );
		throw BrokenSession( e );
	}

}

// ============================================================================

