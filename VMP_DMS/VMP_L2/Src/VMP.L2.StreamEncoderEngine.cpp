// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.StreamEncoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.AutoCloser.h"

#include "VMP.L2.PushAutoCloser.h"
#include "VMP.L2.StreamEncoderEngine.h"

#include "VMP.L3.StreamEncoderAutoCloser.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, StreamEncoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::StreamEncoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "c2d8433c-8bc7-4d17-8be8-2c018d2ea410" ),
		"VMP.L2.StreamEncoderEngine",
		ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::StreamEncoderEngine::StreamEncoderEngine(
		L3::StreamEncoderBackendPtr	pBackend ) :

	ProxyPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, pBackend->getInputType() );

	setNbrOutputs( 1 );
	setOutputType( 0, BytesSession::getClassType() );

	backend = pBackend;
	odevice = new L3::StreamEncoderODevice;

	backend->attachIODevice( odevice );

	setBackend( backend );

}

L2::StreamEncoderEngine::~StreamEncoderEngine() {

	try {
		backend->detachIODevice();
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

void L2::StreamEncoderEngine::putSessionCallback(
		SessionPtr	pSession ) {

	//msgDbg(Buffer(__FUNCTION__));

	if ( ! backend->canEncode( pSession ) ) {
		throw NotSuitable();
	}

	// Initialize backend...

	BytesSessionPtr	bSession;

	try {
		bSession = backend->initSession( pSession );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		throw NotSuitable( e );
	}

	L3::StreamEncoderAutoCloser backendCloser( backend );

	// Initialize output queue...

	odevice->open();

	IO::AutoCloser deviceCloser( odevice );

	// Initialize peer engine...

	try {
		getPeerEngine( 0 )->putSessionCallback( bSession );
	}
	catch ( NotSuitable & e ) {
		throw NotSuitable( e );
	}

	L2::PushAutoCloser peerCloser( getPeerEngine( 0 ) );

	// Encode header...

	try {
		backend->putHeader();
	}
	catch ( L3::StreamEncoderBackend::NotSuitable & e ) {
		throw NotSuitable( e );
	}

	// Don't send the packets! Defer this to the next steps...

	setInSession( true );

	// Deactivate all auto-closers...

	backendCloser.forget();
	deviceCloser.forget();
	peerCloser.forget();

}

void L2::StreamEncoderEngine::endSessionCallback() {

	//msgDbg(Buffer(__FUNCTION__));

	L2::PushAutoCloser		peerCloser( getPeerEngine( 0 ) );
	IO::AutoCloser			deviceCloser( odevice );
	L3::StreamEncoderAutoCloser	backendCloser( backend );

	// Encode trailer...

	try {
		backend->putTrailer();
	}
	catch ( L3::StreamEncoderBackend::NotSuitable & e ) {
		msgExc( e, "Can't put trailer!" );
	}

	// Purge output queue...

	while ( odevice->hasFrame() ) {
		try {
			getPeerEngine( 0 )->putFrameCallback( odevice->getFrame() );
		}
		catch ( EndOfStream & ) {
			msgWrn( "endSessionCallback(): EOS on peer!" );
			break;
		}
		catch ( BrokenSession & ) {
			msgWrn( "endSessionCallback(): BS on peer!" );
			peerCloser.forget();
			break;
		}
	}

	setInSession( false );

}

void L2::StreamEncoderEngine::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrameCallback(): ---> " + Buffer( pFrame->getOffset() ) );

	L2::PushAutoCloser		peerCloser( getPeerEngine( 0 ) );
	IO::AutoCloser			deviceCloser( odevice );
	L3::StreamEncoderAutoCloser	backendCloser( backend );

	try {
		backend->putFrame( pFrame );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		setInSession( false );
		throw BrokenSession( e );
	}
	catch ( L3::Backend::EndOfStream & e ) {
		setInSession( false );
		throw BrokenSession( e );
	}
	catch ( L3::Backend::BrokenPipe & e ) {
		backendCloser.forget();
		setInSession( false );
		throw BrokenSession( e );
	}

	while ( odevice->hasFrame() ) {
//		msgDbg( "putFrameCallback(): forwarding output frame..." );
		try {
			getPeerEngine( 0 )->putFrameCallback( odevice->getFrame() );
		}
		catch ( BrokenSession & ) {
			peerCloser.forget();
			setInSession( false );
			throw;
		}
		catch ( EndOfStream & ) {
			setInSession( false );
			throw BrokenSession();
		}
	}

	peerCloser.forget();
	deviceCloser.forget();
	backendCloser.forget();

//	msgDbg( "putFrameCallback(): <---" );

}

// ============================================================================

