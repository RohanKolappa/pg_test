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
//	BFC.Crypto.SHA1.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.SHA1.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SHA1 )

// ============================================================================

const DL::TypeCPtr & Crypto::SHA1::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "ca0ec17d-3a11-473a-83d2-c41313a2dc72" ),
		"BFC.Crypto.SHA1",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"sha1",
				"Hash",
				"SHA1 Hash" ),
			20,
			64 ) );

	return i;

}

// ============================================================================

Crypto::SHA1::SHA1() :

	Hash( getClassType() ) {

}

// ============================================================================

void Crypto::SHA1::init() {

	state[0] = 0x67452301UL;
	state[1] = 0xefcdab89UL;
	state[2] = 0x98badcfeUL;
	state[3] = 0x10325476UL;
	state[4] = 0xc3d2e1f0UL;

	msgBuf.kill();
	msgLen = 0;

}

void Crypto::SHA1::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress();
		msgBuf = msgBuf( 64 );
	}

}

void Crypto::SHA1::done(
		Buffer &	pOutput ) {

	msgLen <<= 3;
	msgBuf += ( Uchar )0x80;

	if ( msgBuf.getLength() > 56 ) {
		msgBuf += Buffer::chars( 128 - msgBuf.getLength(), 0x00 );
	}
	else {
		msgBuf += Buffer::chars( 64 - msgBuf.getLength(), 0x00 );
	}

	STORE64H( msgLen, msgBuf.getVarPtr() + msgBuf.getLength() - 8 );

	while ( msgBuf.getLength() >= 64 ) {
		compress();
		msgBuf = msgBuf( 64 );
	}

	Uint32 hashSize = getDescr()->getHashSize();

	if ( pOutput.getLength() != hashSize ) {
		pOutput.resize( hashSize );
	}

	Uchar *	out = pOutput.getVarPtr();

	for ( Uint32 i = 0 ; i < hashSize ; i += 4 ) {
		STORE32H( state[ i / 4 ], out + i );
	}

}

// ============================================================================

void Crypto::SHA1::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[20];
	} tests[] = {
		{ "abc",
			{ 0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
			0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
			0x9c, 0xd0, 0xd8, 0x9d }
		},
		{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
			{ 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E,
			0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5,
			0xE5, 0x46, 0x70, 0xF1 }
		}
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < (Uint32)(sizeof(tests) / sizeof(tests[0])); i++) {
		init();
		process( tests[i].msg );
		done( tmp );
		if ( tmp != Buffer( tests[i].hash, 20 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

void Crypto::SHA1::compress() {

	const Uchar *	buf = msgBuf.getCstPtr();

	Uint32 a,b,c,d,e,W[80],i;
	Uint32 t;

	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++) {
		W[i] = LOAD32H( buf + (4*i) );
	}

	/* copy state */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	/* expand it */
	for (i = 16; i < 80; i++) {
		W[i] = ROL(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1); 
	}

	/* compress */
	/* round one */
	#define FF0(a,b,c,d,e,i) e = (ROL(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999UL); b = ROL(b, 30);
	#define FF1(a,b,c,d,e,i) e = (ROL(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1UL); b = ROL(b, 30);
	#define FF2(a,b,c,d,e,i) e = (ROL(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdcUL); b = ROL(b, 30);
	#define FF3(a,b,c,d,e,i) e = (ROL(a, 5) + F1(b,c,d) + e + W[i] + 0xca62c1d6UL); b = ROL(b, 30);
 
	for (i = 0; i < 20; ) {
		FF0(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 40; ) {
		FF1(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 60; ) {
		FF2(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 80; ) {
		FF3(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	#undef FF0
	#undef FF1
	#undef FF2
	#undef FF3

	/* store */
	state[0] = state[0] + a;
	state[1] = state[1] + b;
	state[2] = state[2] + c;
	state[3] = state[3] + d;
	state[4] = state[4] + e;

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::SHA1 > sha1Registerer;

// ============================================================================

