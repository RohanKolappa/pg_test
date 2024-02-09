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
//	VMP.L2.StreamDecoderEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.AutoCloser.h"

#include "VMP.L2.StreamDecoderEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, StreamDecoderEngine )

// ============================================================================

const DL::TypeCPtr & L2::StreamDecoderEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "ac76150c-fb03-4ed6-bf14-f800d389cff9" ),
		"VMP.L2.StreamDecoderEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::StreamDecoderEngine::StreamDecoderEngine(
		L3::StreamDecoderBackendPtr	pBackend ) :

	ProxyPullEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, BytesSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, pBackend->getOutputType() );

	backend = pBackend;

	setBackend( pBackend );

}

// ============================================================================

void L2::StreamDecoderEngine::attachPeerEngine(
	const	Uint32		slot,
		PullEnginePtr	srcEngine ) {

	if ( slot ) {
		throw InternalError( "Invalid slot!" );
	}

	ioDevice = new IOPullInputDevice( srcEngine );
	ioStream = new IO::ByteInputStream( ioDevice );

	backend->attachIODevice( ioStream );

	ProxyPullEngine::attachPeerEngine( slot, srcEngine );

}

void L2::StreamDecoderEngine::detachPeerEngine(
	const	Uint32		slot ) {

	ProxyPullEngine::detachPeerEngine( slot );

	if ( slot ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( isInSession() ) {
		try {
			endSessionCallback();
		}
		catch ( Exception& e ) {
			msgExc( e, "detachPeerEngine(): failed to auto-end session!" );
		}
	}

	backend->detachIODevice();

	ioStream.kill();
	ioDevice.kill();

}

// ============================================================================

SessionPtr L2::StreamDecoderEngine::getSessionCallback() {

//	msgDbg( "getSessionCallback()" );

	// Let's get a new BytesSession...

	try {
		ioStream->open();
	}
	catch ( ThreadStopped & ) {
		throw;
	}
	catch ( Exception & e ) {
		throw NotSuitable( e );
	}

	IO::AutoCloser	ioStreamCloser( ioStream );

	Bool	randomDevice = ioDevice->hasRandomAccess();

//	msgDbg( "getSessionCallback(): open peer: device seekable == "
//		+ Buffer( randomDevice ) );

	BytesSessionPtr	iBSession = ioDevice->getSession();

	if ( randomDevice != ioStream->hasRandomAccess() ) {
		throw InternalError( "Seekable ? Device != ByteStream" );
	}

	if ( randomDevice != ( iBSession->getFlags() >= Session::Seekable ) ) {
		throw InternalError( "Seekable ? Device != Session" );
	}

	// Make sure we start at the beginning of the input stream...

	begOffset = ioStream->tell();

//	msgDbg( "getSessionCallback(): begOffset: " + Buffer( begOffset ) );

	if ( begOffset != 0 ) {
		msgWrn( "Init: stream begins at " + Buffer( begOffset ) );
	}

	nxtOffset = 0;

//	msgDbg( "getSessionCallback(): asking backend..." );

	SessionPtr	oSession;

	try {
		oSession = backend->initSession( iBSession );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		throw NotSuitable( e );
	}

	// Now, determine how we will handle random access.

//	msgDbg( "getSessionCallback(): checking random access..." );

	frmStore.kill();
	untIndex.kill();
	keyIndex.kill();

	if ( oSession->getFlags() >= Session::Seekable ) {
//		msgWrn( "getSessionCB(): backend handling random access itself!" );
		randMode = BackendRandom;
		frmStore = new FrameStore( 50 );
	}
	else if ( ! randomDevice ) {
//		msgWrn( "getSessionCB(): no random access (device not seekable)!" );
		randMode = NoRandom;
	}
	else if ( ! iBSession->hasUnitsIndex() ) {
//		msgWrn( "getSessionCB(): no random access (no units index)" );
		randMode = NoRandom;
	}
	else {
//		msgDbg( "getSessionCB(): handling random access myself!" );
		untIndex = iBSession->getUnitsIndex();
		Uint32 m = 25;
		if ( untIndex->hasKeyIndex() ) {
			keyIndex = untIndex->getKeyIndex();
			for ( Uint32 i = 1 ; i < keyIndex->getSize() ; i++ ) {
				Uint32 d = keyIndex->getIndex( i )
				         - keyIndex->getIndex( i - 1 );
				if ( d > m ) {
					m = d;
				}
			}
//			msgDbg( "Estimated GOP size: " + Buffer( m ) );
			m *= 2;
			if ( m > 250 ) {
				msgWrn( "getSessionCB(): spurious GOP size!" );
				m = 250;
			}
		}
//		msgDbg( "Using frame store of size " + Buffer( m ) );
		frmStore = new FrameStore( m );
		randMode = SelfRandom;
	}

	setInSession( true );

	ioStreamCloser.forget();

//	msgDbg( "getSessionCallback(): all OK :-)" );

	return oSession;

}

void L2::StreamDecoderEngine::endSessionCallback() {

	frmStore.kill();
	untIndex.kill();
	keyIndex.kill();

	backend->exitSession();

	if ( ioStream->isConnected() ) {
		ioStream->close();
	}

	setInSession( false );

}

FramePtr L2::StreamDecoderEngine::getFrameCallback(
	const	Uint64		pReqOffset ) {

//	msgDbg( "getFrameCallback(): sequence: "
//		+ Buffer( nxtOffset )
//		+ ", requested: "
//		+ ( pReqOffset == Frame::NoOffset ? Buffer( "n/a" ) : Buffer( pReqOffset ) ) );

	// Maybe already in our cache ?...

	if ( frmStore && frmStore->hasFrame( pReqOffset ) ) {
//		msgDbg( "getFrameCallback(): cache hit!" );
		FramePtr res = frmStore->getFrame( pReqOffset );
		nxtOffset = pReqOffset + res->getLength();
		return res;
	}

	Uint64	expOffset = pReqOffset;	// What should get out of this object.

	if ( expOffset == Frame::NoOffset ) {
		// Take whatever comes first!
		expOffset = nxtOffset;
	}
	else if ( expOffset == nxtOffset ) {
		// OK. In sequence!
	}
	else if ( randMode == NoRandom ) {
		// No way to satisfy our client! Just issue a warning, but don't
		// crash on this.
//		msgWrn( "getFrameCB(): no random access, but asked to seek!" );
		expOffset = nxtOffset;
	}
	else if ( randMode == BackendRandom ) {
		// Let the backend handle this...
//		msgWrn( "getFrameCallback(): seeking backend (sequence: "
//			+ Buffer( nxtOffset )
//			+ ", requested: "
//			+ Buffer( pReqOffset )
//			+ ")..." );
		try {
			backend->seekFrame( expOffset );
			nxtOffset = expOffset;
		}
		catch ( L3::Backend::NotSuitable & e ) {
			msgExc( e, "getFrameCB(): backend can't seek!" );
			expOffset = nxtOffset;
		}
	}
	else if ( expOffset >= untIndex->getSize() ) {
		throw EndOfStream();
	}
	else if ( ! keyIndex ) {
		// No key index --> jump to the exact requested frame.
		Uint64	expPos = untIndex->getByteOffset( (Uint32)expOffset );
		//msgDbg( "getFrameCB(): direct jump to byte pos "
		//	+ Buffer( expPos ) );
		try {
			ioStream->seek( expPos );
			backend->resetSession();
			nxtOffset = expOffset;
		}
		catch ( Exception & e ) {
			msgExc( e, "Failed to seek to std frame!" );
			expOffset = nxtOffset;
		}
	}
	else if ( expOffset < nxtOffset
	       || keyIndex->getBefore( (Uint32)expOffset ) > nxtOffset ) {
		// We need to seek now...
		Uint32	keyOffset = keyIndex->getBefore( (Uint32)expOffset );
		Uint64	keyPos = untIndex->getByteOffset( keyOffset );
		//msgDbg( "getFrameCB(): jump to key byte pos "
		//	+ Buffer( keyPos ) );
		try {
			ioStream->seek( keyPos );
			backend->resetSession();
			nxtOffset = keyOffset;
		}
		catch ( Exception & e ) {
			msgExc( e, "Failed to seek to key frame!" );
			expOffset = nxtOffset;
		}
	}

	for (;;) {

		FramePtr res;

		try {
			res = backend->getFrame();
		}
		catch ( L3::Backend::EndOfStream & /* e */ ) {
//			msgExc( e, "Reached end of stream!" );
			throw EndOfStream();
		}
		catch ( L3::Backend::BrokenPipe & /* e */ ) {
//			msgExc( e, "Got a broken pipe here!" );
			setInSession( false );
			throw BrokenSession();
		}
	
		Uint64 resOffset = res->getOffset();

		if ( resOffset == Frame::NoOffset ) {
			// The backend has certainly produced what we asked for!
			resOffset = nxtOffset;
			res->setOffset( nxtOffset );
		}
		else if ( resOffset != nxtOffset ) {
//			msgWrn( "getFrameCallback(): expected: "
//				+ Buffer( nxtOffset )
//				+ ", produced: "
//				+ Buffer( resOffset ) );
			nxtOffset = resOffset;
		}
	
		if ( frmStore ) {
			// We store this frame, whatever its index!
//			msgDbg( "getFrameCallback(): storing frame..." );
			frmStore->addFrame( resOffset, res );
		}

		// Break if got expected frame, or if no hope to get it soon!

		nxtOffset += res->getLength();

//		msgDbg( "getFrameCallback(): to return: "
//			+ Buffer( expOffset )
//			+ ", produced: "
//			+ Buffer( resOffset )
//			+ ", length: "
//			+ Buffer( res->getLength() ) );

		if ( resOffset >= expOffset ) {
			//msgDbg( "getFrameCallback(): ---> done!" );
			return res;
		}

		//msgDbg( "getFrameCallback(): ---> looping..." );

	}

}

// ============================================================================

