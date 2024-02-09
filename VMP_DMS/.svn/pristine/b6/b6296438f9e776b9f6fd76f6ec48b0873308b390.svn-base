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
//	BFC.Crypto.Safer.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.Safer.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Crypto::SaferBase
// ============================================================================

Crypto::SaferBase::SaferBase(
	const	CipherDescriptorCPtr &
				pDesc ) :

	Cipher	( pDesc ),
	rounds	( pDesc->getDefaultRounds() ) {

}

// ============================================================================

void Crypto::SaferBase::setRounds(
	const	Uint32		pRounds ) {

	rounds = ( pRounds > MaxRounds ? MaxRounds : pRounds );

}

// ============================================================================

void Crypto::SaferBase::encrypt(
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

	Uchar	a, b, c, d, e, f, g, h, t;
	Uchar *	pkey = key;
	Uint32	round = *pkey;

	a = pt[0]; b = pt[1]; c = pt[2]; d = pt[3];
	e = pt[4]; f = pt[5]; g = pt[6]; h = pt[7];

	if ( round > MaxRounds ) {
		round = MaxRounds;
	}

	while ( round-- > 0 ) {
		a ^= *++pkey; b += *++pkey; c += *++pkey; d ^= *++pkey;
		e ^= *++pkey; f += *++pkey; g += *++pkey; h ^= *++pkey;
		a = EXP(a) + *++pkey; b = LOG(b) ^ *++pkey;
		c = LOG(c) ^ *++pkey; d = EXP(d) + *++pkey;
		e = EXP(e) + *++pkey; f = LOG(f) ^ *++pkey;
		g = LOG(g) ^ *++pkey; h = EXP(h) + *++pkey;
		PHT(a, b); PHT(c, d); PHT(e, f); PHT(g, h);
		PHT(a, c); PHT(e, g); PHT(b, d); PHT(f, h);
		PHT(a, e); PHT(b, f); PHT(c, g); PHT(d, h);
		t = b; b = e; e = c; c = t; t = d; d = f; f = g; g = t;
	}

	a ^= *++pkey; b += *++pkey; c += *++pkey; d ^= *++pkey;
	e ^= *++pkey; f += *++pkey; g += *++pkey; h ^= *++pkey;

	ct[0] = a; ct[1] = b;
	ct[2] = c; ct[3] = d;
	ct[4] = e; ct[5] = f;
	ct[6] = g; ct[7] = h;

}

void Crypto::SaferBase::decrypt(
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

	Uchar	a, b, c, d, e, f, g, h, t;
	Uchar *	pkey = key;
	Uint32	round = *pkey;

	a = ct[0]; b = ct[1]; c = ct[2]; d = ct[3];
	e = ct[4]; f = ct[5]; g = ct[6]; h = ct[7];

	if ( round > MaxRounds ) {
		round = MaxRounds;
	}

	pkey += CBlockLen * (1 + 2 * round);

	h ^= *pkey; g -= *--pkey; f -= *--pkey; e ^= *--pkey;
	d ^= *--pkey; c -= *--pkey; b -= *--pkey; a ^= *--pkey;

	while (round--) {
		t = e; e = b; b = c; c = t; t = f; f = d; d = g; g = t;
		IPHT(a, e); IPHT(b, f); IPHT(c, g); IPHT(d, h);
		IPHT(a, c); IPHT(e, g); IPHT(b, d); IPHT(f, h);
		IPHT(a, b); IPHT(c, d); IPHT(e, f); IPHT(g, h);
		h -= *--pkey; g ^= *--pkey; f ^= *--pkey; e -= *--pkey;
		d -= *--pkey; c ^= *--pkey; b ^= *--pkey; a -= *--pkey;
		h = LOG(h) ^ *--pkey; g = EXP(g) - *--pkey;
		f = EXP(f) - *--pkey; e = LOG(e) ^ *--pkey;
		d = LOG(d) ^ *--pkey; c = EXP(c) - *--pkey;
		b = EXP(b) - *--pkey; a = LOG(a) ^ *--pkey;
	}

	pt[0] = a; pt[1] = b;
	pt[2] = c; pt[3] = d;
	pt[4] = e; pt[5] = f;
	pt[6] = g; pt[7] = h;

}

