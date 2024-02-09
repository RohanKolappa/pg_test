// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.IndexFactoryRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.IndexFactoryRegistry.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( IndexFactoryRegistry )

// ============================================================================

IndexFactoryRegistry::IndexFactoryRegistry() {



}

IndexFactoryRegistry::~IndexFactoryRegistry() {



}

// ============================================================================

IndexFactoryRegistryPtr IndexFactoryRegistry::instance() {

	static IndexFactoryRegistryPtr i = new IndexFactoryRegistry;

	return i;

}

// ============================================================================

UnitsIndexPtr IndexFactoryRegistry::getNewIndex(
	const	UUId&		id ) const {

	for ( Uint32 i = 0 ; i < factories.size() ; i++ ) {
		if ( factories[ i ]->getUUId() == id ) {
			return factories[ i ]->getNewIndex();
		}
	}

	throw NoSuchId();

}

// ============================================================================

void IndexFactoryRegistry::addFactory(
		IndexFactoryPtr		p ) {

	factories += p;

}

// ============================================================================

