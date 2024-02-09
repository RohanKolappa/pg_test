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
//	VMP.L2.ProxyPushFifo.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedSession.h"

#include "VMP.L2.EngineRegisterer.h"
#include "VMP.L2.ProxyPushFifo.h"
#include "VMP.L2.Pumper.h"
#include "VMP.L2.SimpleProxyPushEngine.h"
#include "VMP.L2.Sink.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProxyPushFifo )

// ============================================================================

const DL::TypeCPtr & L2::ProxyPushFifo::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "03d3129d-14e7-4662-b5b4-a04115e6c940" ),
		"VMP.L2.ProxyPushFifo",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"ProxyPushFifo",
			"Utils",
			"ProxyPushFifo"
		)
	);

	return i;

}

// ============================================================================

L2::ProxyPushFifo::ProxyPushFifo() :

	ProxyPushEngine	( getClassType() ) {

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, Session::getClassType() );

}

L2::ProxyPushFifo::~ProxyPushFifo() {

//	if ( outpEngn ) {
//		try {
//			privExit();
//		}
//		catch ( Exception & ) {
//		}
//	}

}

// ============================================================================

void L2::ProxyPushFifo::attachPeerEngine(
	const	Uint32		pPeerIndx,
		PushEnginePtr	pPeerEngn ) {

//	msgDbg( "attach(): ---> peer " + Buffer( pPeerIndx ) );

	if ( pPeerIndx ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( outpEngn ) {
		throw InternalError( "Already attached!" );
	}

	privInit( pPeerEngn );

//	msgDbg( "attach(): <---" );

}

void L2::ProxyPushFifo::detachPeerEngine(
	const	Uint32		pPeerIndx ) {

//	msgDbg( "detach(): ---> peer " + Buffer( pPeerIndx ) );

	if ( pPeerIndx ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! outpEngn ) {
		throw InternalError( "Not attached!" );
	}

	privExit();

//	msgDbg( "detach(): <---" );

}

// ============================================================================

void L2::ProxyPushFifo::putSessionCallback(
		SessionPtr	pSession ) {

//	msgDbg( "putSession(): --->" );

	sinkEngn->getInputEngine()->putSessionCallback( pSession );

	setInSession( true );

//	msgDbg( "putSession(): <---" );

}

void L2::ProxyPushFifo::endSessionCallback() {

//	msgDbg( "endSession(): --->" );

	setInSession( false );

	sinkEngn->getInputEngine()->endSessionCallback();

//	msgDbg( "endSession(): <---" );

}

void L2::ProxyPushFifo::putFrameCallback(
		FramePtr	pFrame ) {

//	msgDbg( "putFrame(): --->" );

	try {
		sinkEngn->getInputEngine()->putFrameCallback( pFrame );
	}
	catch ( L2::BrokenSession & ) {
		setInSession( false );
		throw;
	}

//	msgDbg( "putFrame(): <---" );

}

void L2::ProxyPushFifo::flushSessionCallback() {

	sinkEngn->getInputEngine()->flushSessionCallback();

}

// ============================================================================

void L2::ProxyPushFifo::privInit(
		PushEnginePtr	pPeerEngn ) {

//	msgDbg( "privInit(): --->" );

	if ( sinkEngn ) {
		throw AlreadyInitialized();
	}

	sinkEngn = new Sink( 0 );
	pumpEngn = new Pumper;
	outpEngn = new SimpleProxyPushEngine;

	pumpEngn->addObjectName( "[ ProxyPushFifo ]" );
	pumpEngn->setSourceEngine( sinkEngn->getOutputEngine() );
	pumpEngn->setTargetEngine( outpEngn );

	outpEngn->attachPeerEngine( 0, pPeerEngn );

	pumpEngn->start();

//	msgDbg( "privInit(): <---" );

}

void L2::ProxyPushFifo::privExit() {

//	msgDbg( "privExit(): --->" );

	if ( ! sinkEngn ) {
		throw NotInitialized();
	}

	if ( isInSession() ) {
		try {
			endSessionCallback();
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't end session!" );
		}
	}

	pumpEngn->cleanup();
	pumpEngn->join();

	sinkEngn.kill();
	pumpEngn.kill();
	outpEngn.kill();

//	msgDbg( "privExit(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< L2::ProxyPushFifo > registerer;

// ============================================================================