// ============================================================================

const Uchar Crypto::SaferBase::EBox[256] = {
	  1,  45, 226, 147, 190,  69,  21, 174, 120,	3, 135, 164, 184,  56, 207,  63,
	  8, 103,	9, 148, 235,  38, 168, 107, 189,  24,  52,  27, 187, 191, 114, 247,
	 64,  53,  72, 156,  81,  47,  59,  85, 227, 192, 159, 216, 211, 243, 141, 177,
	255, 167,  62, 220, 134, 119, 215, 166,  17, 251, 244, 186, 146, 145, 100, 131,
	241,  51, 239, 218,  44, 181, 178,  43, 136, 209, 153, 203, 140, 132,  29,  20,
	129, 151, 113, 202,  95, 163, 139,  87,  60, 130, 196,  82,  92,  28, 232, 160,
	  4, 180, 133,  74, 246,  19,  84, 182, 223,  12,  26, 142, 222, 224,  57, 252,
	 32, 155,  36,  78, 169, 152, 158, 171, 242,  96, 208, 108, 234, 250, 199, 217,
	  0, 212,  31, 110,  67, 188, 236,  83, 137, 254, 122,  93,  73, 201,  50, 194,
	249, 154, 248, 109,  22, 219,  89, 150,  68, 233, 205, 230,  70,  66, 143,  10,
	193, 204, 185, 101, 176, 210, 198, 172,  30,  65,  98,  41,  46,  14, 116,  80,
	  2,  90, 195,  37, 123, 138,  42,  91, 240,	6,  13,  71, 111, 112, 157, 126,
	 16, 206,  18,  39, 213,  76,  79, 214, 121,  48, 104,  54, 117, 125, 228, 237,
	128, 106, 144,  55, 162,  94, 118, 170, 197, 127,  61, 175, 165, 229,  25,  97,
	253,  77, 124, 183,  11, 238, 173,  75,  34, 245, 231, 115,  35,  33, 200,	5,
	225, 102, 221, 179,  88, 105,  99,  86,  15, 161,  49, 149,  23,	7,  58,  40
};

const Uchar Crypto::SaferBase::LBox[256] = {
	128,	0, 176,	9,  96, 239, 185, 253,  16,  18, 159, 228, 105, 186, 173, 248,
	192,  56, 194, 101,  79,	6, 148, 252,  25, 222, 106,  27,  93,  78, 168, 130,
	112, 237, 232, 236, 114, 179,  21, 195, 255, 171, 182,  71,  68,	1, 172,  37,
	201, 250, 142,  65,  26,  33, 203, 211,  13, 110, 254,  38,  88, 218,  50,  15,
	 32, 169, 157, 132, 152,	5, 156, 187,  34, 140,  99, 231, 197, 225, 115, 198,
	175,  36,  91, 135, 102,  39, 247,  87, 244, 150, 177, 183,  92, 139, 213,  84,
	121, 223, 170, 246,  62, 163, 241,  17, 202, 245, 209,  23, 123, 147, 131, 188,
	189,  82,  30, 235, 174, 204, 214,  53,	8, 200, 138, 180, 226, 205, 191, 217,
	208,  80,  89,  63,  77,  98,  52,  10,  72, 136, 181,  86,  76,  46, 107, 158,
	210,  61,  60,	3,  19, 251, 151,  81, 117,  74, 145, 113,  35, 190, 118,  42,
	 95, 249, 212,  85,  11, 220,  55,  49,  22, 116, 215, 119, 167, 230,	7, 219,
	164,  47,  70, 243,  97,  69, 103, 227,  12, 162,  59,  28, 133,  24,	4,  29,
	 41, 160, 143, 178,  90, 216, 166, 126, 238, 141,  83,  75, 161, 154, 193,  14,
	122,  73, 165,  44, 129, 196, 199,  54,  43, 127,  67, 149,  51, 242, 108, 104,
	109, 240,	2,  40, 206, 221, 155, 234,  94, 153, 124,  20, 134, 207, 229,  66,
	184,  64, 120,  45,  58, 233, 100,  31, 146, 144, 125,  57, 111, 224, 137,  48
};

