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
//	BFC.Crypto.SHA384.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.SHA384.h"
#include "BFC.Crypto.SHA512.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, SHA384 )

// ============================================================================

const DL::TypeCPtr & Crypto::SHA384::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "d1f7ccfe-b6c3-486e-92b7-bd5341a534e9" ),
		"BFC.Crypto.SHA384",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"sha384",
				"Hash",
				"SHA384 Hash" ),
			48,
			128 ) );

	return i;

}

// ============================================================================

Crypto::SHA384::SHA384() :

	SHA512( getClassType() ) {

}

// ============================================================================

void Crypto::SHA384::init() {

	msgBuf.kill();
	msgLen = 0;

	state[0] = CONST64(0xcbbb9d5dc1059ed8);
	state[1] = CONST64(0x629a292a367cd507);
	state[2] = CONST64(0x9159015a3070dd17);
	state[3] = CONST64(0x152fecd8f70e5939);
	state[4] = CONST64(0x67332667ffc00b31);
	state[5] = CONST64(0x8eb44a8768581511);
	state[6] = CONST64(0xdb0c2e0d64f98fa7);
	state[7] = CONST64(0x47b5481dbefa4fa4);

}

// ============================================================================

void Crypto::SHA384::test() {

	static const struct {
		const	Char *	msg;
			Uchar	hash[48];
	} tests[] = {
	{ "abc",
		{ 0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
		0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
		0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
		0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
		0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
		0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7 }
	},
	{ "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		{ 0x09, 0x33, 0x0c, 0x33, 0xf7, 0x11, 0x47, 0xe8,
		0x3d, 0x19, 0x2f, 0xc7, 0x82, 0xcd, 0x1b, 0x47,
		0x53, 0x11, 0x1b, 0x17, 0x3b, 0x3b, 0x05, 0xd2,
		0x2f, 0xa0, 0x80, 0x86, 0xe3, 0xb0, 0xf7, 0x12,
		0xfc, 0xc7, 0xc7, 0x1a, 0x55, 0x7e, 0x2d, 0xb9,
		0x66, 0xc3, 0xe9, 0xfa, 0x91, 0x74, 0x60, 0x39 }
	},
	};

	Buffer tmp;

	for ( Uint32 i = 0 ; i < ( Uint32 )( sizeof( tests ) / sizeof( tests[ 0 ] ) ) ; i++ ) {
		init();
		process( tests[i].msg );
		done( tmp );
		if ( tmp != Buffer( tests[i].hash, 48 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

static Crypto::HashRegisterer< Crypto::SHA384 > sha384Registerer;

// ============================================================================

