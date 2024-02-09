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
//	BFC.Crypto.RC2.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.RC2.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RC2 )

// ============================================================================

const DL::TypeCPtr & Crypto::RC2::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "21677692-0a70-41d7-9b4f-a0e44481193c" ),
		"BFC.Crypto.RC2",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"rc2",
				"Cipher",
				"RC2 Cipher" ),
			8, 128, 8, 16 ) );

	return i;

}

// ============================================================================

Crypto::RC2::RC2() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::RC2::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 8 ) {
		throw InvalidArgument();
	}

	return ( pKeySize > 128 ? 128 : pKeySize );

}

// ============================================================================

void Crypto::RC2::setKey(
	const	Buffer &	pKey ) {

//	const Uchar *	keyPtr = pKey.getCstPtr();
	Uint32		keyLen = pKey.getLength();

	if ( keyLen < getMinKeySize()
	  || keyLen > getMaxKeySize() ) {
		throw InvalidArgument( "Invalid key size!" );
	}

	Buffer	tmp = pKey;

	/* Phase 1: Expand input key to 128 bytes */
	if ( keyLen < 128 ) {
		tmp.resize( 128 );
		for ( Uint32 i = keyLen ; i < 128 ; i++ ) {
				tmp[ i ] = permute[(tmp[i - 1] + tmp[i - keyLen]) & 255];
		}
	}
	
	/* Phase 2 - reduce effective key size to "bits" */
	Uint32	bits	= keyLen << 3;
	Uint32	T8	= (bits+7)>>3;
	Uint32	TM	= (255 >> (Uint32)(7 & -(Int32)bits));
	tmp[128 - T8] = permute[tmp[128 - T8] & TM];
	for ( Int32 i = 127 - T8; i >= 0; i--) {
		tmp[i] = permute[tmp[i + 1] ^ tmp[i + T8]];
	}

	/* Phase 3 - copy to xkey in little-endian order */
	for ( Uint32 i = 0; i < 64; i++) {
		xkey[i] =  (Uint32)tmp[2*i] + ((Uint32)tmp[2*i+1] << 8);
	}		

}

// ============================================================================

void Crypto::RC2::encrypt(
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

	Uint32		x76, x54, x32, x10;

	x76 = ((Uint32)pt[7] << 8) + (Uint32)pt[6];
	x54 = ((Uint32)pt[5] << 8) + (Uint32)pt[4];
	x32 = ((Uint32)pt[3] << 8) + (Uint32)pt[2];
	x10 = ((Uint32)pt[1] << 8) + (Uint32)pt[0];

	for ( Uint32 i = 0 ; i < 16 ; i++ ) {
		x10 = (x10 + (x32 & ~x76) + (x54 & x76) + xkey[4*i+0]) & 0xFFFF;
		x10 = ((x10 << 1) | (x10 >> 15));

		x32 = (x32 + (x54 & ~x10) + (x76 & x10) + xkey[4*i+1]) & 0xFFFF;
		x32 = ((x32 << 2) | (x32 >> 14));

		x54 = (x54 + (x76 & ~x32) + (x10 & x32) + xkey[4*i+2]) & 0xFFFF;
		x54 = ((x54 << 3) | (x54 >> 13));

		x76 = (x76 + (x10 & ~x54) + (x32 & x54) + xkey[4*i+3]) & 0xFFFF;
		x76 = ((x76 << 5) | (x76 >> 11));

		if (i == 4 || i == 10) {
				x10 = (x10 + xkey[x76 & 63]) & 0xFFFF;
				x32 = (x32 + xkey[x10 & 63]) & 0xFFFF;
				x54 = (x54 + xkey[x32 & 63]) & 0xFFFF;
				x76 = (x76 + xkey[x54 & 63]) & 0xFFFF;
		}
	}

	ct[0] = (Uchar)x10;
	ct[1] = (Uchar)(x10 >> 8);
	ct[2] = (Uchar)x32;
	ct[3] = (Uchar)(x32 >> 8);
	ct[4] = (Uchar)x54;
	ct[5] = (Uchar)(x54 >> 8);
	ct[6] = (Uchar)x76;
	ct[7] = (Uchar)(x76 >> 8);
 
}

