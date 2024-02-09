// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PushEngineProxy.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PushEngine.h"
#include "VMP.L2.PushEngineProxy.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PushEngineProxy )

// ============================================================================

void L2::PushEngineProxy::attachPeerEngine(
	const	Uint32		slot,
		L2::PushEnginePtr
				backend ) {

//	msgDbg( "attachPeerEngine(): attaching slot " + Buffer( slot ) + " to "
//		+ backend->getObjectName() + "..." );

	peerList[ slot ] = backend;

}

void L2::PushEngineProxy::detachPeerEngine(
	const	Uint32		slot ) {

//	msgDbg( "detachPeerEngine(): detaching slot " + Buffer( slot ) + "..." );

	peerList[ slot ].kill();

}

L2::PushEnginePtr L2::PushEngineProxy::getPeerEngine(
	const	Uint32		slot ) const {

//	if ( ! peerList[ slot ] ) {
//		msgWrn( "getPeerEngine(): no peer on slot " + Buffer( slot ) + "..." );
//	}

	return peerList[ slot ];

}

// ============================================================================

void L2::PushEngineProxy::setNbrOutputs(
	const	Uint32		pSize ) {

	peerList.resize( pSize );

}

// ============================================================================

