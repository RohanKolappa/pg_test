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
//	BFC.Crypto.RC6.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.RC6.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RC6 )

// ============================================================================

const DL::TypeCPtr & Crypto::RC6::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "57236b86-52ca-46e8-8d83-b211b68a1f71" ),
		"BFC.Crypto.RC6",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"rc6",
				"Cipher",
				"RC6 Cipher" ),
			8, 128, 16, 20 ) );

	return i;

}

// ============================================================================

Crypto::RC6::RC6() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::RC6::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 8 ) {
		throw InvalidArgument();
	}

	return ( pKeySize > 128 ? 128 : pKeySize );

}

// ============================================================================

void Crypto::RC6::setKey(
	const	Buffer &	pKey ) {

	Uint32 L[64], S[50], A, B, i, j, v, s, l;

	const Uchar *	keyPtr = pKey.getCstPtr();
	Uint32		keyLen = pKey.getLength();

	if ( keyLen < getMinKeySize()
	  || keyLen > getMaxKeySize() ) {
		throw InvalidArgument( "Invalid key size!" );
	}

	/* copy the key into the L array */
	for (A = i = j = 0; i < keyLen; ) {
		A = (A << 8) | ((Uint32)(keyPtr[i++] & 255));
		if (!(i & 3)) {
			L[j++] = BSWAP(A);
			A = 0;
		}
	}

	if (keyLen & 3) {
		A <<= (8 * (4 - (keyLen & 3)));
		L[j++] = BSWAP(A);
	}

	/* setup the S array */
	System::memcpy(
		( Uchar * )S,
		( const Uchar * )stab,
		44 * sizeof( Uint32 ) );

	/* mix buffer */
	s = 3 * Math::getMax( 44U, j );
	l = j;
	for (A = B = i = j = v = 0; v < s; v++) {
		A = S[i] = ROL(S[i] + A + B, 3);
		B = L[j] = ROL(L[j] + A + B, (A+B));
		if (++i == 44) { i = 0; }
		if (++j == l)  { j = 0; }
	}

	/* copy to key */
	for (i = 0; i < 44; i++) {
		K[i] = S[i];
	}

}

// ============================================================================

void Crypto::RC6::encrypt(
	const	Buffer &	pPlainText,
		Buffer &	pCipherText ) {

	if ( pPlainText.getLength() != 16 ) {
		throw InvalidArgument();
	}

	if ( pCipherText.getLength() != 16 ) {
		pCipherText.resize( 16 );
	}

	const Uchar *	pt = pPlainText.getCstPtr();
	Uchar *		ct = pCipherText.getVarPtr();

	Uint32 a,b,c,d;

	a = LOAD32L( pt      );
	b = LOAD32L( pt +  4 );
	c = LOAD32L( pt +  8 );
	d = LOAD32L( pt + 12 );

	const Uint32 *	S = K;

	b += S[ 0 ];
	d += S[ 1 ];

	S += 2;

	for ( Uint32 r = 0 ; r < 20 ; r += 4 ) {
		encRND( a, b, c, d, S );
		encRND( b, c, d, a, S );
		encRND( c, d, a, b, S );
		encRND( d, a, b, c, S );
	}

	a += S[ 0 ];
	c += S[ 1 ];

	STORE32L( a, ct      );
	STORE32L( b, ct +  4 );
	STORE32L( c, ct +  8 );
	STORE32L( d, ct + 12 );

}

void Crypto::RC6::decrypt(
	const	Buffer &	pCipherText,
		Buffer &	pPlainText ) {

	if ( pCipherText.getLength() != 16 ) {
		throw InvalidArgument();
	}

	if ( pPlainText.getLength() != 16 ) {
		pPlainText.resize( 16 );
	}

	const Uchar *	ct = pCipherText.getCstPtr();
	Uchar *		pt = pPlainText.getVarPtr();

	Uint32 a,b,c,d;

	a = LOAD32L( ct      );
	b = LOAD32L( ct +  4 );
	c = LOAD32L( ct +  8 );
	d = LOAD32L( ct + 12 );

	const Uint32 *	S = K + 42;

	a -= S[ 0 ];
	c -= S[ 1 ];

	for ( Uint32 r = 0 ; r < 20 ; r += 4 ) {
		decRND( d, a, b, c, S );
		decRND( c, d, a, b, S );
		decRND( b, c, d, a, S );
		decRND( a, b, c, d, S );
	}

	S -= 2;

	b -= S[ 0 ];
	d -= S[ 1 ];

	STORE32L( a, pt      );
	STORE32L( b, pt +  4 );
	STORE32L( c, pt +  8 );
	STORE32L( d, pt + 12 );

}

