// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.SessionFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(SessionFactory)

// ============================================================================

SessionFactory::SessionFactory(
	const	DL::TypeCPtr &	type ) {

	uuid = type->getUUId();

	registry = SessionFactoryRegistry::instance();

	registry->addFactory( this );

}

SessionFactory::~SessionFactory() {

	registry->delFactory( this );

}

// ============================================================================

const UUId& SessionFactory::getUUId() const {

	return uuid;

}

// ============================================================================


