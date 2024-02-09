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
//	BFC.Crypto.MD2.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.HashRegisterer.h"
#include "BFC.Crypto.MD2.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, MD2 )

// ============================================================================

const DL::TypeCPtr & Crypto::MD2::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7068782a-d697-41e7-a0ad-c6e8e3113643" ),
		"BFC.Crypto.MD2",
		Crypto::Hash::getClassType(),
		new HashDescriptor(
			DL::Descr(
				"md2",
				"Hash",
				"MD2 Hash" ),
			16,
			16 ) );

	return i;

}

// ============================================================================

Crypto::MD2::MD2() :

	Hash( getClassType() ) {

	init();

}

// ============================================================================

void Crypto::MD2::init() {

	tmp.kill();
	Cbuf = Buffer::chars( 16, 0x00 ); C = Cbuf.getVarPtr();
	Xbuf = Buffer::chars( 48, 0x00 ); X = Xbuf.getVarPtr();

}

void Crypto::MD2::process(
	const	Buffer &	pData ) {

	tmp += pData;

	while ( tmp.getLength() >= 16 ) {
		compress();
		updateChksum();
		tmp = tmp( 16 );
	}

}

void Crypto::MD2::done(
		Buffer &	pOutput ) {

	Uint32 padLen = 16 - tmp.getLength();

	if ( padLen ) {
		tmp += Buffer::chars( padLen, padLen );
	}

	compress();
	updateChksum();

	tmp = Cbuf;

	compress();

	pOutput = Xbuf( 0, 16 );

}

void Crypto::MD2::test() {

	static const struct {
		const	Char *	msg;
			Uchar	md[ 16 ];
	} tests[] = {
		{ "",
		{0x83,0x50,0xe5,0xa3,0xe2,0x4c,0x15,0x3d,
			0xf2,0x27,0x5c,0x9f,0x80,0x69,0x27,0x73
		}
		},
		{ "a",
		{0x32,0xec,0x01,0xec,0x4a,0x6d,0xac,0x72,
			0xc0,0xab,0x96,0xfb,0x34,0xc0,0xb5,0xd1
		}
		},
		{ "message digest",
		{0xab,0x4f,0x49,0x6b,0xfb,0x2a,0x53,0x0b,
			0x21,0x9f,0xf3,0x30,0x31,0xfe,0x06,0xb0
		}
		},
		{ "abcdefghijklmnopqrstuvwxyz",
		{0x4e,0x8d,0xdf,0xf3,0x65,0x02,0x92,0xab,
			0x5a,0x41,0x08,0xc3,0xaa,0x47,0x94,0x0b
		}
		},
		{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		{0xda,0x33,0xde,0xf2,0xa4,0x2d,0xf1,0x39,
			0x75,0x35,0x28,0x46,0xc3,0x03,0x38,0xcd
		}
		},
		{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		{0xd5,0x97,0x6f,0x79,0xd8,0x3d,0x3a,0x0d,
			0xc9,0x80,0x6c,0x3c,0x66,0xf3,0xef,0xd8
		}
		}
	};

	Buffer buf;

	for ( Uint32 i = 0; i < ( Uint32 )(sizeof(tests) / sizeof(tests[0])); i++) {
		Buffer msg( tests[ i ].msg );
//		msgDbg( "Test: len == " + Buffer( msg.getLength() ) );
		init();
		process( msg );
		done( buf );
		if ( buf != Buffer( tests[ i ].md, 16 ) ) {
			throw InternalError();
		}
	}

//	msgDbg( "Test OK :-)" );

}

// ============================================================================

void Crypto::MD2::compress() {

	const Uchar * b = tmp.getCstPtr();

	for ( Uint32 j = 0 ; j < 16 ; j++ ) {
		X[ 16 + j ] = b[ j ];
		X[ 32 + j ] = X[ j ] ^ X[ 16 + j ];
	}

	Uchar t = 0;

	for ( Uint32 j = 0 ; j < 18 ; j++ ) {
		for ( Uint32 k = 0 ; k < 48 ; k++ ) {
			t = ( X[ k ] ^= PI_SUBST[ t & 255 ] );
		}
		t += ( Uchar )j;
	}

}

void Crypto::MD2::updateChksum() {

	const Uchar * b = tmp.getCstPtr();

	Uchar L = C[ 15 ];

	for ( Uint32 j = 0 ; j < 16 ; j++ ) {
		L = ( C[ j ] ^= PI_SUBST[ b[ j ] ^ L ] );
	}

}

// ============================================================================

const Uchar Crypto::MD2::PI_SUBST[ 256 ] = {
	41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
	19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
	76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
	138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
	245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
	148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
	39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
	181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
	150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
	112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
	96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
	85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
	234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
	129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
	8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
	203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
	166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
	31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

// ============================================================================

static Crypto::HashRegisterer< Crypto::MD2 > md2Registerer;

// ============================================================================

