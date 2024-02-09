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
//	VMP.L2.PicturesProducerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PicturesProducerEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PicturesProducerEngine )

// ============================================================================

const DL::TypeCPtr & L2::PicturesProducerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f0b29254-f9c5-4410-9440-5b9ba2c21b7d" ),
		"VMP.L2.PicturesProducerEngine",
		ProxyPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::PicturesProducerEngine::PicturesProducerEngine(
		L3::PicturesProducerBackendPtr	pBackend ) :

	ProxyPushEngine( getClassType() ) {

	nbrOutputs = pBackend->getNbrOutputs();

	setNbrInputs( 1 );
	setInputType( 0, VideoSession::getClassType() );

	setNbrOutputs( nbrOutputs );
	setOutputType( 0, VideoSession::getClassType() );

	backend = pBackend;

	setBackend( backend );

}

// ============================================================================

void L2::PicturesProducerEngine::putSessionCallback(
		SessionPtr		session ) {

	// Get new output session...

	VideoSessionPtr		iVSession = session;
	VideoSessionArray	oVSessions;

	try {
		oVSessions = backend->processSession( iVSession );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		throw NotSuitable( e );
	}

	if ( oVSessions.getSize() != nbrOutputs ) {
		throw InternalError( "Backend returned nbr sessions != expected" );
	}

	// Propagate output session...

	for ( Uint32 i = 0 ; i < nbrOutputs ; i++ ) {
		try {
			getPeerEngine( i )->putSessionCallback( oVSessions[ i ] );
		}
		catch ( NotSuitable & s ) {
			for ( Uint32 j = 0 ; j < i ; j++ ) {
				getPeerEngine( j )->endSessionCallback();
			}
			throw NotSuitable( s );
		}
	}

	setInSession( true );

}

void L2::PicturesProducerEngine::endSessionCallback() {

	// Propagate end session...

	for ( Uint32 i = 0 ; i < nbrOutputs ; i++ ) {
		getPeerEngine( i )->endSessionCallback();
	}

	setInSession( false );

}

void L2::PicturesProducerEngine::putFrameCallback(
		FramePtr		frame ) {

	VideoFramePtr	iVFrame = frame;
	VideoFrameArray	oVFrames;

	try {
		oVFrames = backend->processFrame( iVFrame );
	}
	catch ( L3::Backend::EndOfStream & ) {
		throw EndOfStream();
	}
	catch ( L3::Backend::BrokenPipe & ) {
		for ( Uint32 j = 0 ; j < nbrOutputs ; j++ ) {
			getPeerEngine( j )->endSessionCallback();
		}
		setInSession( false );
		throw BrokenSession();
	}

	if ( oVFrames.getSize() != nbrOutputs ) {
		throw InternalError( "Backend returned nbr frames != expected" );
	}

	// Send frames...

	for ( Uint32 i = 0 ; i < nbrOutputs ; i++ ) {
		try {
			getPeerEngine( i )->putFrameCallback( oVFrames[ i ] );
		}
		catch ( BrokenSession & ) {
			for ( Uint32 j = 0 ; j < i ; j++ ) {
				getPeerEngine( j )->endSessionCallback();
			}
			setInSession( false );
			throw BrokenSession();
		}
		catch ( EndOfStream & ) {
			throw EndOfStream();
		}
	}

}

// ============================================================================

