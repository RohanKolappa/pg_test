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
//	BFC.Crypto.SPRNG.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.SPRNG.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SPRNG )

// ============================================================================

const DL::TypeCPtr & Crypto::SPRNG::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1309397b-0f3f-40f4-8da5-a98ed569fc0c" ),
		Crypto::PRNG::getClassType(),
		"BFC.Crypto.SPRNG" );

	return i;

}

// ============================================================================

const Crypto::PRNG::DescriptorCPtr & Crypto::SPRNG::getClassType() {

	static DescriptorCPtr i = new Descriptor(
		DL::Descr(
			"sprng",
			"PRNG",
			Version(JBS_TARGET_VERSION_MAJ,
				JBS_TARGET_VERSION_MED,
				JBS_TARGET_VERSION_MIN ),
			"SPRNG PRNG",
			"<no long description available>",
			Crypto::SPRNG::getClassType() ),
		0 );

	return i;

}

// ============================================================================

Crypto::SPRNG::SPRNG() :

	PRNG( getClassType() ) {

	rng = RNG::createNew();

}

// ============================================================================

void Crypto::SPRNG::start() {

}

void Crypto::SPRNG::add_entropy(
	const	Buffer &	/* pData */ ) {

}

void Crypto::SPRNG::ready() {

}

void Crypto::SPRNG::done() {

}

// ============================================================================

Uint32 Crypto::SPRNG::read(
		Buffer &	pBuffer ) {

	pBuffer = rng->getBytes( pBuffer.getLength() );

	return pBuffer.getLength();

}

// ============================================================================

Buffer Crypto::SPRNG::exportState() {

	return Buffer();

}
 
void Crypto::SPRNG::importState(
	const	Buffer &	/* pŜtate */ ) {

}

// ============================================================================

void Crypto::SPRNG::test() {

}

// ============================================================================

static Crypto::PRNG::Registerer< Crypto::SPRNG > sprngRegisterer;

// ============================================================================

