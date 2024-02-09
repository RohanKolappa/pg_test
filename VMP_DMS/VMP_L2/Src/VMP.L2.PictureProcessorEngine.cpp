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
//	VMP.L2.PictureProcessorEngine.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "VMP.L2.PictureProcessorEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PictureProcessorEngine )

// ============================================================================

const DL::TypeCPtr & L2::PictureProcessorEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7ee03f6d-6eb9-4e58-ac37-397b9de96252" ),
		"VMP.L2.PictureProcessorEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::PictureProcessorEngine::PictureProcessorEngine(
		L3::PictureProcessorBackendPtr	pBackend ) :

	ProxyPullEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, VideoSession::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, VideoSession::getClassType() );

	backend = pBackend;

	setBackend( pBackend );

}

// ============================================================================

SessionPtr L2::PictureProcessorEngine::getSessionCallback() {

	SessionPtr res;

	try {
		res = backend->processSession(
			getPeerEngine( 0 )->getSessionCallback() );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		throw NotSuitable( e );
	}

	setInSession( true );

	return res;

}

void L2::PictureProcessorEngine::endSessionCallback() {

	getPeerEngine( 0 )->endSessionCallback();

	setInSession( false );

}

FramePtr L2::PictureProcessorEngine::getFrameCallback(
	const	BFC::Uint64		index ) {

	VideoFramePtr iFrame = getPeerEngine( 0 )->getFrameCallback( index );
	VideoFramePtr oFrame;

	try {
		oFrame = backend->processFrame( iFrame );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		getPeerEngine( 0 )->endSessionCallback();
		setInSession( false );
		throw BrokenSession( e );
	}

	if ( oFrame->getOffset() == Frame::NoOffset ) {
		oFrame->setOffset( iFrame->getOffset() );
	}

	return oFrame;

}

// ============================================================================