// ============================================================================

const Uint32 Crypto::SaferBase::MaxRounds;	// DEFINITION IS NEEDED!!!
const Uint32 Crypto::SaferBase::CBlockLen;	// DEFINITION IS NEEDED!!!
const Uint32 Crypto::SaferBase::CKeyLen;	// DEFINITION IS NEEDED!!!

// ============================================================================

void Crypto::SaferBase::expandKey(
	const	Uchar *		userkey_1,
	const	Uchar *		userkey_2,
	const	Bool		strengthened ) {

	Uint32	nbrRounds = rounds;
	Uint32	k;
	Uchar	ka[CBlockLen + 1];
	Uchar	kb[CBlockLen + 1];

	Uchar *	pkey = key;

	*pkey++ = (Uchar)nbrRounds;

	ka[CBlockLen] = (Uchar)0;
	kb[CBlockLen] = (Uchar)0;

	k = 0;

	for ( Uint32 j = 0 ; j < CBlockLen ; j++ ) {
		ka[j] = ROL8(userkey_1[j], 5);
		ka[CBlockLen] ^= ka[j];
		kb[j] = *pkey++ = userkey_2[j];
		kb[CBlockLen] ^= kb[j];
	}

	for ( Uint32 i = 1 ; i <= nbrRounds ; i++ ) {
		for ( Uint32 j = 0 ; j < CBlockLen + 1 ; j++ ) {
			ka[j] = ROL8(ka[j], 6);
			kb[j] = ROL8(kb[j], 6);
		}
		if (strengthened) {
			k = 2 * i - 1;
			while (k >= (CBlockLen + 1)) { k -= CBlockLen + 1; }
		}
		for ( Uint32 j = 0 ; j < CBlockLen ; j++ ) {
			if (strengthened) {
				*pkey++ = (ka[k] + EBox[(int)EBox[(int)((18 * i + j + 1)&0xFF)]]) & 0xFF;
				if (++k == (CBlockLen + 1)) {
					k = 0;
				}
			}
			else {
				*pkey++ = (ka[j] + EBox[(int)EBox[(int)((18 * i + j + 1)&0xFF)]]) & 0xFF;
			}
		}
		if (strengthened) {
			k = 2 * i;
			while (k >= (CBlockLen + 1)) {
				k -= CBlockLen + 1;
			}
		}
		for ( Uint32 j = 0 ; j < CBlockLen ; j++ ) {
			if (strengthened) {
				*pkey++ = (kb[k] + EBox[(int)EBox[(int)((18 * i + j + 10)&0xFF)]]) & 0xFF;
				if (++k == (CBlockLen + 1)) { k = 0; }
			}
			else {
				*pkey++ = (kb[j] + EBox[(int)EBox[(int)((18 * i + j + 10)&0xFF)]]) & 0xFF;
			}
		}
	}

}

// ============================================================================
// BFC::Crypto::Safer64
// ============================================================================

Crypto::Safer64::Safer64(
	const	CipherDescriptorCPtr &
				pDesc ) :

	SaferBase( pDesc ) {

}

// ============================================================================

Uint32 Crypto::Safer64::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 8 ) {
		throw InvalidArgument();
	}

	return 8;

}

// ============================================================================
// BFC::Crypto::SaferK64
// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SaferK64 )

// ============================================================================

const DL::TypeCPtr & Crypto::SaferK64::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "dee01034-cd5c-42c6-8e04-b1968a8ed87b" ),
		"BFC.Crypto.SaferK64",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"safer-k64",
				"Cipher",
				"Safer-K64 Cipher" ),
			8, 8, 8, 6 ) );

	return i;

}

// ============================================================================

Crypto::SaferK64::SaferK64() :

	Safer64	( getClassType() ) {

}

// ============================================================================

void Crypto::SaferK64::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	keylen = pKey.getLength();

	if ( keylen != 8 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	expandKey(
		key,
		key,
		false );

}

