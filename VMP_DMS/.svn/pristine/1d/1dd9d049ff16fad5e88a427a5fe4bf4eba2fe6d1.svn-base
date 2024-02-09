// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DL.Type.h"

#include "VMP.IndexFactory.h"
#include "VMP.IndexFactoryRegistry.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(IndexFactory)

// ============================================================================

IndexFactory::IndexFactory(
	const	DL::TypeCPtr &	__type ) {

	type = __type;

	IndexFactoryRegistry::instance()->addFactory( this );

}

IndexFactory::~IndexFactory() {



}

// ============================================================================

const UUId& IndexFactory::getUUId() const {

	return type->getUUId();

}

// ============================================================================


