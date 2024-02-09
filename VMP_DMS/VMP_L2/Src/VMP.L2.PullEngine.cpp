// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PullEngine.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L2.PullEngine.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L2, PullEngine )

// ============================================================================

const DL::TypeCPtr & L2::PullEngine::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "ebc5e58c-441f-4f73-96de-1244cf1e79de" ),
		"VMP.L2.PullEngine",
		L2::Engine::getClassType() );

	return i;

}

// ============================================================================

