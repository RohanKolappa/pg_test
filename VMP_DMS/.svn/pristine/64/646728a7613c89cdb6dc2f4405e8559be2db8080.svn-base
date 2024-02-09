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
//	BFC.Crypto.SkipJack.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.SkipJack.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SkipJack )

// ============================================================================

const DL::TypeCPtr & Crypto::SkipJack::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "a258951a-111e-40cd-b85e-34fd9420a157" ),
		"BFC.Crypto.SkipJack",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"skipjack",
				"Cipher",
				"SkipJack Cipher" ),
			10, 10, 8, 32 ) );

	return i;

}

// ============================================================================

Crypto::SkipJack::SkipJack() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::SkipJack::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 10 ) {
		throw InvalidArgument();
	}

	return 10;

}

// ============================================================================

void Crypto::SkipJack::setKey(
	const	Buffer &	pKey ) {

	if ( pKey.getLength() != 10 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	System::memcpy( key, pKey.getCstPtr(), 10 );

}

// ============================================================================

void Crypto::SkipJack::encrypt(
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

	Uint32	tmp,tmp1;
	Uint32	x, kp;

	/* load block */
	Uint32	w1 = ((Uint32)pt[0]<<8)|pt[1];
	Uint32	w2 = ((Uint32)pt[2]<<8)|pt[3];
	Uint32	w3 = ((Uint32)pt[4]<<8)|pt[5];
	Uint32	w4 = ((Uint32)pt[6]<<8)|pt[7];

	/* 8 rounds of RULE A */
	for (x = 1, kp = 0; x < 9; x++) {
		tmp = g_func(w1, kp);
		w1  = tmp ^ w4 ^ x;
		w4  = w3; w3 = w2;
		w2  = tmp;
	}

	/* 8 rounds of RULE B */
	for (; x < 17; x++) {
		tmp  = g_func(w1, kp);
		tmp1 = w4; w4  = w3;
		w3	= w1 ^ w2 ^ x;
		w1	= tmp1; w2 = tmp;
	}

	/* 8 rounds of RULE A */
	for (; x < 25; x++) {
		tmp = g_func(w1, kp);
		w1  = tmp ^ w4 ^ x;
		w4  = w3; w3 = w2;
		w2  = tmp;
	}

	/* 8 rounds of RULE B */
	for (; x < 33; x++) {
		tmp  = g_func(w1, kp);
		tmp1 = w4; w4  = w3;
		w3	= w1 ^ w2 ^ x;
		w1	= tmp1; w2 = tmp;
	}

	/* store block */
	ct[0] = (w1>>8); ct[1] = w1;
	ct[2] = (w2>>8); ct[3] = w2;
	ct[4] = (w3>>8); ct[5] = w3;
	ct[6] = (w4>>8); ct[7] = w4;

}

void Crypto::SkipJack::decrypt(
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

	Uint32 tmp;
	Uint32 x, kp;

	/* load block */
	Uint32 w1 = ((Uint32)ct[0]<<8)|ct[1];
	Uint32 w2 = ((Uint32)ct[2]<<8)|ct[3];
	Uint32 w3 = ((Uint32)ct[4]<<8)|ct[5];
	Uint32 w4 = ((Uint32)ct[6]<<8)|ct[7];

	/* 8 rounds of RULE B^-1

		Note the value "kp = 8" comes from "kp = (32 * 4) mod 10" where 32*4 is 128 which mod 10 is 8
	 */
	for (x = 32, kp = 8; x > 24; x--) {
		tmp = ig_func(w2, kp);
		w2  = tmp ^ w3 ^ x;
		w3  = w4; w4 = w1; w1 = tmp;
	}

	/* 8 rounds of RULE A^-1 */
	for (; x > 16; x--) {
		tmp = w1 ^ w2 ^ x;
		w1  = ig_func(w2, kp);
		w2  = w3; w3 = w4; w4 = tmp;
	}


	/* 8 rounds of RULE B^-1 */
	for (; x > 8; x--) {
		tmp = ig_func(w2, kp);
		w2  = tmp ^ w3 ^ x;
		w3  = w4; w4 = w1; w1 = tmp;
	}

	/* 8 rounds of RULE A^-1 */
	for (; x > 0; x--) {
		tmp = w1 ^ w2 ^ x;
		w1  = ig_func(w2, kp);
		w2  = w3; w3 = w4; w4 = tmp;
	}

	/* store block */
	pt[0] = (w1>>8); pt[1] = w1;
	pt[2] = (w2>>8); pt[3] = w2;
	pt[4] = (w3>>8); pt[5] = w3;
	pt[6] = (w4>>8); pt[7] = w4;

}

// ============================================================================

void Crypto::SkipJack::test() {

	static const struct {
		Uchar	key[ 10 ];
		Uchar	pt[ 8 ];
		Uchar	ct[ 8 ];
	} tests[] = {
		{
			 { 0x00, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 },
			 { 0x33, 0x22, 0x11, 0x00, 0xdd, 0xcc, 0xbb, 0xaa },
			 { 0x25, 0x87, 0xca, 0xe2, 0x7a, 0x12, 0xd3, 0x00 }
		}
	};

	for ( Uint32 x = 0; x < (sizeof(tests)/sizeof(tests[0])); x++) {

		Buffer	key( tests[ x ].key, 10 );
		Buffer	pt( tests[ x ].pt, 8 );
		Buffer	ct( tests[ x ].ct, 8 );

		setKey( key );

		Buffer	nct = Cipher::encrypt( pt );
		Buffer	npt = Cipher::decrypt( nct );

		if ( nct != ct
		  || npt != pt ) {
			throw InternalError( "SkipJack failed (pass 1)!" );
		}

		Buffer tmp = ct;

		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::encrypt( tmp );
		}
		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::decrypt( tmp );
		}

		if ( tmp != ct ) {
			throw InternalError( "SkipJack failed (pass 2)!" );
		}

	}

}

// ============================================================================

Uint32 Crypto::SkipJack::g_func(Uint32 w, Uint32 & kp ) const {

	Uchar g1 = (w >> 8);
	Uchar g2 = w;

	g1 ^= sbox[ g2 ^ key[ kp ] ]; kp = keystep[ kp ];
	g2 ^= sbox[ g1 ^ key[ kp ] ]; kp = keystep[ kp ];
	g1 ^= sbox[ g2 ^ key[ kp ] ]; kp = keystep[ kp ];
	g2 ^= sbox[ g1 ^ key[ kp ] ]; kp = keystep[ kp ];

	return ((Uint32)g1<<8)|(Uint32)g2;

}

Uint32 Crypto::SkipJack::ig_func(Uint32 w, Uint32 & kp ) const {

	Uchar g1 = (w >> 8);
	Uchar g2 = w;

	kp = ikeystep[kp]; g2 ^= sbox[g1^key[kp]];
	kp = ikeystep[kp]; g1 ^= sbox[g2^key[kp]];
	kp = ikeystep[kp]; g2 ^= sbox[g1^key[kp]];
	kp = ikeystep[kp]; g1 ^= sbox[g2^key[kp]];

	return ((Uint32)g1<<8)|(Uint32)g2;

}

// ============================================================================

const Uchar Crypto::SkipJack::sbox[256] = {
	0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
	0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
	0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
	0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
	0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
	0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
	0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
	0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
	0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
	0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
	0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
	0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
	0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
	0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
	0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
	0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

const Uint32 Crypto::SkipJack::keystep[] =  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

const Uint32 Crypto::SkipJack::ikeystep[] = { 9, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SkipJack > skipJackRegisterer;

// ============================================================================

