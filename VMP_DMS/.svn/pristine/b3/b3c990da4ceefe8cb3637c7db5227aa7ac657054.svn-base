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
//	BFC.Crypto.MD4.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.MD4.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, MD4 )

// ============================================================================

const DL::TypeCPtr & Crypto::MD4::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "41925d89-1548-4cb7-a2d4-db356c93ee06" ),
		"BFC.Crypto.MD4",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"md4",
				"Hash",
				"MD4 Hash" ),
			16,
			64 ) );

	return i;

}

// ============================================================================

Crypto::MD4::MD4() :

	Hash( getClassType() ) {

	init();

}

// ============================================================================

void Crypto::MD4::init() {

	msgBuf.kill();
	msgLen = 0;

	state[ 0 ] = 0x67452301UL;
	state[ 1 ] = 0xefcdab89UL;
	state[ 2 ] = 0x98badcfeUL;
	state[ 3 ] = 0x10325476UL;

}

void Crypto::MD4::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

}

void Crypto::MD4::done(
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

void Crypto::MD4::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[ 16 ];
	} tests[] = {
		{ "", 
			{0x31, 0xd6, 0xcf, 0xe0, 0xd1, 0x6a, 0xe9, 0x31,
			0xb7, 0x3c, 0x59, 0xd7, 0xe0, 0xc0, 0x89, 0xc0} },
		{ "a",
			{0xbd, 0xe5, 0x2c, 0xb3, 0x1d, 0xe3, 0x3e, 0x46,
			0x24, 0x5e, 0x05, 0xfb, 0xdb, 0xd6, 0xfb, 0x24} },
		{ "abc",
			{0xa4, 0x48, 0x01, 0x7a, 0xaf, 0x21, 0xd8, 0x52, 
			0x5f, 0xc1, 0x0a, 0xe8, 0x7a, 0xa6, 0x72, 0x9d} },
		{ "message digest", 
			{0xd9, 0x13, 0x0a, 0x81, 0x64, 0x54, 0x9f, 0xe8, 
			0x18, 0x87, 0x48, 0x06, 0xe1, 0xc7, 0x01, 0x4b} },
		{ "abcdefghijklmnopqrstuvwxyz", 
			{0xd7, 0x9e, 0x1c, 0x30, 0x8a, 0xa5, 0xbb, 0xcd, 
			0xee, 0xa8, 0xed, 0x63, 0xdf, 0x41, 0x2d, 0xa9} },
		{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
			{0x04, 0x3f, 0x85, 0x82, 0xf2, 0x41, 0xdb, 0x35, 
			0x1c, 0xe6, 0x27, 0xe1, 0x53, 0xe7, 0xf0, 0xe4} },
		{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", 
			{0xe3, 0x3b, 0x4d, 0xdc, 0x9c, 0x38, 0xf2, 0x19, 
			0x9c, 0x3e, 0x7b, 0x16, 0x4f, 0xcc, 0x05, 0x36} },
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < ( Uint32 )( sizeof( tests ) / sizeof( tests[ 0 ] ) ) ; i++ ) {
		init();
		process( tests[ i ].msg );
		done( tmp );
		if ( tmp != Buffer( tests[i].hash, 16 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

void Crypto::MD4::compress(
	const	Uchar *		buf ) {

	Uint32 x[ 16 ];

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		x[ i ] = LOAD32L( buf + 4 * i );
	}

	Uint32 a = state[0];
	Uint32 b = state[1];
	Uint32 c = state[2];
	Uint32 d = state[3];

	/* Round 1 */ 
	FF (a, b, c, d, x[ 0],  3); /* 1 */ 
	FF (d, a, b, c, x[ 1],  7); /* 2 */ 
	FF (c, d, a, b, x[ 2], 11); /* 3 */ 
	FF (b, c, d, a, x[ 3], 19); /* 4 */ 
	FF (a, b, c, d, x[ 4],  3); /* 5 */ 
	FF (d, a, b, c, x[ 5],  7); /* 6 */ 
	FF (c, d, a, b, x[ 6], 11); /* 7 */ 
	FF (b, c, d, a, x[ 7], 19); /* 8 */ 
	FF (a, b, c, d, x[ 8],  3); /* 9 */ 
	FF (d, a, b, c, x[ 9],  7); /* 10 */
	FF (c, d, a, b, x[10], 11); /* 11 */ 
	FF (b, c, d, a, x[11], 19); /* 12 */
	FF (a, b, c, d, x[12],  3); /* 13 */
	FF (d, a, b, c, x[13],  7); /* 14 */ 
	FF (c, d, a, b, x[14], 11); /* 15 */ 
	FF (b, c, d, a, x[15], 19); /* 16 */ 
	
	/* Round 2 */ 
	GG (a, b, c, d, x[ 0],  3); /* 17 */ 
	GG (d, a, b, c, x[ 4],  5); /* 18 */ 
	GG (c, d, a, b, x[ 8],  9); /* 19 */ 
	GG (b, c, d, a, x[12], 13); /* 20 */ 
	GG (a, b, c, d, x[ 1],  3); /* 21 */ 
	GG (d, a, b, c, x[ 5],  5); /* 22 */ 
	GG (c, d, a, b, x[ 9],  9); /* 23 */ 
	GG (b, c, d, a, x[13], 13); /* 24 */ 
	GG (a, b, c, d, x[ 2],  3); /* 25 */ 
	GG (d, a, b, c, x[ 6],  5); /* 26 */ 
	GG (c, d, a, b, x[10],  9); /* 27 */ 
	GG (b, c, d, a, x[14], 13); /* 28 */ 
	GG (a, b, c, d, x[ 3],  3); /* 29 */ 
	GG (d, a, b, c, x[ 7],  5); /* 30 */ 
	GG (c, d, a, b, x[11],  9); /* 31 */ 
	GG (b, c, d, a, x[15], 13); /* 32 */ 
	
	/* Round 3 */
	HH (a, b, c, d, x[ 0],  3); /* 33 */ 
	HH (d, a, b, c, x[ 8],  9); /* 34 */ 
	HH (c, d, a, b, x[ 4], 11); /* 35 */ 
	HH (b, c, d, a, x[12], 15); /* 36 */ 
	HH (a, b, c, d, x[ 2],  3); /* 37 */ 
	HH (d, a, b, c, x[10],  9); /* 38 */ 
	HH (c, d, a, b, x[ 6], 11); /* 39 */ 
	HH (b, c, d, a, x[14], 15); /* 40 */ 
	HH (a, b, c, d, x[ 1],  3); /* 41 */ 
	HH (d, a, b, c, x[ 9],  9); /* 42 */ 
	HH (c, d, a, b, x[ 5], 11); /* 43 */ 
	HH (b, c, d, a, x[13], 15); /* 44 */ 
	HH (a, b, c, d, x[ 3],  3); /* 45 */ 
	HH (d, a, b, c, x[11],  9); /* 46 */ 
	HH (c, d, a, b, x[ 7], 11); /* 47 */ 
	HH (b, c, d, a, x[15], 15); /* 48 */ 
	

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::MD4 > md4Registerer;

// ============================================================================

