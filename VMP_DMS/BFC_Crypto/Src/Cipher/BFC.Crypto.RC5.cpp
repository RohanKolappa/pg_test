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
//	BFC.Crypto.RC5.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Math.Utils.h"

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.RC5.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RC5 )

// ============================================================================

const DL::TypeCPtr & Crypto::RC5::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "869cd7ff-27af-4b18-a1fe-4f2b30b9f2ec" ),
		"BFC.Crypto.RC5",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"rc5",
				"Cipher",
				"RC5 Cipher" ),
			8, 128, 8, 12 ) );

	return i;

}

// ============================================================================

Crypto::RC5::RC5() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::RC5::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 8 ) {
		throw InvalidArgument();
	}

	return ( pKeySize > 128 ? 128 : pKeySize );

}

// ============================================================================

void Crypto::RC5::setKey(
	const	Buffer &	pKey ) {

	Uint32 L[64], A, B, i, j, v, s, t, l;

	rounds = getDescr()->getDefaultRounds();

	const Uchar *	keyPtr = pKey.getCstPtr();
	Uint32		keyLen = pKey.getLength();

	if ( keyLen < getMinKeySize()
	  || keyLen > getMaxKeySize() ) {
		throw InvalidArgument( "Invalid key size!" );
	}

	/* copy the key into the L array */
	for (A = i = j = 0 ; i < keyLen; ) {
		A = (A << 8) | ((Uint32)(keyPtr[i++] & 255));
		if ((i & 3) == 0) {
			L[j++] = BSWAP(A);
			A = 0;
		}
	}

	if ((keyLen & 3) != 0) {
		A <<= ((8 * (4 - (keyLen&3))));
		L[j++] = BSWAP(A);
	}

	/* setup the S array */
	t = (2 * (rounds + 1));
	System::memcpy(
		( Uchar * )K,
		( const Uchar * )stab,
		t * sizeof( Uint32 ) );

	/* mix buffer */
	s = 3 * Math::getMax(t, j);
	l = j;
	for (A = B = i = j = v = 0; v < s; v++) {
		A = K[i] = ROL(K[i] + A + B, 3);
		B = L[j] = ROL(L[j] + A + B, (A+B));
		if (++i == t) { i = 0; }
		if (++j == l) { j = 0; }
	}

}

// ============================================================================

void Crypto::RC5::encrypt(
	const	Buffer &	pPlainText,
		Buffer &	pCipherText ) {

	if ( pPlainText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pCipherText.getLength() != 8 ) {
		pCipherText.resize( 8 );
	}

	const Uchar *	pt = pPlainText.getCstPtr();
	Uchar *		ct = pCipherText.getVarPtr();

	Uint32		A, B;

	A = LOAD32L( pt     );
	B = LOAD32L( pt + 4 );

	Uint32 *	T = K;

	A += T[ 0 ];
	B += T[ 1 ];

	T += 2;

	for ( Uint32 r = 0 ; r < rounds ; r++ ) {
		A = ROL(A ^ B, B) + T[0];
		B = ROL(B ^ A, A) + T[1];
		T += 2;
	}

	STORE32L( A, ct     );
	STORE32L( B, ct + 4 );

}

void Crypto::RC5::decrypt(
	const	Buffer &	pCipherText,
		Buffer &	pPlainText ) {

	if ( pCipherText.getLength() != 8 ) {
		throw InvalidArgument();
	}

	if ( pPlainText.getLength() != 8 ) {
		pPlainText.resize( 8 );
	}

	const Uchar *	ct = pCipherText.getCstPtr();
	Uchar *		pt = pPlainText.getVarPtr();

	Uint32		A, B;

	A = LOAD32L( ct     );
	B = LOAD32L( ct + 4 );

	Uint32 *	T = K + ( rounds << 1 );

	for ( Uint32 r = rounds ; r ; r-- ) {
		B = ROR( B - T[ 1 ], A ) ^ A;
		A = ROR( A - T[ 0 ], B ) ^ B;
		T -= 2;
	}

	A -= T[ 0 ];
	B -= T[ 1 ];

	STORE32L( A, pt     );
	STORE32L( B, pt + 4 );

}

// ============================================================================

