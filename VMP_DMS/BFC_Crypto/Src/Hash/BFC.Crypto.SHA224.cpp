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
//	BFC.Crypto.SHA224.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.SHA224.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SHA224 )

// ============================================================================

const DL::TypeCPtr & Crypto::SHA224::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "84ecab96-1a37-4236-9611-f8d81f0109df" ),
		"BFC.Crypto.SHA224",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"sha224",
				"Hash",
				"SHA224 Hash" ),
			28,
			64 ) );

	return i;

}

// ============================================================================

Crypto::SHA224::SHA224() :

	SHA256( getClassType() ) {

}

// ============================================================================

void Crypto::SHA224::init() {

	msgBuf.kill();
	msgLen = 0;

	state[0] = 0xc1059ed8UL;
	state[1] = 0x367cd507UL;
	state[2] = 0x3070dd17UL;
	state[3] = 0xf70e5939UL;
	state[4] = 0xffc00b31UL;
	state[5] = 0x68581511UL;
	state[6] = 0x64f98fa7UL;
	state[7] = 0xbefa4fa4UL;

}

// ============================================================================

void Crypto::SHA224::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[32];
	} tests[] = {
		{ "abc",
		{ 0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8,
		0x22, 0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2,
		0x55, 0xb3, 0x2a, 0xad, 0xbc, 0xe4, 0xbd,
		0xa0, 0xb3, 0xf7, 0xe3, 0x6c, 0x9d, 0xa7 }
		},
		{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{ 0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76,
		0xcc, 0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89,
		0x01, 0x50, 0xb0, 0xc6, 0x45, 0x5c, 0xb4,
		0xf5, 0x8b, 0x19, 0x52, 0x52, 0x25, 0x25 }
		},
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < (Uint32)(sizeof(tests) / sizeof(tests[0])); i++) {
		init();
		process( tests[i].msg );
		done( tmp );
		if ( tmp != Buffer( tests[i].hash, 28 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::SHA224 > sha224Registerer;

// ============================================================================

