// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyPushEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProxyPushEngine )

// ============================================================================

const DL::TypeCPtr & L2::ProxyPushEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "41466b67-a4d7-4bd5-a2fc-a4f015c92012" ),
		"VMP.L2.ProxyPushEngine",
		L2::PushEngine::getClassType() );

	return i;

}

// ============================================================================

void L2::ProxyPushEngine::setNbrOutputs(
	const	Uint32		pSize ) {

	PushEngine::setNbrOutputs( pSize );
	PushEngineProxy::setNbrOutputs( pSize );

}

// ============================================================================

void L2::ProxyPushEngine::flushSessionCallback() {

	for ( Uint32 i = 0 ; i < getNbrOutputs() ; i++ ) {
//		msgDbg( "flushSessionCallback(): flushing output "
//			+ Buffer( i ) + "..." );
		getPeerEngine( i )->flushSessionCallback();
	}

}

// ============================================================================