// ============================================================================

void Crypto::SaferK64::test() {

	static const Uchar k64_key[] = { 8, 7, 6, 5, 4, 3, 2, 1 };
	static const Uchar k64_pt[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	static const Uchar k64_ct[]  = { 200, 242, 156, 221, 135, 120, 62, 217 };

	Buffer	key( k64_key, 8 );
	Buffer	pt( k64_pt, 8 );
	Buffer	ct( k64_ct, 8 );

	setKey( key /*, 6 */ );

	Buffer nct = Cipher::encrypt( pt );
	Buffer npt = Cipher::decrypt( nct );

	if ( nct != ct || npt != pt ) {
		throw InternalError( "Safer-k64 failed (pass 1)!" );
	}

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SaferK64 > saferK64Registerer;

// ============================================================================
// BFC::Crypto::SaferSK64
// ============================================================================

const DL::TypeCPtr & Crypto::SaferSK64::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e91cd6ff-87c3-4349-bff9-f76350331d9b" ),
		"BFC.Crypto.SaferSK64",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"safer-sk64",
				"Cipher",
				"Safer-SK64 Cipher" ),
			8, 8, 8, 8 ) );

	return i;

}

// ============================================================================

Crypto::SaferSK64::SaferSK64() :

	Safer64( getClassType() ) {

}

// ============================================================================

void Crypto::SaferSK64::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	keylen = pKey.getLength();

	if ( keylen != 8 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	expandKey(
		key,
		key,
		true );

}

// ============================================================================

void Crypto::SaferSK64::test() {

	static const Uchar sk64_pt[]  = { 1, 2, 3, 4, 5, 6, 7, 8 };
	static const Uchar sk64_key[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	static const Uchar sk64_ct[]  = { 95, 206, 155, 162, 5, 132, 56, 199 };

	Buffer	key( sk64_key, 8 );
	Buffer	pt( sk64_pt, 8 );
	Buffer	ct( sk64_ct, 8 );

	setRounds( 6 );
	setKey( key );

	Buffer nct = Cipher::encrypt( pt );
	Buffer npt = Cipher::decrypt( nct );

	if ( nct != ct || npt != pt ) {
		throw InternalError( "Safer-SK64 failed (pass 1)!" );
	}

	Buffer tmp = ct;

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		tmp = Cipher::encrypt( tmp );
	}

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		tmp = Cipher::decrypt( tmp );
	}

	if ( tmp != ct ) {
		throw InternalError( "Safer-SK64 failed (pass 2)!" );
	}

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SaferSK64 > saferSK64Registerer;

// ============================================================================
// BFC::Crypto::Safer128
// ============================================================================

Crypto::Safer128::Safer128(
	const	CipherDescriptorCPtr &	pDesc ) :

	SaferBase( pDesc ) {

}

// ============================================================================

Uint32 Crypto::Safer128::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 16 ) {
		throw InvalidArgument();
	}

	return 16;

}

// ============================================================================
// BFC::Crypto::SaferK128
// ============================================================================

const DL::TypeCPtr & Crypto::SaferK128::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "07aabf67-7e3b-44fb-9ef7-07c5f14dbafa" ),
		"BFC.Crypto.SaferK128",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"safer-k128",
				"Cipher",
				"Safer-K128 Cipher" ),
			16, 16, 8, 10 ) );

	return i;

}

// ============================================================================

Crypto::SaferK128::SaferK128() :

	Safer128( getClassType() ) {

}

// ============================================================================

void Crypto::SaferK128::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	keylen = pKey.getLength();

	if ( keylen != 16 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	expandKey(
		key,
		key + 8,
		false );

}

// ============================================================================

void Crypto::SaferK128::test() {

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SaferK128 > saferK128Registerer;

// ============================================================================
// BFC::Crypto::SaferSK128
// ============================================================================

const DL::TypeCPtr & Crypto::SaferSK128::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "42780055-cbe4-4fae-a557-361166b3b014" ),
		"BFC.Crypto.SaferSK128",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"safer-sk128",
				"Cipher",
				"Safer-SK128 Cipher" ),
			16, 16, 8, 10 ) );

	return i;

}

