// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyProdEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.ProxyProdEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, ProxyProdEngine )

// ============================================================================

const DL::TypeCPtr & L2::ProxyProdEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "71bef3a1-11df-4776-a2b7-3bbc0c01e8d5" ),
		"VMP.L2.ProxyProdEngine",
		L2::ProdEngine::getClassType() );

	return i;

}

// ============================================================================

void L2::ProxyProdEngine::setNbrOutputs(
	const	Uint32		pSize ) {

	ProdEngine::setNbrOutputs( pSize );
	PushEngineProxy::setNbrOutputs( pSize );

}

// ============================================================================

