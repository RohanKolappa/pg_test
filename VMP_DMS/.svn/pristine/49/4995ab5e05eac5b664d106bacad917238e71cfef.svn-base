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
//	BFC.Crypto.RIPEMD128.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.RIPEMD128.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RIPEMD128 )

// ============================================================================

const DL::TypeCPtr & Crypto::RIPEMD128::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b8ff093f-e87d-4e52-87d8-cefb2900f1ec" ),
		"BFC.Crypto.RIPEMD128",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"rmd128",
				"Hash",
				"RIPEMD-128 Hash" ),
			16,
			64 ) );

	return i;

}

// ============================================================================

Crypto::RIPEMD128::RIPEMD128() :

	Hash( getClassType() ) {

	init();

}

// ============================================================================

void Crypto::RIPEMD128::init() {

	msgBuf.kill();
	msgLen = 0;

	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;

}

void Crypto::RIPEMD128::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

}

void Crypto::RIPEMD128::done(
		Buffer &	out ) {

	msgLen <<= 3;
	msgBuf += ( Uchar )0x80;

	if ( msgBuf.getLength() > 56 ) {
		msgBuf += Buffer::chars( 128 - msgBuf.getLength(), 0x00 );
	}
	else {
		msgBuf += Buffer::chars( 64 - msgBuf.getLength(), 0x00 );
	}

	STORE64L( msgLen, msgBuf.getVarPtr() + msgBuf.getLength() - 8 );

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

	if ( out.getLength() != 16 ) {
		out.resize( 16 );
	}

	Uchar * ptr = out.getVarPtr();

	for ( Uint32 i = 0 ; i < 4 ; i++ ) {
		STORE32L( state[ i ], ptr + 4 * i );
	}

}

// ============================================================================

void Crypto::RIPEMD128::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[ 16 ];
	} tests[] = {
	{ "",
	{ 0xcd, 0xf2, 0x62, 0x13, 0xa1, 0x50, 0xdc, 0x3e,
		0xcb, 0x61, 0x0f, 0x18, 0xf6, 0xb3, 0x8b, 0x46 }
	},
	{ "a",
	{ 0x86, 0xbe, 0x7a, 0xfa, 0x33, 0x9d, 0x0f, 0xc7,
		0xcf, 0xc7, 0x85, 0xe7, 0x2f, 0x57, 0x8d, 0x33 }
	},
	{ "abc",
	{ 0xc1, 0x4a, 0x12, 0x19, 0x9c, 0x66, 0xe4, 0xba,
		0x84, 0x63, 0x6b, 0x0f, 0x69, 0x14, 0x4c, 0x77 }
	},
	{ "message digest",
	{ 0x9e, 0x32, 0x7b, 0x3d, 0x6e, 0x52, 0x30, 0x62,
		0xaf, 0xc1, 0x13, 0x2d, 0x7d, 0xf9, 0xd1, 0xb8 }
	},
	{ "abcdefghijklmnopqrstuvwxyz",
	{ 0xfd, 0x2a, 0xa6, 0x07, 0xf7, 0x1d, 0xc8, 0xf5,
		0x10, 0x71, 0x49, 0x22, 0xb3, 0x71, 0x83, 0x4e }
	},
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
	{ 0xd1, 0xe9, 0x59, 0xeb, 0x17, 0x9c, 0x91, 0x1f,
		0xae, 0xa4, 0x62, 0x4c, 0x60, 0xc5, 0xc7, 0x02 }
	}
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < ( Uint32 )( sizeof( tests ) / sizeof( tests[ 0 ] ) ) ; i++ ) {
		init();
		process( tests[ i ].msg );
		done( tmp );
		Buffer exp( tests[i].hash, 16 );
		if ( tmp != exp ) {
			throw InternalError( "Step " + Buffer( i ) + "\n"
				"exp: " + Utils::toHex( exp ) + "\n"
				"got: " + Utils::toHex( tmp ) );
		}
	}

}

