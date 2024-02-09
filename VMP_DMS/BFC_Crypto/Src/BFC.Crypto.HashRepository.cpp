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
//	BFC.Crypto.HashRepository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.HashRepository.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, HashRepository )

// ============================================================================

Crypto::HashRepository::HashRepository() :

	Repository( UUId( "9e61a2f1-8708-468b-a7d1-52327b8d784d" ) ) {

	setObjectName( "BFC.Crypto.HashRepository" );

}

// ============================================================================

Crypto::HashRepositoryPtr Crypto::HashRepository::instance() {

	static HashRepositoryPtr i = new HashRepository;

	return i;

}

// ============================================================================

DL::FactoryPtr Crypto::HashRepository::findFactoryFor(
	const	Buffer &	pHashName ) {

	for ( Uint32 i = 0 ; i < getNbrFactories() ; i++ ) {
		if ( getFactory( i )->getDescr()->getName() == pHashName ) {
			return getFactory( i );
		}
	}

	throw InvalidArgument( "No factory for: " + pHashName );

}

// ============================================================================