// ============================================================================

Crypto::SaferSK128::SaferSK128() :

	Safer128( getClassType() ) {

}

// ============================================================================

void Crypto::SaferSK128::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	keylen = pKey.getLength();

	if ( keylen != 16 ) {
		throw InvalidArgument( "Invalid key!" );
	}

	expandKey(
		key,
		key + 8,
		true );

}

// ============================================================================

void Crypto::SaferSK128::test() {

	static const Uchar sk128_key[] = { 1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0 };
	static const Uchar sk128_pt[]  = { 1, 2, 3, 4, 5, 6, 7, 8 };
	static const Uchar sk128_ct[]  = { 255, 120, 17, 228, 179, 167, 46, 113 };

	Buffer	key( sk128_key, 16 );
	Buffer	pt( sk128_pt, 8 );
	Buffer	ct( sk128_ct, 8 );

	setKey( key );

	Buffer nct = Cipher::encrypt( pt );
	Buffer npt = Cipher::decrypt( nct );

	if ( nct != ct || npt != pt ) {
		throw InternalError( "Safer-SK128 failed (pass 1)!" );
	}

	Buffer tmp = ct;

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		tmp = Cipher::encrypt( tmp );
	}

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		tmp = Cipher::decrypt( tmp );
	}

	if ( tmp != ct ) {
		throw InternalError( "Safer-SK128 failed (pass 2)!" );
	}

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SaferSK128 > saferSK128Registerer;

// ============================================================================
// BFC::Crypto::SapherPlus
// ============================================================================

const DL::TypeCPtr & Crypto::SaferPlus::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "dc5478c8-e74c-458d-b203-facf70b00b9e" ),
		"BFC.Crypto.SaferPlus",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"safer+",
				"Cipher",
				"Safer+ Cipher" ),
			16, 32, 16, 8 ) );

	return i;

}

// ============================================================================

Crypto::SaferPlus::SaferPlus() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::SaferPlus::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 16 ) {
		throw InvalidArgument();
	}

	if ( pKeySize < 24 ) {
		return 16;
	}

	if ( pKeySize < 32 ) {
		return 24;
	}

	return 32;

}

// ============================================================================

void Crypto::SaferPlus::setKey(
	const	Buffer &	pKey ) {

	const Uchar * key = pKey.getCstPtr();
	Uint32 keylen = pKey.getLength();

	if ( keylen != 16 && keylen != 24 && keylen != 32 ) {
		throw InvalidArgument( "Invalid key!" );
	}

//	static const Uint32 tbl[3] = { 8, 12, 16 };

	Uint32 x, y, z;
	Uchar t[33];

	/* 128 bit key version */
	if (keylen == 16) {
		/* copy key into t */
		for (x = y = 0; x < 16; x++) {
			t[x] = key[x];
			y ^= key[x];
		}
		t[16] = y;

		/* make round keys */
		for (x = 0; x < 16; x++) {
			K[0][x] = t[x];
		}

		/* make the 16 other keys as a transformation of the first key */
		for (x = 1; x < 17; x++) {
			/* rotate 3 bits each */
			for (y = 0; y < 17; y++) {
					t[y] = ((t[y]<<3)|(t[y]>>5)) & 255;
			}

			/* select and add */
			z = x;
			for (y = 0; y < 16; y++) {
					K[x][y] = (t[z] + safer_bias[x-1][y]) & 255;
					if (++z == 17) { z = 0; }
			}
		}
		rounds = 8;
	} else if (keylen == 24) {
		/* copy key into t */
		for (x = y = 0; x < 24; x++) {
			t[x] = key[x];
			y ^= key[x];
		}
		t[24] = y;

		/* make round keys */
		for (x = 0; x < 16; x++) {
			K[0][x] = t[x];
		}

		for (x = 1; x < 25; x++) {
			/* rotate 3 bits each */
			for (y = 0; y < 25; y++) {
					t[y] = ((t[y]<<3)|(t[y]>>5)) & 255;
			}

			/* select and add */
			z = x;
			for (y = 0; y < 16; y++) {
					K[x][y] = (t[z] + safer_bias[x-1][y]) & 255;
					if (++z == 25) { z = 0; }
			}
		}
		rounds = 12;
	} else {
		/* copy key into t */
		for (x = y = 0; x < 32; x++) {
			t[x] = key[x];
			y ^= key[x];
		}
		t[32] = y;

		/* make round keys */
		for (x = 0; x < 16; x++) {
			K[0][x] = t[x];
		}

		for (x = 1; x < 33; x++) {
			/* rotate 3 bits each */
			for (y = 0; y < 33; y++) {
					t[y] = ((t[y]<<3)|(t[y]>>5)) & 255;
			}

			/* select and add */
			z = x;
			for (y = 0; y < 16; y++) {
					K[x][y] = (t[z] + safer_bias[x-1][y]) & 255;
					if (++z == 33) { z = 0; }
			}
		}
		rounds = 16;
	}

}

