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
//	VMP.L2.ProducerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ProducerEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProducerEngine )

// ============================================================================

const DL::TypeCPtr & L2::ProducerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "fe14e396-fe36-4e81-bcd5-f5022d91bc75" ),
		"VMP.L2.ProducerEngine",
		L2::FinalPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::ProducerEngine::ProducerEngine(
		L3::ProducerBackendPtr	pBackend ) :

	FinalPullEngine( getClassType() ) {

	setNbrOutputs( 1 );
	setOutputType( 0, pBackend->getOutputType() );

	setBackend( pBackend );

	backend = pBackend;

}

// ============================================================================

SessionPtr L2::ProducerEngine::getSessionCallback() {

	SessionPtr res = backend->initSession();

	setInSession( true );

	return res;

}

void L2::ProducerEngine::endSessionCallback() {

	backend->exitSession();

	setInSession( false );

}

FramePtr L2::ProducerEngine::getFrameCallback(
	const	Uint64		pIndex ) {

	try {
		return backend->getFrame( pIndex );
	}
	catch ( L3::Backend::EndOfStream & e ) {
		throw EndOfStream( e );
	}
	catch ( L3::Backend::BrokenPipe & e ) {
		setInSession( false );
		throw BrokenSession( e );
	}

}

// ============================================================================

