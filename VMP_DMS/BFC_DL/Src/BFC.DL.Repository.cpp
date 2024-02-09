// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DL.Repository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "BFC.DL.DataBase.h"
#include "BFC.DL.Descr.h"
#include "BFC.DL.Event.h"
#include "BFC.DL.Factory.h"
#include "BFC.DL.Repository.h"
#include "BFC.DL.Type.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DL, Repository )

// ============================================================================

DL::Repository::Repository(
	const	UUId &		pRepositoryUUId ) :

	uuid	( pRepositoryUUId ) {

//	setObjectName( "BFC.DL.Repository" );
//
//	msgDbg( "Repository(): " + uuid.toBuffer() + ": " + getObjectName() );

	DataBase::instance()->addRepository( uuid, RepositoryPtr( this, false ) );

}

DL::Repository::~Repository() {

	DataBase::instance()->delRepository( uuid );

//	msgDbg( "~Repository(): " + uuid.toBuffer() + ": " + getObjectName() );

}

// ============================================================================

void DL::Repository::addFactory(
		FactoryPtr	pFactory,
	const	Buffer &	/*pDllName*/ ) {

	UUId	uuid = pFactory->getType()->getUUId();
	Buffer	name = pFactory->getType()->getName();

//	msgDbg( "addFactory(): " + uuid.toBuffer() + " (" + name + ")" );

	{
		FastCSLocker locker( critSect );

		loadMap.add( uuid, pFactory, true );
		lastLst.add( uuid, pFactory, true );
	}

	warnObservers( new FactoryAddedEvent( pFactory ) );

}

void DL::Repository::delFactory(
		FactoryPtr	pFactory,
	const	Buffer &	/*pDllName*/ ) {

	UUId	uuid = pFactory->getType()->getUUId();
	Buffer	name = pFactory->getType()->getName();

//	msgDbg( "delFactory(): " + uuid.toBuffer() + " (" + name + ")" );

	{
		FastCSLocker locker( critSect );

		if ( ! loadMap.contains( uuid ) ) {
			return;
//			throw InternalError( "No loader!" );
		}

		loadMap.del( uuid );

		if ( lastLst.contains( uuid ) ) {
			lastLst.del( uuid );
		}
	}

	warnObservers( new FactoryDeletedEvent( pFactory ) );

}

// ============================================================================

Bool DL::Repository::hasFactoryFor(
	const	UUId &		id ) const {

	return loadMap.contains( id );

}

DL::FactoryPtr DL::Repository::getFactoryFor(
	const	UUId &		id ) const {

	return loadMap[ id ];

}

// ============================================================================

Bool DL::Repository::hasFactoryFor(
	const	Buffer &	id ) const {

	for ( Uint32 i = 0 ; i < loadMap.getSize() ; i++ ) {
		if ( loadMap.getValue( i )->getType()->getData()->getName() == id ) {
			return true;
		}
	}

	return false;

}

DL::FactoryPtr DL::Repository::getFactoryFor(
	const	Buffer &	id ) const {

	for ( Uint32 i = 0 ; i < loadMap.getSize() ; i++ ) {
		if ( loadMap.getValue( i )->getType()->getData()->getName() == id ) {
			return loadMap.getValue( i );
		}
	}

	throw InvalidArgument( "No such factory: \"" + id + "\"" );

}

// ============================================================================

DL::FactoryArray DL::Repository::getFactories() const {

	FactoryArray res;

	for ( Uint32 i = 0 ; i < loadMap.getSize() ; i++ ) {
		res += loadMap.getValue( i );
	}

	return res;

}

Uint32 DL::Repository::getNbrFactories() const {

	return loadMap.getSize();

}

DL::FactoryPtr DL::Repository::getFactory(
	const	Uint32		pIndex ) {

	return loadMap.getValue( pIndex );

}

// ============================================================================

DL::FactoryArray DL::Repository::getLastFactories() {

	FastCSLocker locker( critSect );

	FactoryArray res;

	for ( Uint32 i = 0 ; i < lastLst.getSize() ; i++ ) {
		res += lastLst.getValue( i );
	}

	lastLst.kill();

	return res;

}

// ============================================================================

DL::ObjectPtr DL::Repository::makeObject(
	const	UUId &		pUUId ) {

	return getFactoryFor( pUUId )->makeObject();

}

// ============================================================================

void DL::Repository::onObserverAdded(
		Msg::ObserverPtr	pObserver,
	const	Msg::Observer::SrvId	pObsSrvId ) {

	FactoryArray a = getFactories();

	for ( Uint32 i = 0 ; i < a.getSize() ; i++ ) {
		warnObserver(
			pObserver,
			pObsSrvId,
			new FactoryAddedEvent( a[ i ] ) );
	}

}

// ============================================================================

