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
//	BFC.Crypto.MD5.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.MD5.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, MD5 )

// ============================================================================

const DL::TypeCPtr & Crypto::MD5::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "5d605f31-f9f8-498e-a07f-d69e02c162d4" ),
		"BFC.Crypto.MD5",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"md5",
				"Hash",
				"MD5 Hash" ),
			16,
			64 ) );

	return i;

}

// ============================================================================

Crypto::MD5::MD5() :

	Hash( getClassType() ) {

	init();

}

// ============================================================================

void Crypto::MD5::init() {

	msgBuf.kill();
	msgLen = 0;

	state[ 0 ] = 0x67452301UL;
	state[ 1 ] = 0xefcdab89UL;
	state[ 2 ] = 0x98badcfeUL;
	state[ 3 ] = 0x10325476UL;

}

void Crypto::MD5::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress( msgBuf.getCstPtr() );
		msgBuf = msgBuf( 64 );
	}

}

void Crypto::MD5::done(
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

void Crypto::MD5::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[ 16 ];
	} tests[] = {
	{ "",
		{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
		0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } },
	{ "a",
		{0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
		0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 } },
	{ "abc",
		{ 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
		0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 } },
	{ "message digest",
		{ 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
		0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 } },
	{ "abcdefghijklmnopqrstuvwxyz",
		{ 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
		0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b } },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		{ 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
		0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f } },
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		{ 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55,
		0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a } }
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

void Crypto::MD5::compress(
	const	Uchar *		buf ) {

	Uint32 W[ 16 ];

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		W[ i ] = LOAD32L( buf + 4 * i );
	}

	Uint32 a = state[0];
	Uint32 b = state[1];
	Uint32 c = state[2];
	Uint32 d = state[3];

	FF(a,b,c,d,W[0],7,0xd76aa478UL);
	FF(d,a,b,c,W[1],12,0xe8c7b756UL);
	FF(c,d,a,b,W[2],17,0x242070dbUL);
	FF(b,c,d,a,W[3],22,0xc1bdceeeUL);
	FF(a,b,c,d,W[4],7,0xf57c0fafUL);
	FF(d,a,b,c,W[5],12,0x4787c62aUL);
	FF(c,d,a,b,W[6],17,0xa8304613UL);
	FF(b,c,d,a,W[7],22,0xfd469501UL);
	FF(a,b,c,d,W[8],7,0x698098d8UL);
	FF(d,a,b,c,W[9],12,0x8b44f7afUL);
	FF(c,d,a,b,W[10],17,0xffff5bb1UL);
	FF(b,c,d,a,W[11],22,0x895cd7beUL);
	FF(a,b,c,d,W[12],7,0x6b901122UL);
	FF(d,a,b,c,W[13],12,0xfd987193UL);
	FF(c,d,a,b,W[14],17,0xa679438eUL);
	FF(b,c,d,a,W[15],22,0x49b40821UL);
	GG(a,b,c,d,W[1],5,0xf61e2562UL);
	GG(d,a,b,c,W[6],9,0xc040b340UL);
	GG(c,d,a,b,W[11],14,0x265e5a51UL);
	GG(b,c,d,a,W[0],20,0xe9b6c7aaUL);
	GG(a,b,c,d,W[5],5,0xd62f105dUL);
	GG(d,a,b,c,W[10],9,0x02441453UL);
	GG(c,d,a,b,W[15],14,0xd8a1e681UL);
	GG(b,c,d,a,W[4],20,0xe7d3fbc8UL);
	GG(a,b,c,d,W[9],5,0x21e1cde6UL);
	GG(d,a,b,c,W[14],9,0xc33707d6UL);
	GG(c,d,a,b,W[3],14,0xf4d50d87UL);
	GG(b,c,d,a,W[8],20,0x455a14edUL);
	GG(a,b,c,d,W[13],5,0xa9e3e905UL);
	GG(d,a,b,c,W[2],9,0xfcefa3f8UL);
	GG(c,d,a,b,W[7],14,0x676f02d9UL);
	GG(b,c,d,a,W[12],20,0x8d2a4c8aUL);
	HH(a,b,c,d,W[5],4,0xfffa3942UL);
	HH(d,a,b,c,W[8],11,0x8771f681UL);
	HH(c,d,a,b,W[11],16,0x6d9d6122UL);
	HH(b,c,d,a,W[14],23,0xfde5380cUL);
	HH(a,b,c,d,W[1],4,0xa4beea44UL);
	HH(d,a,b,c,W[4],11,0x4bdecfa9UL);
	HH(c,d,a,b,W[7],16,0xf6bb4b60UL);
	HH(b,c,d,a,W[10],23,0xbebfbc70UL);
	HH(a,b,c,d,W[13],4,0x289b7ec6UL);
	HH(d,a,b,c,W[0],11,0xeaa127faUL);
	HH(c,d,a,b,W[3],16,0xd4ef3085UL);
	HH(b,c,d,a,W[6],23,0x04881d05UL);
	HH(a,b,c,d,W[9],4,0xd9d4d039UL);
	HH(d,a,b,c,W[12],11,0xe6db99e5UL);
	HH(c,d,a,b,W[15],16,0x1fa27cf8UL);
	HH(b,c,d,a,W[2],23,0xc4ac5665UL);
	II(a,b,c,d,W[0],6,0xf4292244UL);
	II(d,a,b,c,W[7],10,0x432aff97UL);
	II(c,d,a,b,W[14],15,0xab9423a7UL);
	II(b,c,d,a,W[5],21,0xfc93a039UL);
	II(a,b,c,d,W[12],6,0x655b59c3UL);
	II(d,a,b,c,W[3],10,0x8f0ccc92UL);
	II(c,d,a,b,W[10],15,0xffeff47dUL);
	II(b,c,d,a,W[1],21,0x85845dd1UL);
	II(a,b,c,d,W[8],6,0x6fa87e4fUL);
	II(d,a,b,c,W[15],10,0xfe2ce6e0UL);
	II(c,d,a,b,W[6],15,0xa3014314UL);
	II(b,c,d,a,W[13],21,0x4e0811a1UL);
	II(a,b,c,d,W[4],6,0xf7537e82UL);
	II(d,a,b,c,W[11],10,0xbd3af235UL);
	II(c,d,a,b,W[2],15,0x2ad7d2bbUL);
	II(b,c,d,a,W[9],21,0xeb86d391UL);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::MD5 > md5Registerer;

// ============================================================================

