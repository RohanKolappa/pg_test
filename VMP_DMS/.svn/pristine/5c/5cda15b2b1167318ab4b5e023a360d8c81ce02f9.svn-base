// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.StreamFactoryRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.StreamFactory.h"
#include "VMP.StreamFactoryRegistry.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(StreamFactoryRegistry)

// ============================================================================

StreamFactoryRegistry::StreamFactoryRegistry() {

	setObjectName( "VMP.StreamFactoryRegistry" );
	setDebug( true );

//	msgDbg( "StreamFactoryRegistry()" );

}

StreamFactoryRegistry::~StreamFactoryRegistry() {

//	msgDbg( "~StreamFactoryRegistry()" );

}

// ============================================================================

StreamFactoryRegistryPtr StreamFactoryRegistry::instance() {

	static StreamFactoryRegistryPtr i = new StreamFactoryRegistry;

	return i;

}

// ============================================================================

StreamPtr StreamFactoryRegistry::getNewStream(
	const	UUId&		id ) {

	MT::Locker locker( listMutex );

	for ( Uint32 i = 0 ; i < factories.size() ; i++ ) {
		if ( factories[i]->getUUId() == id ) {
			return factories[i]->getNewStream();
		}
	}

	throw NoSuchId();

}

// ============================================================================

void StreamFactoryRegistry::addFactory(
		StreamFactory		*p ) {

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

void StreamFactoryRegistry::delFactory(
		StreamFactory		*p ) {

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

