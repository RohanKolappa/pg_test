// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PictureDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PictureDecoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PictureDecoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::PictureDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "2b27a3e3-d6a3-4ceb-8dae-974f1187b7a2" ),
		"VMP.L2.PictureDecoderEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::PictureDecoderEngine::PictureDecoderEngine(
		L3::PictureDecoderBackendPtr	pBackend ) :

	ProxyPullEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, VideoSession::getClassType() );

	backend = pBackend;

	setBackend( backend );

}

// ============================================================================

SessionPtr L2::PictureDecoderEngine::getSessionCallback() {

	constructPicture();

	VideoSessionPtr	session = new VideoSession( Session::Seekable );
	VideoStreamPtr	stream  = new VideoStream;

	session->setLength( BFC::Uint64( 1 ) );
	session->setContent( stream );

	stream->setWidth ( picture.getWidth () );
	stream->setHeight( picture.getHeight() );
	stream->setFormat( picture.getFormat() );

	setInSession( true );

	return session;

}

void L2::PictureDecoderEngine::endSessionCallback() {

	setInSession( false );

}

FramePtr L2::PictureDecoderEngine::getFrameCallback(
	const	BFC::Uint64		index ) {

	if ( index ) {
		throw EndOfStream();
	}

	VideoFramePtr res = new VideoFrame( picture );

	res->setOffset( 0 );

	return res;

}

// ============================================================================

void L2::PictureDecoderEngine::constructPicture() {

	BytesSessionPtr session = getPeerEngine( 0 )->getSessionCallback();

	picture = backend->decode( dumpBytesSession( (Uint32)session->getLength() ) );

	try {
		getPeerEngine( 0 )->endSessionCallback();
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't end session!" );
	}

}

// ============================================================================

Buffer L2::PictureDecoderEngine::dumpBytesSession(
	const	Uint32		totalLength ) {

	// Let's prepare our output buffer...

	Buffer input;

	if ( totalLength ) {
		input.supposedMaxSize( totalLength );
	}

	// Dump input stream until we reach the EndOfStream!

	PullEnginePtr backend = getPeerEngine( 0 );

	for ( ;; ) {

		BytesFramePtr cell;

		try {
			cell = backend->getFrameCallback( input.length() );
		}
		catch ( BrokenSession& ) {
			// Proactive peer ;-)
//			msgWrn( "dumpBytesSession(): got broken session!" );
			break;
		}
		catch ( EndOfStream& ) {
//			msgDbg( "dumpBytesSession(): EOS @ " + Buffer( input.length() ) );
			break;
		}

		Buffer data = cell->getData();

		if ( cell->getOffset() != Frame::NoOffset
		  && cell->getOffset() != input.length() ) {
			msgWrn( "dumpBytesSession(): seeked input!" );
			return input;
		}

		input += data;

	}

	return input;

}

// ============================================================================

