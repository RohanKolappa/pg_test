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
//	VMP.L2.EnmuxerEngine.cpp
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
#include "VMP.L2.EnmuxerEngine.h"

#include "VMP.UI.UObjectDescr.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, EnmuxerEngine )

// ============================================================================

const DL::TypeCPtr & L2::EnmuxerEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "0755930a-385e-4f97-8dbe-0d2456eadfdd" ),
		"VMP.L2.EnmuxerEngine",
		L2::ProxyPushEngine::getClassType(),
		new UI::UObjectDescr(
			"SimpleEnmuxer",
			"Utils,Muxed",
			"Simple Enmuxer (1 in)"
		)
	);

	return i;

}

// ============================================================================

L2::EnmuxerEngine::EnmuxerEngine() :

	ProxyPushEngine	( getClassType() ) {

	privInit( new EnmuxerCore );

}

L2::EnmuxerEngine::EnmuxerEngine(
		EnmuxerEnginePtr
				pMuxrEngn ) :

	ProxyPushEngine	( getClassType() ) {

	privInit( pMuxrEngn ? pMuxrEngn->muxrCore : new EnmuxerCore );

}

L2::EnmuxerEngine::EnmuxerEngine(
		EnmuxerCorePtr	pMuxrCore ) :

	ProxyPushEngine	( getClassType() ) {

	privInit( pMuxrCore );

}

L2::EnmuxerEngine::~EnmuxerEngine() {

//	privExit();

}

// ============================================================================

void L2::EnmuxerEngine::attachPeerEngine(
	const	Uint32		pPeerIndx,
		PushEnginePtr	pPeerEngn ) {

//	msgDbg( "attach(): ---> peer " + Buffer( pPeerIndx ) );

	if ( pPeerIndx ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( peerEngn ) {
		throw InternalError( "Already attached!" );
	}

	muxrCore->attachPeerEngine( muxrSlot, pPeerEngn );

	peerEngn = pPeerEngn;

	ProxyPushEngine::attachPeerEngine( pPeerIndx, pPeerEngn );

//	msgDbg( "attach(): <---" );

}

void L2::EnmuxerEngine::detachPeerEngine(
	const	Uint32		pPeerIndx ) {

//	msgDbg( "detach(): ---> peer " + Buffer( pPeerIndx ) );

	if ( pPeerIndx ) {
		throw InternalError( "Invalid slot!" );
	}

	if ( ! peerEngn ) {
		throw InternalError( "Not attached!" );
	}

	ProxyPushEngine::detachPeerEngine( pPeerIndx );

	peerEngn.kill();

	muxrCore->detachPeerEngine( muxrSlot );

//	msgDbg( "detach(): <---" );

}

// ============================================================================

void L2::EnmuxerEngine::putSessionCallback(
		SessionPtr		pSession ) {

//	msgDbg( "putSessionCallback(): --->" );

//	msgDbg( "putSessionCallback()" );

	if ( isInSession() ) {
		throw InternalError( "putSessionCallback(): already in session!" );
	}

	muxrCore->putSession( muxrSlot, pSession );

	setInSession( true );

//	msgDbg( "putSessionCallback(): <---" );

}

void L2::EnmuxerEngine::endSessionCallback() {

//	msgDbg( "endSessionCallback(): --->" );

	if ( ! isInSession() ) {
		throw InternalError( "endSessionCallback(): not in session!" );
	}

	muxrCore->endSession( muxrSlot );

	setInSession( false );

//	msgDbg( "endSessionCallback(): <---" );

}

void L2::EnmuxerEngine::putFrameCallback(
		FramePtr		pFrame ) {

//	msgDbg( "putFrameCallback(): --->" );

//	msgDbg( "putFrameCallback(): " + pFrame->toBuffer() );

	if ( ! isInSession() ) {
		throw InternalError( "putFrameCallback(): not in session!" );
	}

	try {
		muxrCore->putFrame( muxrSlot, pFrame );
	}
	catch ( BrokenSession & ) {
		setInSession( false );
		throw BrokenSession();
	}

//	msgDbg( "putFrameCallback(): <---" );

}

// ============================================================================

void L2::EnmuxerEngine::privInit(
		EnmuxerCorePtr	pMuxrCore ) {

//	msgDbg( "init(): --->" );

	setNbrInputs( 1 );
	setInputType( 0, Session::getClassType() );

	setNbrOutputs( 1 );
	setOutputType( 0, MuxedSession::getClassType() );

	muxrCore = pMuxrCore;
	muxrSlot = muxrCore->addSlot();

//	msgDbg( "init(): <---" );

}

void L2::EnmuxerEngine::privExit() {

//	msgDbg( "exit(): --->" );

	if ( isInSession() ) {
		try {
			endSessionCallback();
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't end session!" );
		}
	}

	if ( peerEngn ) {
		peerEngn.kill();
		try {
			muxrCore->detachPeerEngine( muxrSlot );
		}
		catch ( Exception & e ) {
			msgExc( e, "Can't detach peer engine!" );
		}
	}

	muxrCore->delSlot( muxrSlot );

//	msgDbg( "exit(): <---" );

}

// ============================================================================

static L2::EngineRegisterer< L2::EnmuxerEngine > enmuxerEngineRegisterer;

// ============================================================================

