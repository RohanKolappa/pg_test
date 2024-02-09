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
//	VMP.L2.ConsumerEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ConsumerEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ConsumerEngine )

// ============================================================================

const DL::TypeCPtr & L2::ConsumerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "c358dca3-a049-44d3-98dd-0737eac3d97d" ),
		"VMP.L2.ConsumerEngine",
		L2::FinalPushEngine::getClassType() );

	return i;

}

// ============================================================================

L2::ConsumerEngine::ConsumerEngine(
		L3::ConsumerBackendPtr	pBackend ) :

	FinalPushEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, pBackend->getInputType() );

	setBackend( pBackend );

	backend = pBackend;

}

// ============================================================================

void L2::ConsumerEngine::putSessionCallback(
		SessionPtr		pSession ) {

	try {
		backend->initSession( pSession );
	}
	catch ( L3::ConsumerBackend::NotSuitable & e ) {
		throw NotSuitable( e );
	}
	catch ( BadCast & ) {
		throw NotSuitable();
	}

	setInSession( true );

}

void L2::ConsumerEngine::endSessionCallback() {

	backend->exitSession();

	setInSession( false );

}

void L2::ConsumerEngine::putFrameCallback(
		FramePtr		pFrame ) {

	try {
		backend->putFrame( pFrame );
	}
	catch ( BadCast & ) {
		backend->exitSession();
		setInSession( false );
		throw BrokenSession();
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

