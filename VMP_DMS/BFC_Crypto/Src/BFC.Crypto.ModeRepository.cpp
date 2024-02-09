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
//	BFC.Crypto.ModeRepository.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.ModeRepository.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, ModeRepository )

// ============================================================================

Crypto::ModeRepository::ModeRepository() :

	Repository( UUId( "df19fb27-22f1-4f52-907a-c95f0890a7cf" ) ) {

	setObjectName( "BFC.Crypto.ModeRepository" );

}

// ============================================================================

Crypto::ModeRepositoryPtr Crypto::ModeRepository::instance() {

	static ModeRepositoryPtr i = new ModeRepository;

	return i;

}

// ============================================================================

DL::FactoryPtr Crypto::ModeRepository::findFactoryFor(
	const	Buffer &	pModeName ) {

	for ( Uint32 i = 0 ; i < getNbrFactories() ; i++ ) {
		if ( getFactory( i )->getDescr()->getName() == pModeName ) {
			return getFactory( i );
		}
	}

	throw InvalidArgument( "No factory for: " + pModeName );

}

// ============================================================================