// ============================================================================

void Crypto::RC6::test() {

	static const struct {
		Uint32	keyLen;
		Uchar	key[32], pt[16], ct[16];
	} tests[] = {
	{
		16,
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79,
			0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1 },
		{ 0x52, 0x4e, 0x19, 0x2f, 0x47, 0x15, 0xc6, 0x23,
			0x1f, 0x51, 0xf6, 0x36, 0x7e, 0xa4, 0x3f, 0x18 }
	},
	{
		24,
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
			0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79,
			0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1 },
		{ 0x68, 0x83, 0x29, 0xd0, 0x19, 0xe5, 0x05, 0x04,
			0x1e, 0x52, 0xe9, 0x2a, 0xf9, 0x52, 0x91, 0xd4 }
	},
	{
		32,
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
			0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0,
			0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe },
		{ 0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79,
			0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1 },
		{ 0xc8, 0x24, 0x18, 0x16, 0xf0, 0xd7, 0xe4, 0x89,
			0x20, 0xad, 0x16, 0xa1, 0x67, 0x4e, 0x5d, 0x48 }
	}
	};

	for ( Uint32 x = 0 ; x < ( Uint32 )(sizeof(tests) / sizeof(tests[0])) ; x++ ) {

		Buffer tmp[2];

		Buffer	key( tests[x].key, tests[ x ].keyLen );
		Buffer	enc( tests[x].ct, 16 );
		Buffer	dec( tests[x].pt, 16 );

		setKey( key );

		encrypt( dec, tmp[ 0 ] );
		decrypt( tmp[ 0 ], tmp[ 1 ]);

		if ( tmp[ 0 ] != enc
		  || tmp[ 1 ] != dec ) {
			throw InternalError( "Test1" );
		}

		tmp[ 1 ] = tmp[ 0 ] = Buffer::chars( 16, 0x00 );

		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			encrypt( tmp[ 0 ], tmp[ 0 ] );
		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			decrypt( tmp[ 0 ], tmp[ 0 ] );

		if ( tmp[ 0 ] != tmp[ 1 ] ) {
			throw InternalError( "Test2" );
		}

	}

}

// ============================================================================

const Uint32 Crypto::RC6::stab[ 44 ] = {
	0xb7e15163UL, 0x5618cb1cUL, 0xf45044d5UL, 0x9287be8eUL, 0x30bf3847UL, 0xcef6b200UL, 0x6d2e2bb9UL, 0x0b65a572UL,
	0xa99d1f2bUL, 0x47d498e4UL, 0xe60c129dUL, 0x84438c56UL, 0x227b060fUL, 0xc0b27fc8UL, 0x5ee9f981UL, 0xfd21733aUL,
	0x9b58ecf3UL, 0x399066acUL, 0xd7c7e065UL, 0x75ff5a1eUL, 0x1436d3d7UL, 0xb26e4d90UL, 0x50a5c749UL, 0xeedd4102UL,
	0x8d14babbUL, 0x2b4c3474UL, 0xc983ae2dUL, 0x67bb27e6UL, 0x05f2a19fUL, 0xa42a1b58UL, 0x42619511UL, 0xe0990ecaUL,
	0x7ed08883UL, 0x1d08023cUL, 0xbb3f7bf5UL, 0x5976f5aeUL, 0xf7ae6f67UL, 0x95e5e920UL, 0x341d62d9UL, 0xd254dc92UL,
	0x708c564bUL, 0x0ec3d004UL, 0xacfb49bdUL, 0x4b32c376UL
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::RC6 > rc6Registerer;

// ============================================================================