// ============================================================================

void Crypto::SaferPlus::encrypt(
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

	Uchar b[16];

	for ( Uint32 x = 0; x < 16; x++) {
		b[x] = pt[x];
	}

	ROUND(b,  0);  LT(b, ct);
	ROUND(ct, 2);  LT(ct, b);
	ROUND(b,  4);  LT(b, ct);
	ROUND(ct, 6);  LT(ct, b);
	ROUND(b,  8);  LT(b, ct);
	ROUND(ct, 10); LT(ct, b);
	ROUND(b,  12); LT(b, ct);
	ROUND(ct, 14); LT(ct, b);

	if (rounds > 8) {
		ROUND(b, 16);  LT(b, ct);
		ROUND(ct, 18); LT(ct, b);
		ROUND(b, 20);  LT(b, ct);
		ROUND(ct, 22); LT(ct, b);
	}

	if (rounds > 12) {
		ROUND(b, 24);  LT(b, ct);
		ROUND(ct, 26); LT(ct, b);
		ROUND(b, 28);  LT(b, ct);
		ROUND(ct, 30); LT(ct, b);
	}

	ct[0] = b[0] ^ K[rounds*2][0];
	ct[1] = (b[1] + K[rounds*2][1]) & 255;
	ct[2] = (b[2] + K[rounds*2][2]) & 255;
	ct[3] = b[3] ^ K[rounds*2][3];
	ct[4] = b[4] ^ K[rounds*2][4];
	ct[5] = (b[5] + K[rounds*2][5]) & 255;
	ct[6] = (b[6] + K[rounds*2][6]) & 255;
	ct[7] = b[7] ^ K[rounds*2][7];
	ct[8] = b[8] ^ K[rounds*2][8];
	ct[9] = (b[9] + K[rounds*2][9]) & 255;
	ct[10] = (b[10] + K[rounds*2][10]) & 255;
	ct[11] = b[11] ^ K[rounds*2][11];
	ct[12] = b[12] ^ K[rounds*2][12];
	ct[13] = (b[13] + K[rounds*2][13]) & 255;
	ct[14] = (b[14] + K[rounds*2][14]) & 255;
	ct[15] = b[15] ^ K[rounds*2][15];

}

