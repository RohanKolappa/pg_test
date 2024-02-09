// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.StreamFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(StreamFactory)

// ============================================================================

StreamFactory::StreamFactory(
	const	DL::TypeCPtr &	type ) {

	uuid = type->getUUId();

	registry = StreamFactoryRegistry::instance();

	registry->addFactory( this );

}

StreamFactory::~StreamFactory() {

	registry->delFactory( this );

}

// ============================================================================

const UUId& StreamFactory::getUUId() const {

	return uuid;

}

// ============================================================================


