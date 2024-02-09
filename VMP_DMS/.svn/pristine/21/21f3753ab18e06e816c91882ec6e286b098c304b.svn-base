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
//	BFC.Dyn.Repository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Repository.h"
#include "BFC.Dyn.Type.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, Repository )

// ============================================================================

Dyn::Repository::Repository() {

}

// ============================================================================

Dyn::RepositoryPtr Dyn::Repository::instance() {

	static RepositoryPtr i = new Repository;

	return i;

}

// ============================================================================

void Dyn::Repository::add(
		TypePtr		pType ) {

	pType->setLocked();

	if ( ! pType->hasUUId() ) {
		throw InternalError( "Can't add Type without uuid!" );
	}

	if ( ! pType->hasName() ) {
		throw InternalError( "Can't add Type without name!" );
	}

	mapUUId.add( pType->getUUId(), pType );
	mapName.add( pType->getName(), pType );

}

void Dyn::Repository::del(
		TypePtr		pType ) {

	mapUUId.del( pType->getUUId() );
	mapName.del( pType->getName() );

}

Dyn::TypePtr Dyn::Repository::get( 
	const	UUId &		pUUId ) const {

	return mapUUId[ pUUId ];

}

Dyn::TypePtr Dyn::Repository::get( 
	const	Buffer &	pName ) const {

	return mapName[ pName ];

}

// ============================================================================

