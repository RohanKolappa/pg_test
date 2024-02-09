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
//	BFC.DL.DataBase.cpp
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
#include "BFC.DL.Event.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DL, DataBase )

// ============================================================================

DL::DataBasePtr DL::DataBase::instance() {

	static DataBasePtr i = new DataBase;

	return i;

}

// ============================================================================

void DL::DataBase::addRepository(
	const	UUId &		pUUId,
		RepositoryPtr	pRepository ) {

	{
		FastCSLocker locker( allLock );
		allRepo.add( pUUId, pRepository );
	}

	warnObservers( new RepositoryAddedEvent( pRepository ) );

}

void DL::DataBase::delRepository(
	const	UUId &		pUUId ) {

	warnObservers( new RepositoryDeletedEvent( pUUId ) );

	{
		FastCSLocker locker( allLock );
		allRepo.del( pUUId );
	}

}

// ============================================================================

Array< DL::RepositoryPtr > DL::DataBase::getAllRepositories() {

	FastCSLocker locker( allLock );

	Array< RepositoryPtr > res( allRepo.getSize() );

	for ( Uint32 i = 0 ; i < allRepo.getSize() ; i++ ) {
		res[ i ] = allRepo.getValue( i );
	}

	return res;

}

// ============================================================================

void DL::DataBase::onObserverAdded(
		Msg::ObserverPtr	pObserver,
	const	Msg::Observer::SrvId	pObsSrvId ) {

	Array< RepositoryPtr > a = getAllRepositories();

	for ( Uint32 i = 0 ; i < a.getSize() ; i++ ) {
		warnObserver(
			pObserver,
			pObsSrvId,
			new RepositoryAddedEvent( a[ i ] ) );
	}

}

// ============================================================================