// ============================================================================

/* the four basic functions F(), G() and H() */
#define F(x, y, z)		((x) ^ (y) ^ (z))
#define G(x, y, z)		(((x) & (y)) | (~(x) & (z)))
#define H(x, y, z)		(((x) | ~(y)) ^ (z))
#define I(x, y, z)		(((x) & (z)) | ((y) & ~(z)))

/* the eight basic operations FF() through III() */
#define FF(a, b, c, d, x, s)		\
		(a) += F((b), (c), (d)) + (x);\
		(a) = ROL((a), (s));

#define GG(a, b, c, d, x, s)		\
		(a) += G((b), (c), (d)) + (x) + 0x5a827999UL;\
		(a) = ROL((a), (s));

#define HH(a, b, c, d, x, s)		\
		(a) += H((b), (c), (d)) + (x) + 0x6ed9eba1UL;\
		(a) = ROL((a), (s));

#define II(a, b, c, d, x, s)		\
		(a) += I((b), (c), (d)) + (x) + 0x8f1bbcdcUL;\
		(a) = ROL((a), (s));

#define FFF(a, b, c, d, x, s)		\
		(a) += F((b), (c), (d)) + (x);\
		(a) = ROL((a), (s));

#define GGG(a, b, c, d, x, s)		\
		(a) += G((b), (c), (d)) + (x) + 0x6d703ef3UL;\
		(a) = ROL((a), (s));

#define HHH(a, b, c, d, x, s)		\
		(a) += H((b), (c), (d)) + (x) + 0x5c4dd124UL;\
		(a) = ROL((a), (s));

#define III(a, b, c, d, x, s)		\
		(a) += I((b), (c), (d)) + (x) + 0x50a28be6UL;\
		(a) = ROL((a), (s));

