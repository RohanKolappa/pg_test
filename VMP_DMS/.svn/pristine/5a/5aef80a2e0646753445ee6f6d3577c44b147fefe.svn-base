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
//	BFC.Crypto.Noekeon.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.Noekeon.h"
#include "BFC.Crypto.CipherRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Noekeon )

// ============================================================================

const DL::TypeCPtr & Crypto::Noekeon::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "8ba3b590-39c6-480b-8c9a-e49b76f97991" ),
		"BFC.Crypto.Noekeon",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"noekeon",
				"Cipher",
				"Noekeon Cipher" ),
			16, 16, 16, 16 ) );

	return i;

}

// ============================================================================

Crypto::Noekeon::Noekeon() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::Noekeon::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 16 ) {
		throw InvalidArgument();
	}

	return 16;

}

// ============================================================================

void Crypto::Noekeon::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	Uint32 keylen = pKey.getLength();

	if ( keylen != 16 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	K[0] = LOAD32H( &key[0] );
	K[1] = LOAD32H( &key[4] );
	K[2] = LOAD32H( &key[8] );
	K[3] = LOAD32H( &key[12] );

	dK[0] = LOAD32H( &key[0] );
	dK[1] = LOAD32H( &key[4] );
	dK[2] = LOAD32H( &key[8] );
	dK[3] = LOAD32H( &key[12] );

	kTHETA(dK[0], dK[1], dK[2], dK[3]);

}

// ============================================================================

void Crypto::Noekeon::encrypt(
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

	a = LOAD32H( &pt[0] ); b = LOAD32H( &pt[4] );
	c = LOAD32H( &pt[8] ); d = LOAD32H( &pt[12] );

	for ( Uint32 r = 0; r < 16; ++r) {
		 a ^= RC[r];
		 THETA(K, a,b,c,d);
		 PI1(a,b,c,d);
		 GAMMA(a,b,c,d);
		 PI2(a,b,c,d);
	}

	a ^= RC[16];
	THETA(K, a, b, c, d);

	STORE32H(a,&ct[0]); STORE32H(b,&ct[4]);
	STORE32H(c,&ct[8]); STORE32H(d,&ct[12]);

}

void Crypto::Noekeon::decrypt(
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

	a = LOAD32H( &ct[0] ); b = LOAD32H( &ct[4] );
	c = LOAD32H( &ct[8] ); d = LOAD32H( &ct[12] );

	for ( Uint32 r = 16; r > 0; --r) {
		 THETA(dK, a,b,c,d);
		 a ^= RC[r];
		 PI1(a,b,c,d);
		 GAMMA(a,b,c,d);
		 PI2(a,b,c,d);
	}

	THETA(dK, a,b,c,d);
	a ^= RC[0];
	STORE32H(a,&pt[0]); STORE32H(b, &pt[4]);
	STORE32H(c,&pt[8]); STORE32H(d, &pt[12]);

}

// ============================================================================

void Crypto::Noekeon::test() {

	static const struct {
		Uint32	keylen;
		Uchar	key[ 16 ];
		Uchar	pt[ 16 ];
		Uchar	ct[ 16 ];
	} tests[] = {
	{
		16,
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		{ 0x18, 0xa6, 0xec, 0xe5, 0x28, 0xaa, 0x79, 0x73,
		  0x28, 0xb2, 0xc0, 0x91, 0xa0, 0x2f, 0x54, 0xc5}
	}
	};

	for ( Uint32 x = 0; x < (sizeof(tests)/sizeof(tests[0])); x++) {

		Buffer	key( tests[ x ].key, tests[ x ].keylen );
		Buffer	pt( tests[ x ].pt, 16 );
		Buffer	ct( tests[ x ].ct, 16 );

		setKey( key );

		Buffer	nct = Cipher::encrypt( pt );
		Buffer	npt = Cipher::decrypt( nct );

		if ( nct != ct
		  || npt != pt ) {
			throw InternalError( "Noekeon failed (pass 1)!" );
		}

		Buffer tmp = ct;

		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::encrypt( tmp );
		}
		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::decrypt( tmp );
		}

		if ( tmp != ct ) {
			throw InternalError( "Noekeon failed (pass 2)!" );
		}

	}

}

// ============================================================================

const Uint32 Crypto::Noekeon::RC[] = {
	0x00000080UL, 0x0000001bUL, 0x00000036UL, 0x0000006cUL,
	0x000000d8UL, 0x000000abUL, 0x0000004dUL, 0x0000009aUL,
	0x0000002fUL, 0x0000005eUL, 0x000000bcUL, 0x00000063UL,
	0x000000c6UL, 0x00000097UL, 0x00000035UL, 0x0000006aUL,
	0x000000d4UL
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::Noekeon > cast5Registerer;

// ============================================================================

