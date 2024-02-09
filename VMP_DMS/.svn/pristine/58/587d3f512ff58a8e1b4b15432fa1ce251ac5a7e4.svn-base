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
//	BFC.Crypto.PRNG.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.UUId.h"

#include "BFC.Crypto.RNG.h"
#include "BFC.Crypto.PRNG.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, PRNG )

// ============================================================================

const DL::TypeCPtr & Crypto::PRNG::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "07c6b925-c0ce-440f-95be-32829bf4dfa7" ),
		"BFC.Crypto.PRNG",
		DL::Object::getClassType() );

	return i;

}

// ============================================================================

void Crypto::PRNG::rng_make_prng(
	const	Uint32		bits ) {

	if ( bits < 64 || bits > 1024 ) {
		throw InvalidArgument( "Bits should be in the range 64..1024!" );
	}

	Uint32	bytes = ( ( bits + 7 ) >> 3 ) << 1;
	RNGPtr	rng = RNG::createNew();
	Buffer	seed = rng->getBytes( bytes );

	start();
	add_entropy( seed );
	ready();

}

// ============================================================================

Buffer Crypto::PRNG::read(
	const	Uint32		pLength ) {

	Buffer res = Buffer::chars( pLength, 0x00 );

	read( res );

	return res;

}

// ============================================================================

void Crypto::PRNG::done() {

}

// ============================================================================

