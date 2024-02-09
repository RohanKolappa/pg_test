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
//	BFC.Crypto.RC4.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.Crypto.RC4.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RC4 )

// ============================================================================

const DL::TypeCPtr & Crypto::RC4::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "df8c50fb-456c-4d6b-8de0-6b07c5bfc192" ),
		Crypto::PRNG::getClassType(),
		"BFC.Crypto.RC4" );

	return i;

}

// ============================================================================

const Crypto::PRNG::DescriptorCPtr & Crypto::RC4::getClassType() {

	static DescriptorCPtr i = new Descriptor(
		DL::Descr(
			"rc4",
			"PRNG",
			Version(JBS_TARGET_VERSION_MAJ,
				JBS_TARGET_VERSION_MED,
				JBS_TARGET_VERSION_MIN ),
			"RC4 PRNG",
			"<no long description available>",
			Crypto::RC4::getClassType() ),
		32 );

	return i;

}

// ============================================================================

Crypto::RC4::RC4() :

	PRNG( getClassType() ) {

}

// ============================================================================

void Crypto::RC4::start() {

	X = 0;
	
}

void Crypto::RC4::add_entropy(
	const	Buffer &	pData ) {

	const Uchar *	inPtr = pData.getCstPtr();
	Uint32		inLen = pData.getLength();

	while ( inLen-- && X < 256 ) {
		B[ X++ ] = *inPtr++;
	}

}

void Crypto::RC4::ready() {

	Uchar	key[ 256 ];

	System::memcpy( key, B, X );

	for ( Uint32 x = 0 ; x < 256 ; x++ ) {
		B[ x ] = x;
	}

	Uchar	y = 0;

	for ( Uint32 x = 0 ; x < 256 ; x++ ) {
		y += B[ x ] + key[ x % X ];
		Uchar tmp = B[ x ]; B[ x ] = B[ y ]; B[ y ] = tmp;
	}

	X = 0;
	Y = 0;

}

// ============================================================================

Uint32 Crypto::RC4::read(
		Buffer &	pBuffer ) {

	Uchar *		outPtr = pBuffer.getVarPtr();
	Uint32		outLen = pBuffer.getLength();

	Uchar	tmp;
	Uchar	x = X;
	Uchar	y = Y;

	for ( Uint32 n = outLen ; n ; n-- ) {
		x++;
		y += B[ x ];
		tmp = B[ x ]; B[ x ] = B[ y ]; B[ y ] = tmp;
		tmp = B[ x ] + B[ y ];
		*outPtr++ ^= B[ tmp ];
	}

	X = x;
	Y = y;

	return outLen;

}

// ============================================================================

Buffer Crypto::RC4::exportState() {

	return PRNG::read( 32 );

}
 
void Crypto::RC4::importState(
	const	Buffer &	pState ) {

	if ( pState.getLength() != 32 ) {
		throw InternalError();
	}
	
	start();

	add_entropy( pState );

}

// ============================================================================

void Crypto::RC4::test() {

	static const struct {
		Uchar	key[8];
		Uchar	pt[8];
		Uchar	ct[8];
	} tests[] = {
	{
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef },
		{ 0x75, 0xb7, 0x87, 0x80, 0x99, 0xe0, 0xc5, 0x96 }
	}
	};

	for ( Uint32 x = 0 ; x < ( Uint32 )(sizeof(tests)/sizeof(tests[0])) ; x++ ) {

		start();
		add_entropy( Buffer( tests[x].key, 8 ) );
		ready();
		Buffer dst( tests[x].pt, 8 );
		read( dst );
		done();
		if ( dst != Buffer( tests[x].ct, 8 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

static Crypto::PRNG::Registerer< Crypto::RC4 > rc4Registerer;

// ============================================================================

