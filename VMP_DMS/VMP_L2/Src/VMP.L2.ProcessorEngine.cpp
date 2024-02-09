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
//	VMP.L2.ProcessorEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ProcessorEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProcessorEngine )

// ============================================================================

const DL::TypeCPtr & L2::ProcessorEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "29535126-8885-4961-9c92-0c336621f40e" ),
		"VMP.L2.ProcessorEngine",
		L2::ProxyPullEngine::getClassType() );

	return i;

}

// ============================================================================

L2::ProcessorEngine::ProcessorEngine(
		L3::ProcessorBackendPtr	pBackend ) :

	ProxyPullEngine( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, pBackend->getSessionType() );

	setNbrOutputs( 1 );
	setOutputType( 0, pBackend->getSessionType() );

	backend = pBackend;

	setBackend( pBackend );

}

// ============================================================================

SessionPtr L2::ProcessorEngine::getSessionCallback() {

	SessionPtr	iSess = getPeerEngine( 0 )->getSessionCallback();
	SessionPtr	oSess;

	try {
		oSess = backend->initSession( iSess );
	}
	catch ( L3::Backend::NotSuitable & e ) {
		getPeerEngine( 0 )->endSessionCallback();
		throw NotSuitable( e );
	}

	setInSession( true );

	return oSess;

}

void L2::ProcessorEngine::endSessionCallback() {

	backend->exitSession();

	getPeerEngine( 0 )->endSessionCallback();

	setInSession( false );

}

FramePtr L2::ProcessorEngine::getFrameCallback(
	const	Uint64		index ) {

	Uint64	idx = backend->convertOffset( index );

	FramePtr iFrame;

	try {
		iFrame = getPeerEngine( 0 )->getFrameCallback( idx );
	}
	catch ( L2::BrokenSession & e ) {
		backend->exitSession();
		setInSession( false );
		throw BrokenSession( e );
	}

	FramePtr oFrame;

	try {
		oFrame = backend->processFrame( iFrame );
	}
//	catch ( L3::Backend::NotSuitable & e ) {
	catch ( Exception & e ) {
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