void Crypto::RIPEMD128::compress(
	const	Uchar *		buf ) {

	Uint32 aa,bb,cc,dd,aaa,bbb,ccc,ddd,X[16];
	Uint32	i;

	/* load words X */
	for (i = 0; i < 16; i++){
		X[i] = LOAD32L( buf + (4 * i));
	}

	/* load state */
	aa = aaa = state[0];
	bb = bbb = state[1];
	cc = ccc = state[2];
	dd = ddd = state[3];

	/* round 1 */
	FF(aa, bb, cc, dd, X[ 0], 11);
	FF(dd, aa, bb, cc, X[ 1], 14);
	FF(cc, dd, aa, bb, X[ 2], 15);
	FF(bb, cc, dd, aa, X[ 3], 12);
	FF(aa, bb, cc, dd, X[ 4],  5);
	FF(dd, aa, bb, cc, X[ 5],  8);
	FF(cc, dd, aa, bb, X[ 6],  7);
	FF(bb, cc, dd, aa, X[ 7],  9);
	FF(aa, bb, cc, dd, X[ 8], 11);
	FF(dd, aa, bb, cc, X[ 9], 13);
	FF(cc, dd, aa, bb, X[10], 14);
	FF(bb, cc, dd, aa, X[11], 15);
	FF(aa, bb, cc, dd, X[12],  6);
	FF(dd, aa, bb, cc, X[13],  7);
	FF(cc, dd, aa, bb, X[14],  9);
	FF(bb, cc, dd, aa, X[15],  8);

	/* round 2 */
	GG(aa, bb, cc, dd, X[ 7],  7);
	GG(dd, aa, bb, cc, X[ 4],  6);
	GG(cc, dd, aa, bb, X[13],  8);
	GG(bb, cc, dd, aa, X[ 1], 13);
	GG(aa, bb, cc, dd, X[10], 11);
	GG(dd, aa, bb, cc, X[ 6],  9);
	GG(cc, dd, aa, bb, X[15],  7);
	GG(bb, cc, dd, aa, X[ 3], 15);
	GG(aa, bb, cc, dd, X[12],  7);
	GG(dd, aa, bb, cc, X[ 0], 12);
	GG(cc, dd, aa, bb, X[ 9], 15);
	GG(bb, cc, dd, aa, X[ 5],  9);
	GG(aa, bb, cc, dd, X[ 2], 11);
	GG(dd, aa, bb, cc, X[14],  7);
	GG(cc, dd, aa, bb, X[11], 13);
	GG(bb, cc, dd, aa, X[ 8], 12);

	/* round 3 */
	HH(aa, bb, cc, dd, X[ 3], 11);
	HH(dd, aa, bb, cc, X[10], 13);
	HH(cc, dd, aa, bb, X[14],  6);
	HH(bb, cc, dd, aa, X[ 4],  7);
	HH(aa, bb, cc, dd, X[ 9], 14);
	HH(dd, aa, bb, cc, X[15],  9);
	HH(cc, dd, aa, bb, X[ 8], 13);
	HH(bb, cc, dd, aa, X[ 1], 15);
	HH(aa, bb, cc, dd, X[ 2], 14);
	HH(dd, aa, bb, cc, X[ 7],  8);
	HH(cc, dd, aa, bb, X[ 0], 13);
	HH(bb, cc, dd, aa, X[ 6],  6);
	HH(aa, bb, cc, dd, X[13],  5);
	HH(dd, aa, bb, cc, X[11], 12);
	HH(cc, dd, aa, bb, X[ 5],  7);
	HH(bb, cc, dd, aa, X[12],  5);

	/* round 4 */
	II(aa, bb, cc, dd, X[ 1], 11);
	II(dd, aa, bb, cc, X[ 9], 12);
	II(cc, dd, aa, bb, X[11], 14);
	II(bb, cc, dd, aa, X[10], 15);
	II(aa, bb, cc, dd, X[ 0], 14);
	II(dd, aa, bb, cc, X[ 8], 15);
	II(cc, dd, aa, bb, X[12],  9);
	II(bb, cc, dd, aa, X[ 4],  8);
	II(aa, bb, cc, dd, X[13],  9);
	II(dd, aa, bb, cc, X[ 3], 14);
	II(cc, dd, aa, bb, X[ 7],  5);
	II(bb, cc, dd, aa, X[15],  6);
	II(aa, bb, cc, dd, X[14],  8);
	II(dd, aa, bb, cc, X[ 5],  6);
	II(cc, dd, aa, bb, X[ 6],  5);
	II(bb, cc, dd, aa, X[ 2], 12);

	/* parallel round 1 */
	III(aaa, bbb, ccc, ddd, X[ 5],  8);
	III(ddd, aaa, bbb, ccc, X[14],  9);
	III(ccc, ddd, aaa, bbb, X[ 7],  9);
	III(bbb, ccc, ddd, aaa, X[ 0], 11);
	III(aaa, bbb, ccc, ddd, X[ 9], 13);
	III(ddd, aaa, bbb, ccc, X[ 2], 15);
	III(ccc, ddd, aaa, bbb, X[11], 15);
	III(bbb, ccc, ddd, aaa, X[ 4],  5);
	III(aaa, bbb, ccc, ddd, X[13],  7);
	III(ddd, aaa, bbb, ccc, X[ 6],  7);
	III(ccc, ddd, aaa, bbb, X[15],  8);
	III(bbb, ccc, ddd, aaa, X[ 8], 11);
	III(aaa, bbb, ccc, ddd, X[ 1], 14);
	III(ddd, aaa, bbb, ccc, X[10], 14);
	III(ccc, ddd, aaa, bbb, X[ 3], 12);
	III(bbb, ccc, ddd, aaa, X[12],  6);

	/* parallel round 2 */
	HHH(aaa, bbb, ccc, ddd, X[ 6],  9);
	HHH(ddd, aaa, bbb, ccc, X[11], 13);
	HHH(ccc, ddd, aaa, bbb, X[ 3], 15);
	HHH(bbb, ccc, ddd, aaa, X[ 7],  7);
	HHH(aaa, bbb, ccc, ddd, X[ 0], 12);
	HHH(ddd, aaa, bbb, ccc, X[13],  8);
	HHH(ccc, ddd, aaa, bbb, X[ 5],  9);
	HHH(bbb, ccc, ddd, aaa, X[10], 11);
	HHH(aaa, bbb, ccc, ddd, X[14],  7);
	HHH(ddd, aaa, bbb, ccc, X[15],  7);
	HHH(ccc, ddd, aaa, bbb, X[ 8], 12);
	HHH(bbb, ccc, ddd, aaa, X[12],  7);
	HHH(aaa, bbb, ccc, ddd, X[ 4],  6);
	HHH(ddd, aaa, bbb, ccc, X[ 9], 15);
	HHH(ccc, ddd, aaa, bbb, X[ 1], 13);
	HHH(bbb, ccc, ddd, aaa, X[ 2], 11);

	/* parallel round 3 */
	GGG(aaa, bbb, ccc, ddd, X[15],  9);
	GGG(ddd, aaa, bbb, ccc, X[ 5],  7);
	GGG(ccc, ddd, aaa, bbb, X[ 1], 15);
	GGG(bbb, ccc, ddd, aaa, X[ 3], 11);
	GGG(aaa, bbb, ccc, ddd, X[ 7],  8);
	GGG(ddd, aaa, bbb, ccc, X[14],  6);
	GGG(ccc, ddd, aaa, bbb, X[ 6],  6);
	GGG(bbb, ccc, ddd, aaa, X[ 9], 14);
	GGG(aaa, bbb, ccc, ddd, X[11], 12);
	GGG(ddd, aaa, bbb, ccc, X[ 8], 13);
	GGG(ccc, ddd, aaa, bbb, X[12],  5);
	GGG(bbb, ccc, ddd, aaa, X[ 2], 14);
	GGG(aaa, bbb, ccc, ddd, X[10], 13);
	GGG(ddd, aaa, bbb, ccc, X[ 0], 13);
	GGG(ccc, ddd, aaa, bbb, X[ 4],  7);
	GGG(bbb, ccc, ddd, aaa, X[13],  5);

	/* parallel round 4 */
	FFF(aaa, bbb, ccc, ddd, X[ 8], 15);
	FFF(ddd, aaa, bbb, ccc, X[ 6],  5);
	FFF(ccc, ddd, aaa, bbb, X[ 4],  8);
	FFF(bbb, ccc, ddd, aaa, X[ 1], 11);
	FFF(aaa, bbb, ccc, ddd, X[ 3], 14);
	FFF(ddd, aaa, bbb, ccc, X[11], 14);
	FFF(ccc, ddd, aaa, bbb, X[15],  6);
	FFF(bbb, ccc, ddd, aaa, X[ 0], 14);
	FFF(aaa, bbb, ccc, ddd, X[ 5],  6);
	FFF(ddd, aaa, bbb, ccc, X[12],  9);
	FFF(ccc, ddd, aaa, bbb, X[ 2], 12);
	FFF(bbb, ccc, ddd, aaa, X[13],  9);
	FFF(aaa, bbb, ccc, ddd, X[ 9], 12);
	FFF(ddd, aaa, bbb, ccc, X[ 7],  5);
	FFF(ccc, ddd, aaa, bbb, X[10], 15);
	FFF(bbb, ccc, ddd, aaa, X[14],  8);

	/* combine results */
	ddd += cc + state[1];	/* final result for MDbuf[0] */
	state[1] = state[2] + dd + aaa;
	state[2] = state[3] + aa + bbb;
	state[3] = state[0] + bb + ccc;
	state[0] = ddd;

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::RIPEMD128 > rmd128Registerer;

// ============================================================================