void Crypto::SaferPlus::decrypt(
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

	Uchar b[16];

	b[0] = ct[0] ^ K[rounds*2][0];
	b[1] = (ct[1] - K[rounds*2][1]) & 255;
	b[2] = (ct[2] - K[rounds*2][2]) & 255;
	b[3] = ct[3] ^ K[rounds*2][3];
	b[4] = ct[4] ^ K[rounds*2][4];
	b[5] = (ct[5] - K[rounds*2][5]) & 255;
	b[6] = (ct[6] - K[rounds*2][6]) & 255;
	b[7] = ct[7] ^ K[rounds*2][7];
	b[8] = ct[8] ^ K[rounds*2][8];
	b[9] = (ct[9] - K[rounds*2][9]) & 255;
	b[10] = (ct[10] - K[rounds*2][10]) & 255;
	b[11] = ct[11] ^ K[rounds*2][11];
	b[12] = ct[12] ^ K[rounds*2][12];
	b[13] = (ct[13] - K[rounds*2][13]) & 255;
	b[14] = (ct[14] - K[rounds*2][14]) & 255;
	b[15] = ct[15] ^ K[rounds*2][15];

	if (rounds > 12) {
		iLT(b, pt); iROUND(pt, 30);
		iLT(pt, b); iROUND(b, 28);
		iLT(b, pt); iROUND(pt, 26);
		iLT(pt, b); iROUND(b, 24);
	}

	if (rounds > 8) {
		iLT(b, pt); iROUND(pt, 22);
		iLT(pt, b); iROUND(b, 20);
		iLT(b, pt); iROUND(pt, 18);
		iLT(pt, b); iROUND(b, 16);
	}

	iLT(b, pt); iROUND(pt, 14);
	iLT(pt, b); iROUND(b, 12);
	iLT(b, pt); iROUND(pt,10);
	iLT(pt, b); iROUND(b, 8);
	iLT(b, pt); iROUND(pt,6);
	iLT(pt, b); iROUND(b, 4);
	iLT(b, pt); iROUND(pt,2);
	iLT(pt, b); iROUND(b, 0);

	for ( Uint32 x = 0 ; x < 16 ; x++ ) {
		pt[x] = b[x];
	}

}

// ============================================================================

void Crypto::SaferPlus::test() {

	static const struct {
		Uint32	keylen;
		Uchar	key[32];
		Uchar	pt[16];
		Uchar	ct[16];
	} tests[] = {
		{
			16,
			{ 41, 35, 190, 132, 225, 108, 214, 174,
				82, 144, 73, 241, 241, 187, 233, 235 },
			{ 179, 166, 219, 60, 135, 12, 62, 153,
				36, 94, 13, 28, 6, 183, 71, 222 },
			{ 224, 31, 182, 10, 12, 255, 84, 70,
				127, 13, 89, 249, 9, 57, 165, 220 }
		}, {
			24,
			{ 72, 211, 143, 117, 230, 217, 29, 42,
				229, 192, 247, 43, 120, 129, 135, 68,
				14, 95, 80, 0, 212, 97, 141, 190 },
			{ 123, 5, 21, 7, 59, 51, 130, 31,
				24, 112, 146, 218, 100, 84, 206, 177 },
			{ 92, 136, 4, 63, 57, 95, 100, 0,
				150, 130, 130, 16, 193, 111, 219, 133 }
		}, {
			32,
			{ 243, 168, 141, 254, 190, 242, 235, 113,
				255, 160, 208, 59, 117, 6, 140, 126,
				135, 120, 115, 77, 208, 190, 130, 190,
				219, 194, 70, 65, 43, 140, 250, 48 },
			{ 127, 112, 240, 167, 84, 134, 50, 149,
				170, 91, 104, 19, 11, 230, 252, 245 },
			{ 88, 11, 25, 36, 172, 229, 202, 213,
				170, 65, 105, 153, 220, 104, 153, 138 }
		}
	};

	for ( Uint32 i = 0 ; i < (sizeof(tests) / sizeof(tests[0])) ; i++ ) {

		Buffer	key( tests[ i ].key, tests[ i ].keylen );
		Buffer	pt( tests[ i ].pt, 16 );
		Buffer	ct( tests[ i ].ct, 16 );

		setKey( key );

		Buffer nct = Cipher::encrypt( pt );
		Buffer npt = Cipher::decrypt( nct );

		if ( nct != ct || npt != pt ) {
			throw InternalError( "Safer+ failed (pass 1)!" );
		}

		Buffer tmp = ct;

		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::encrypt( tmp );
		}

		for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
			tmp = Cipher::decrypt( tmp );
		}

		if ( tmp != ct ) {
			throw InternalError( "Safer+ failed (pass 2)!" );
		}

	}

}

// ============================================================================

