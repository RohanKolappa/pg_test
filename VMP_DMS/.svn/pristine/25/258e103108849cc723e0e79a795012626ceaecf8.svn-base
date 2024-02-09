// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.FrameFactoryRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.FrameFactory.h"
#include "VMP.FrameFactoryRegistry.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(FrameFactoryRegistry)

// ============================================================================

FrameFactoryRegistry::FrameFactoryRegistry() {

	setObjectName( "VMP.FrameFactoryRegistry" );
	setDebug( true );

//	msgDbg( "FrameFactoryRegistry()" );

}

FrameFactoryRegistry::~FrameFactoryRegistry() {

//	msgDbg( "~FrameFactoryRegistry()" );

}

// ============================================================================

FrameFactoryRegistryPtr FrameFactoryRegistry::instance() {

	static FrameFactoryRegistryPtr i = new FrameFactoryRegistry;

	return i;

}

// ============================================================================

FramePtr FrameFactoryRegistry::getNewFrame(
	const	UUId&		id ) {

	MT::Locker locker( listMutex );

	for ( Uint32 i = 0 ; i < factories.size() ; i++ ) {
		if ( factories[i]->getUUId() == id ) {
			return factories[i]->getNewFrame();
		}
	}

	throw NoSuchId();

}

// ============================================================================

void FrameFactoryRegistry::addFactory(
		FrameFactory		*p ) {

	MT::Locker locker( listMutex );

//	msgDbg( "addFactory(): uuid == "
//		+ p->getUUId().toBuffer() );

	try {
		factories += p;
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't add factory!" );
	}

}

void FrameFactoryRegistry::delFactory(
		FrameFactory		*p ) {

	MT::Locker locker( listMutex );

//	msgDbg( "delFactory(): uuid == "
//		+ p->getUUId().toBuffer() );

	try {
		factories -= p;
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't del factory!" );
	}

}

// ============================================================================