void Crypto::RC2::decrypt(
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

	Uint32		x76, x54, x32, x10;

	x76 = ((Uint32)ct[7] << 8) + (Uint32)ct[6];
	x54 = ((Uint32)ct[5] << 8) + (Uint32)ct[4];
	x32 = ((Uint32)ct[3] << 8) + (Uint32)ct[2];
	x10 = ((Uint32)ct[1] << 8) + (Uint32)ct[0];

	for ( Int32 i = 15; i >= 0; i--) {
		if (i == 4 || i == 10) {
				x76 = (x76 - xkey[x54 & 63]) & 0xFFFF;
				x54 = (x54 - xkey[x32 & 63]) & 0xFFFF;
				x32 = (x32 - xkey[x10 & 63]) & 0xFFFF;
				x10 = (x10 - xkey[x76 & 63]) & 0xFFFF;
		}

		x76 = ((x76 << 11) | (x76 >> 5));
		x76 = (x76 - ((x10 & ~x54) + (x32 & x54) + xkey[4*i+3])) & 0xFFFF;

		x54 = ((x54 << 13) | (x54 >> 3));
		x54 = (x54 - ((x76 & ~x32) + (x10 & x32) + xkey[4*i+2])) & 0xFFFF;

		x32 = ((x32 << 14) | (x32 >> 2));
		x32 = (x32 - ((x54 & ~x10) + (x76 & x10) + xkey[4*i+1])) & 0xFFFF;

		x10 = ((x10 << 15) | (x10 >> 1));
		x10 = (x10 - ((x32 & ~x76) + (x54 & x76) + xkey[4*i+0])) & 0xFFFF;
	}

	pt[0] = (Uchar)x10;
	pt[1] = (Uchar)(x10 >> 8);
	pt[2] = (Uchar)x32;
	pt[3] = (Uchar)(x32 >> 8);
	pt[4] = (Uchar)x54;
	pt[5] = (Uchar)(x54 >> 8);
	pt[6] = (Uchar)x76;
	pt[7] = (Uchar)(x76 >> 8);

}

// ============================================================================

void Crypto::RC2::test() {

	static const struct {
		Uint32	keyLen;
		Uchar	key[16], pt[8], ct[8];
	} tests[] = {
	{
		8,
		{ 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
		{ 0x30, 0x64, 0x9e, 0xdf, 0x9b, 0xe7, 0xd2, 0xc2 }
	},
	{
		16,
		{ 0x88, 0xbc, 0xa9, 0x0e, 0x90, 0x87, 0x5a, 0x7f,
			0x0f, 0x79, 0xc3, 0x84, 0x62, 0x7b, 0xaf, 0xb2 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x22, 0x69, 0x55, 0x2a, 0xb0, 0xf8, 0x5c, 0xa6 }
	}
	};

	for ( Uint32 x = 0 ; x < ( Uint32 )(sizeof(tests) / sizeof(tests[0])); x++) {

		Buffer tmp[2];

		Buffer	key( tests[x].key, tests[ x ].keyLen );
		Buffer	enc( tests[x].ct, 8 );
		Buffer	dec( tests[x].pt, 8 );

		setKey( key );

		encrypt( dec, tmp[ 0 ] );
		decrypt( tmp[ 0 ], tmp[ 1 ]);

		if ( tmp[ 0 ] != enc
		  || tmp[ 1 ] != dec ) {
			throw InternalError( "Test1" );
		}

		tmp[ 1 ] = tmp[ 0 ] = Buffer::chars( 8, 0x00 );

		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			encrypt( tmp[ 0 ], tmp[ 0 ] );
		for ( Uint32 y = 0 ; y < 1000 ; y++ )
			decrypt( tmp[ 0 ], tmp[ 0 ] );

		if ( tmp[ 0 ] != tmp[ 1 ] ) {
			throw InternalError( "Test2" );
		}
		
	}

}

// ============================================================================

const Uchar Crypto::RC2::permute[256] = {
	217,120,249,196, 25,221,181,237, 40,233,253,121, 74,160,216,157,
	198,126, 55,131, 43,118, 83,142, 98, 76,100,136, 68,139,251,162,
	23,154, 89,245,135,179, 79, 19, 97, 69,109,141,  9,129,125, 50,
	189,143, 64,235,134,183,123, 11,240,149, 33, 34, 92,107, 78,130,
	84,214,101,147,206, 96,178, 28,115, 86,192, 20,167,140,241,220,
	18,117,202, 31, 59,190,228,209, 66, 61,212, 48,163, 60,182, 38,
	111,191, 14,218, 70,105,  7, 87, 39,242, 29,155,188,148, 67,  3,
	248, 17,199,246,144,239, 62,231,  6,195,213, 47,200,102, 30,215,
	8,232,234,222,128, 82,238,247,132,170,114,172, 53, 77,106, 42,
	150, 26,210,113, 90, 21, 73,116, 75,159,208, 94,  4, 24,164,236,
	194,224, 65,110, 15, 81,203,204, 36,145,175, 80,161,244,112, 57,
	153,124, 58,133, 35,184,180,122,252,  2, 54, 91, 37, 85,151, 49,
	45, 93,250,152,227,138,146,174,  5,223, 41, 16,103,108,186,201,
	211,  0,230,207,225,158,168, 44, 99, 22,  1, 63, 88,226,137,169,
	13, 56, 52, 27,171, 51,255,176,187, 72, 12, 95,185,177,205, 46,
	197,243,219, 71,229,165,156,119, 10,166, 32,104,254,127,193,173
};

// ============================================================================

static Crypto::CipherRegisterer< Crypto::RC2 > rc2Registerer;

// ============================================================================

