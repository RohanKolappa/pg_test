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
//	VMP.L2.PicturesConsumerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PicturesConsumerEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PicturesConsumerEngine )

// ============================================================================

const DL::TypeCPtr & L2::PicturesConsumerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "383ce712-1963-45e8-ab3f-6def15f4a421" ),
		"VMP.L2.PicturesConsumerEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::PicturesConsumerEngine::PicturesConsumerEngine(
		L3::PicturesConsumerBackendPtr	pBackend ) :

	ProxyPullEngine	( getClassType() ),
	backend		( pBackend ) {

	nbrInputs	= backend->getNbrInputs();
	outputType	= backend->getOutputType();

	setNbrInputs( nbrInputs );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		setInputType( i, VideoSession::getClassType() );
	}

	setNbrOutputs( 1 );
	setOutputType( 0, outputType );

	setBackend( backend );

}

// ============================================================================

SessionPtr L2::PicturesConsumerEngine::getSessionCallback() {

//	msgDbg( "getSessionCallback()" );

	VideoSessionArray array;

	array.resize( nbrInputs );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		array[ i ] = getPeerEngine( i )->getSessionCallback();
	}

	SessionPtr res;

	try {
		res = backend->consumeSessions( array );
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

void L2::PicturesConsumerEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback()" );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		getPeerEngine( i )->endSessionCallback();
	}

	setInSession( false );

}

FramePtr L2::PicturesConsumerEngine::getFrameCallback(
	const	Uint64		index ) {

//	msgDbg( "getFrameCallback()" );

	Image::PictureArray array;

	array.resize( nbrInputs );

	for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
		FramePtr frame;
		try {
			frame = getPeerEngine( i )->getFrameCallback( index );
		}
		catch ( EndOfStream & ) {
			throw;
		}
		catch ( BrokenSession & ) {
			for ( Uint32 j = 0 ; j < nbrInputs ; j++ ) {
				if ( j != i ) {
					getPeerEngine( j )->endSessionCallback();
				}
			}
			setInSession( false );
			throw;
		}
		array[ i ] = VideoFramePtr( frame )->getPic();
	}

	FramePtr ret;

	try {
		ret = backend->consumePictures( array );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		for ( Uint32 i = 0 ; i < nbrInputs ; i++ ) {
			getPeerEngine( i )->endSessionCallback();
		}
		setInSession( false );
		throw BrokenSession( e );
	}

	return ret;

}

// ============================================================================

