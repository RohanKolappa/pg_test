// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyPullEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ProxyPullEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProxyPullEngine )

// ============================================================================

void L2::ProxyPullEngine::attachPeerEngine(
	const	BFC::Uint32		slot,
		PullEnginePtr		backend ) {

	backends[ slot ] = backend;

}

void L2::ProxyPullEngine::detachPeerEngine(
	const	BFC::Uint32		slot ) {

	backends[ slot ].kill();

}

L2::PullEnginePtr L2::ProxyPullEngine::getPeerEngine(
	const	BFC::Uint32		slot ) const {

	return backends[ slot ];

}

// ============================================================================

void L2::ProxyPullEngine::setNbrInputs(
	const	BFC::Uint32		size ) {

	backends.resize( size );

	Engine::setNbrInputs( size );

}

// ============================================================================

const DL::TypeCPtr & L2::ProxyPullEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "ad0bc200-61fd-4007-a6f1-4d59bc27db6c" ),
		"VMP.L2.ProxyPullEngine",
		L2::PullEngine::getClassType() );

	return i;

}

// ============================================================================

