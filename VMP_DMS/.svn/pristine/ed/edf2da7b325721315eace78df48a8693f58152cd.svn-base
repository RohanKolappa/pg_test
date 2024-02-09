// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.MultiProcessorEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.MultiProcessorEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, MultiProcessorEngine )

// ============================================================================

const DL::TypeCPtr & L2::MultiProcessorEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "58c770b1-e034-411b-830b-9c6932d27955" ),
		"VMP.L2.MultiProcessorEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::MultiProcessorEngine::MultiProcessorEngine(
		L3::MultiProcessorBackendPtr	pBackend ) :

	ProxyPullEngine	( getClassType() ),
	backend		( pBackend ) {

	nbrInputs	= backend->getNbrInputs();

	setNbrInputs( nbrInputs );
	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		setInputType( i, pBackend->getSessionType() );
	}

	setNbrOutputs( 1 );
	setOutputType( 0, pBackend->getSessionType() );

	setBackend( pBackend );

}

// ============================================================================

SessionPtr L2::MultiProcessorEngine::getSessionCallback() {

	SessionArray array;

	array.resize( nbrInputs );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		array[ i ] = getPeerEngine( i )->getSessionCallback();
	}

	SessionPtr res;

	try {
		res = backend->initSession( array );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
			getPeerEngine( i )->endSessionCallback();
		}
		throw NotSuitable( e );
	}

	setInSession( true );

	return res;

}

void L2::MultiProcessorEngine::endSessionCallback() {

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		getPeerEngine( i )->endSessionCallback();
	}

	backend->exitSession();

	setInSession( false );

}

FramePtr L2::MultiProcessorEngine::getFrameCallback(
	const	Uint64		index ) {

	FrameArray iFrame;

	iFrame.resize( nbrInputs );

	Uint64 idx = backend->convertOffset( index );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		try {
			iFrame[ i ] = getPeerEngine( i )->getFrameCallback( idx );
		}
		catch ( L2::BrokenSession & e ) {
			for ( Uint32 j = 0 ; j < nbrInputs ; j++ ) {
				if ( j != i ) {
					getPeerEngine( j )->endSessionCallback();
				}
			}
			backend->exitSession();
			setInSession( false );
			throw BrokenSession( e );
		}
		if ( iFrame[ i ]->getOffset() != Frame::NoOffset
		  && iFrame[ i ]->getOffset() != idx ) {
			for ( Uint32 j = 0 ; j < nbrInputs ; j++ ) {
				getPeerEngine( j )->endSessionCallback();
			}
			backend->exitSession();
			setInSession( false );
			throw BrokenSession(
				"Invalid frame offset "
				+ Buffer( iFrame[ i ]->getOffset() )
				+ " (exp: "
				+ Buffer( idx )
				+ ") from peer "
				+ Buffer( i )
				+ "!" );
		}
	}

	FramePtr oFrame;

	try {
		oFrame = backend->processFrame( iFrame );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
			getPeerEngine( i )->endSessionCallback();
		}
		setInSession( false );
		throw BrokenSession( e );
	}

	if ( oFrame->getOffset() == Frame::NoOffset ) {
		oFrame->setOffset( index );
	}

	return oFrame;

}

// ============================================================================