const Uchar Crypto::SaferPlus::safer_bias[33][16] = {
	{  70, 151, 177, 186, 163, 183,  16,  10, 197,  55, 179, 201,  90,  40, 172, 100},
	{ 236, 171, 170, 198, 103, 149,  88,  13, 248, 154, 246, 110, 102, 220,	5,  61},
	{ 138, 195, 216, 137, 106, 233,  54,  73,  67, 191, 235, 212, 150, 155, 104, 160},
	{  93,  87, 146,  31, 213, 113,  92, 187,  34, 193, 190, 123, 188, 153,  99, 148},
	{  42,  97, 184,  52,  50,  25, 253, 251,  23,  64, 230,  81,  29,  65,  68, 143},
	{ 221,	4, 128, 222, 231,  49, 214, 127,	1, 162, 247,  57, 218, 111,  35, 202},
	{  58, 208,  28, 209,  48,  62,  18, 161, 205,  15, 224, 168, 175, 130,  89,  44},
	{ 125, 173, 178, 239, 194, 135, 206, 117,	6,  19,	2, 144,  79,  46, 114,  51},
	{ 192, 141, 207, 169, 129, 226, 196,  39,  47, 108, 122, 159,  82, 225,  21,  56},
	{ 252,  32,  66, 199,	8, 228,	9,  85,  94, 140,  20, 118,  96, 255, 223, 215},
	{ 250,  11,  33,	0,  26, 249, 166, 185, 232, 158,  98,  76, 217, 145,  80, 210},
	{  24, 180,	7, 132, 234,  91, 164, 200,  14, 203,  72, 105,  75,  78, 156,  53},
	{  69,  77,  84, 229,  37,  60,  12,  74, 139,  63, 204, 167, 219, 107, 174, 244},
	{  45, 243, 124, 109, 157, 181,  38, 116, 242, 147,  83, 176, 240,  17, 237, 131},
	{ 182,	3,  22, 115,  59,  30, 142, 112, 189, 134,  27,  71, 126,  36,  86, 241},
	{ 136,  70, 151, 177, 186, 163, 183,  16,  10, 197,  55, 179, 201,  90,  40, 172},
	{ 220, 134, 119, 215, 166,  17, 251, 244, 186, 146, 145, 100, 131, 241,  51, 239},
	{  44, 181, 178,  43, 136, 209, 153, 203, 140, 132,  29,  20, 129, 151, 113, 202},
	{ 163, 139,  87,  60, 130, 196,  82,  92,  28, 232, 160,	4, 180, 133,  74, 246},
	{  84, 182, 223,  12,  26, 142, 222, 224,  57, 252,  32, 155,  36,  78, 169, 152},
	{ 171, 242,  96, 208, 108, 234, 250, 199, 217,	0, 212,  31, 110,  67, 188, 236},
	{ 137, 254, 122,  93,  73, 201,  50, 194, 249, 154, 248, 109,  22, 219,  89, 150},
	{ 233, 205, 230,  70,  66, 143,  10, 193, 204, 185, 101, 176, 210, 198, 172,  30},
	{  98,  41,  46,  14, 116,  80,	2,  90, 195,  37, 123, 138,  42,  91, 240,	6},
	{  71, 111, 112, 157, 126,  16, 206,  18,  39, 213,  76,  79, 214, 121,  48, 104},
	{ 117, 125, 228, 237, 128, 106, 144,  55, 162,  94, 118, 170, 197, 127,  61, 175},
	{ 229,  25,  97, 253,  77, 124, 183,  11, 238, 173,  75,  34, 245, 231, 115,  35},
	{ 200,	5, 225, 102, 221, 179,  88, 105,  99,  86,  15, 161,  49, 149,  23,	7},
	{  40,	1,  45, 226, 147, 190,  69,  21, 174, 120,	3, 135, 164, 184,  56, 207},
	{	8, 103,	9, 148, 235,  38, 168, 107, 189,  24,  52,  27, 187, 191, 114, 247},
	{  53,  72, 156,  81,  47,  59,  85, 227, 192, 159, 216, 211, 243, 141, 177, 255},
	{  62, 220, 134, 119, 215, 166,  17, 251, 244, 186, 146, 145, 100, 131, 241,  51}
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::SaferPlus > saferPlusRegisterer;

// ============================================================================

