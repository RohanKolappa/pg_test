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
//	BFC.Crypto.DES.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.DES.h"
#include "BFC.Crypto.DESCommon.h"
#include "BFC.Crypto.Functions.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, DES )

// ============================================================================

const DL::TypeCPtr & Crypto::DES::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b8f4dfe4-011b-42b1-920b-835fe82affb2" ),
		"BFC.Crypto.DES",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"des",
				"Cipher",
				"DES Cipher" ),
			8, 8, 8, 16 ) );

	return i;

}

// ============================================================================

Crypto::DES::DES() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::DES::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 8 ) {
		throw InvalidArgument();
	}

	return 8;

}

// ============================================================================

void Crypto::DES::setKey(
	const	Buffer &	pKey ) {

	if ( pKey.getLength() != 8 ) {
		throw InvalidArgument( "Invalid keysize!" );
	}

	const Uchar * ptr = pKey.getCstPtr();

	DESCommon::deskey( ptr, DESCommon::EN0, ek );
	DESCommon::deskey( ptr, DESCommon::DE1, dk );

}

// ============================================================================

void Crypto::DES::encrypt(
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

	Uint32		work[ 2 ];

	work[ 0 ] = LOAD32H( pt     );
	work[ 1 ] = LOAD32H( pt + 4 );

	DESCommon::desfunc( work, ek );

	STORE32H( work[ 0 ], ct     );
	STORE32H( work[ 1 ], ct + 4 );

}

void Crypto::DES::decrypt(
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

	Uint32		work[ 2 ];

	work[ 0 ] = LOAD32H( ct     );
	work[ 1 ] = LOAD32H( ct + 4 );

	DESCommon::desfunc( work, dk );

	STORE32H( work[ 0 ], pt     );
	STORE32H( work[ 1 ], pt + 4 );  

}

// ============================================================================

void Crypto::DES::test() {

	static const struct des_test_case {
		Uint32 num, mode; /* mode 1 = encrypt */
		Uchar key[8], txt[8], out[8];
	} cases[] = {
		{ 1, 1,	{ 0x10, 0x31, 0x6E, 0x02, 0x8C, 0x8F, 0x3B, 0x4A },
						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x82, 0xDC, 0xBA, 0xFB, 0xDE, 0xAB, 0x66, 0x02 } },
		{ 2, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x95, 0xF8, 0xA5, 0xE5, 0xDD, 0x31, 0xD9, 0x00 },
						{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 3, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0xDD, 0x7F, 0x12, 0x1C, 0xA5, 0x01, 0x56, 0x19 },
						{ 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 4, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x2E, 0x86, 0x53, 0x10, 0x4F, 0x38, 0x34, 0xEA },
						{ 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 5, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x4B, 0xD3, 0x88, 0xFF, 0x6C, 0xD8, 0x1D, 0x4F },
						{ 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 6, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x20, 0xB9, 0xE7, 0x67, 0xB2, 0xFB, 0x14, 0x56 },
						{ 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 7, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x55, 0x57, 0x93, 0x80, 0xD7, 0x71, 0x38, 0xEF },
						{ 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 8, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x6C, 0xC5, 0xDE, 0xFA, 0xAF, 0x04, 0x51, 0x2F },
						{ 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 9, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x0D, 0x9F, 0x27, 0x9B, 0xA5, 0xD8, 0x72, 0x60 }, 
						{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{10, 1,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0xD9, 0x03, 0x1B, 0x02, 0x71, 0xBD, 0x5A, 0x0A },
						{ 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },

		{ 1, 0,	{ 0x10, 0x31, 0x6E, 0x02, 0x8C, 0x8F, 0x3B, 0x4A },
						{ 0x82, 0xDC, 0xBA, 0xFB, 0xDE, 0xAB, 0x66, 0x02 },
						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ 2, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x95, 0xF8, 0xA5, 0xE5, 0xDD, 0x31, 0xD9, 0x00 } },
		{ 3, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0xDD, 0x7F, 0x12, 0x1C, 0xA5, 0x01, 0x56, 0x19 } },
		{ 4, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x2E, 0x86, 0x53, 0x10, 0x4F, 0x38, 0x34, 0xEA } },
		{ 5, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x4B, 0xD3, 0x88, 0xFF, 0x6C, 0xD8, 0x1D, 0x4F } },
		{ 6, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x20, 0xB9, 0xE7, 0x67, 0xB2, 0xFB, 0x14, 0x56 } },
		{ 7, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x55, 0x57, 0x93, 0x80, 0xD7, 0x71, 0x38, 0xEF } },
		{ 8, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x6C, 0xC5, 0xDE, 0xFA, 0xAF, 0x04, 0x51, 0x2F } },
		{ 9, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0x0D, 0x9F, 0x27, 0x9B, 0xA5, 0xD8, 0x72, 0x60 } }, 
		{10, 0,	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
						{ 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
						{ 0xD9, 0x03, 0x1B, 0x02, 0x71, 0xBD, 0x5A, 0x0A } }
	};

	Buffer tmp, tmp2;

	for ( Uint32 i = 0 ; i < ( Uint32 )(sizeof(cases)/sizeof(cases[0])) ; i++ ) {
		setKey( Buffer( cases[i].key, 8 ) );
		if (cases[i].mode != 0) { 
			tmp = Cipher::encrypt( Buffer( cases[i].txt, 8 ) );
		} else {
			tmp = Cipher::decrypt( Buffer( cases[i].txt, 8 ) );
		}
		if ( Buffer( cases[i].out, 8 ) != tmp ) {
			throw InternalError( "Test1" );
		}
		tmp2 = tmp = Buffer::chars( 8, 0x00 );
		for ( Uint32 y = 0 ; y < 1000 ; y++) tmp = Cipher::encrypt( tmp );
		for ( Uint32 y = 0 ; y < 1000 ; y++) tmp = Cipher::decrypt( tmp );
		if ( tmp != tmp2 ) {
			throw InternalError( "Test2" );
		}
	}

}

// ============================================================================

void Crypto::DES::setOddParity(
		Buffer &	pKey ) {

	static const Uchar tbl[ 256 ] = {
		  1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
		 16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
		 32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
		 49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
		 64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
		 81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
		 97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
		112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
		128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
		145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
		161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
		176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
		193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
		208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
		224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
		241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254
	};

	for ( Uint32 i = 0 ; i < pKey.getLength() ; i++ ) {
		pKey[ i ] = tbl[ pKey[ i ] ];
	}

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::DES > desRegisterer;

// ============================================================================