void Crypto::RC5::test() {

	static const struct {
		Uchar key[16], pt[8], ct[8];
	} tests[] = {
	{
		{ 0x91, 0x5f, 0x46, 0x19, 0xbe, 0x41, 0xb2, 0x51,
			0x63, 0x55, 0xa5, 0x01, 0x10, 0xa9, 0xce, 0x91 },
		{ 0x21, 0xa5, 0xdb, 0xee, 0x15, 0x4b, 0x8f, 0x6d },
		{ 0xf7, 0xc0, 0x13, 0xac, 0x5b, 0x2b, 0x89, 0x52 }
	},
	{
		{ 0x78, 0x33, 0x48, 0xe7, 0x5a, 0xeb, 0x0f, 0x2f,
			0xd7, 0xb1, 0x69, 0xbb, 0x8d, 0xc1, 0x67, 0x87 },
		{ 0xF7, 0xC0, 0x13, 0xAC, 0x5B, 0x2B, 0x89, 0x52 },
		{ 0x2F, 0x42, 0xB3, 0xB7, 0x03, 0x69, 0xFC, 0x92 }
	},
	{
		{ 0xDC, 0x49, 0xdb, 0x13, 0x75, 0xa5, 0x58, 0x4f,
			0x64, 0x85, 0xb4, 0x13, 0xb5, 0xf1, 0x2b, 0xaf },
		{ 0x2F, 0x42, 0xB3, 0xB7, 0x03, 0x69, 0xFC, 0x92 },
		{ 0x65, 0xc1, 0x78, 0xb2, 0x84, 0xd1, 0x97, 0xcc }
	}
	};

	Buffer	tmp[2];

	for ( Uint32 x = 0 ; x < ( Uint32 )(sizeof(tests) / sizeof(tests[0])); x++) {

		Buffer	key( tests[x].key, 16 );
		Buffer	enc( tests[x].ct, 8 );
		Buffer	dec( tests[x].pt, 8 );

		setKey( key /*, 12 */ );
		tmp[ 0 ] = Cipher::encrypt( dec );
		tmp[ 1 ] = Cipher::decrypt( tmp[ 0 ] );

		if ( tmp[ 0 ] != enc
		  || tmp[ 1 ] != dec ) {
			throw InternalError( "Test1" );
		}

		tmp[ 1 ] = tmp[ 0 ] = Buffer::chars( 8, 0x00 );

		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			tmp[ 0 ] = Cipher::encrypt( tmp[ 0 ] );
		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			tmp[ 0 ] = Cipher::decrypt( tmp[ 0 ] );

		if ( tmp[ 0 ] != tmp[ 1 ] ) {
			throw InternalError( "Test2" );
		}

	}

}

// ============================================================================

const Uint32 Crypto::RC5::stab[50] = {
	0xb7e15163UL, 0x5618cb1cUL, 0xf45044d5UL, 0x9287be8eUL, 0x30bf3847UL, 0xcef6b200UL, 0x6d2e2bb9UL, 0x0b65a572UL,
	0xa99d1f2bUL, 0x47d498e4UL, 0xe60c129dUL, 0x84438c56UL, 0x227b060fUL, 0xc0b27fc8UL, 0x5ee9f981UL, 0xfd21733aUL,
	0x9b58ecf3UL, 0x399066acUL, 0xd7c7e065UL, 0x75ff5a1eUL, 0x1436d3d7UL, 0xb26e4d90UL, 0x50a5c749UL, 0xeedd4102UL,
	0x8d14babbUL, 0x2b4c3474UL, 0xc983ae2dUL, 0x67bb27e6UL, 0x05f2a19fUL, 0xa42a1b58UL, 0x42619511UL, 0xe0990ecaUL,
	0x7ed08883UL, 0x1d08023cUL, 0xbb3f7bf5UL, 0x5976f5aeUL, 0xf7ae6f67UL, 0x95e5e920UL, 0x341d62d9UL, 0xd254dc92UL,
	0x708c564bUL, 0x0ec3d004UL, 0xacfb49bdUL, 0x4b32c376UL, 0xe96a3d2fUL, 0x87a1b6e8UL, 0x25d930a1UL, 0xc410aa5aUL,
	0x62482413UL, 0x007f9dccUL
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::RC5 > rc5Registerer;

// ============================================================================

