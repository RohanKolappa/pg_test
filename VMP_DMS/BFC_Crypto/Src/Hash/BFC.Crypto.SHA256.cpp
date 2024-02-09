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
//	BFC.Crypto.SHA256.cpp
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
#include "BFC.Crypto.SHA256.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SHA256 )

// ============================================================================

const DL::TypeCPtr & Crypto::SHA256::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "629d2a8b-72d9-47b7-8a86-1897f1ceb8a8" ),
		"BFC.Crypto.SHA256",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"sha256",
				"Hash",
				"SHA256 Hash" ),
			32,
			64 ) );

	return i;

}

// ============================================================================

Crypto::SHA256::SHA256() :

	Hash( getClassType() ) {

}

Crypto::SHA256::SHA256(
	const	DL::TypeCPtr &	pType ) :

	Hash( pType ) {

}

// ============================================================================

void Crypto::SHA256::init() {

	msgBuf.kill();
	msgLen = 0;

	state[0] = 0x6A09E667UL;
	state[1] = 0xBB67AE85UL;
	state[2] = 0x3C6EF372UL;
	state[3] = 0xA54FF53AUL;
	state[4] = 0x510E527FUL;
	state[5] = 0x9B05688CUL;
	state[6] = 0x1F83D9ABUL;
	state[7] = 0x5BE0CD19UL;

}

void Crypto::SHA256::process(
	const	Buffer &	pData ) {

	msgBuf += pData;
	msgLen += pData.getLength();

	while ( msgBuf.getLength() >= 64 ) {
		compress();
		msgBuf = msgBuf( 64 );
	}

}

void Crypto::SHA256::done(
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

void Crypto::SHA256::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[32];
	} tests[] = {
		{ "abc",
		{ 0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
		0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
		0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
		0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad }
		},
		{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{ 0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
		0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
		0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
		0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1 }
		},
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < (Uint32)(sizeof(tests) / sizeof(tests[0])); i++) {
		init();
		process( tests[i].msg );
		done( tmp );
		if ( tmp != Buffer( tests[i].hash, 32 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

#define Ch(x,y,z)		(z ^ (x & (y ^ z)))
#define Maj(x,y,z)		(((x | y) & z) | (x & y))
#define S(x, n)			ROR((x),(n))
#define R(x, n)			(((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)		(S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)		(S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)		(S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)		(S(x, 17) ^ S(x, 19) ^ R(x, 10))

void Crypto::SHA256::compress() {

	const Uchar *	buf = msgBuf.getCstPtr();

	Uint32 S[8], W[64], t0, t1;
	Uint32 t;

	/* copy state into S */
	for ( Uint32 i = 0; i < 8; i++) {
		S[i] = state[i];
	}

	/* copy the state into 512-bits into W[0..15] */
	for ( Uint32 i = 0; i < 16; i++) {
		W[i] = LOAD32H( buf + (4*i));
	}

	/* fill W[16..63] */
	for ( Uint32 i = 16; i < 64; i++) {
		W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
	}

	/* Compress */
#define RND(a,b,c,d,e,f,g,h,i)				\
	t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];	\
	t1 = Sigma0(a) + Maj(a, b, c);			\
	d += t0;					\
	h  = t0 + t1;

	for ( Uint32 i = 0; i < 64; ++i) {
		RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i);
		t = S[7]; S[7] = S[6]; S[6] = S[5]; S[5] = S[4];
		S[4] = S[3]; S[3] = S[2]; S[2] = S[1]; S[1] = S[0]; S[0] = t;
	}
	/* feedback */
	for ( Uint32 i = 0; i < 8; i++) {
		state[i] += S[i];
	}

}

#undef RND

// ============================================================================

const Uint32 Crypto::SHA256::K[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
	0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
	0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
	0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
	0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
	0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
	0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
	0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
	0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
	0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

// ============================================================================

static Crypto::HashRegisterer< Crypto::SHA256 > sha256Registerer;

// ============================================================================

