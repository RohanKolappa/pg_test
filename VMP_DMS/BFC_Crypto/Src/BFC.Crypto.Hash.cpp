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
//	BFC.Crypto.Hash.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.DL.Type.h"

#include "BFC.Crypto.Hash.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Hash )

// ============================================================================

const DL::TypeCPtr & Crypto::Hash::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "07c6b925-c0ce-440f-95be-32829bf4dfa7" ),
		"BFC.Crypto.Hash",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

void Crypto::Hash::hashMemory(
	const	Buffer &	pIBuffer,
		Buffer &	pOBuffer ) {

	init();
	process( pIBuffer );
	done( pOBuffer );

}

Buffer Crypto::Hash::hashMemory(
	const	Buffer &	pIBuffer ) {

	Buffer pOBuffer;

	init();
	process( pIBuffer );
	done( pOBuffer );

	return pOBuffer;

}

// ============================================================================

