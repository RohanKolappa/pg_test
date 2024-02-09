// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.SessionFactoryRegistry.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.MT.Locker.h"

#include "VMP.SessionFactory.h"
#include "VMP.SessionFactoryRegistry.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL(SessionFactoryRegistry)

// ============================================================================

SessionFactoryRegistry::SessionFactoryRegistry() {

	setObjectName( "VMP.SessionFactoryRegistry" );
	setDebug( true );

//	msgDbg( "SessionFactoryRegistry()" );

}

SessionFactoryRegistry::~SessionFactoryRegistry() {

//	msgDbg( "~SessionFactoryRegistry()" );

}

// ============================================================================

SessionFactoryRegistryPtr SessionFactoryRegistry::instance() {

	static SessionFactoryRegistryPtr i = new SessionFactoryRegistry;

	return i;

}

// ============================================================================

SessionPtr SessionFactoryRegistry::getNewSession(
	const	UUId&		id ) {

	MT::Locker locker( listMutex );

	for ( Uint32 i = 0 ; i < factories.size() ; i++ ) {
		if ( factories[i]->getUUId() == id ) {
			return factories[i]->getNewSession();
		}
	}

	throw NoSuchId( id.toBuffer() );

}

// ============================================================================

void SessionFactoryRegistry::addFactory(
		SessionFactory		*p ) {

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

void SessionFactoryRegistry::delFactory(
		SessionFactory		*p ) {

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

