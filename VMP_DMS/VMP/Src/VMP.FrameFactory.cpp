// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.FrameFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( FrameFactory )

// ============================================================================

FrameFactory::FrameFactory(
	const	DL::TypeCPtr &	type) {

	uuid = type->getUUId();

	registry = FrameFactoryRegistry::instance();

	registry->addFactory( this );

}

FrameFactory::~FrameFactory() {

	registry->delFactory( this );

}

// ============================================================================

const UUId& FrameFactory::getUUId() const {

	return uuid;

}

// ============================================================================


