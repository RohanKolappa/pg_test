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
//	BFC.Crypto.XTEA.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Crypto.CipherRegisterer.h"
#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.XTEA.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, XTEA )

// ============================================================================

const DL::TypeCPtr & Crypto::XTEA::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "60217f3c-e924-4212-a8af-4e30a4126ee1" ),
		"BFC.Crypto.XTEA",
		Crypto::Cipher::getClassType(),
		new CipherDescriptor(
			DL::Descr(
				"xtea",
				"Cipher",
				"XTEA Cipher" ),
			16, 16, 8, 32 ) );

	return i;

}

// ============================================================================

Crypto::XTEA::XTEA() :

	Cipher( getClassType() ) {

}

// ============================================================================

Uint32 Crypto::XTEA::getKeySize(
	const	Uint32		pKeySize ) const {

	if ( pKeySize < 16 ) {
		throw InvalidArgument();
	}

	return 16;

}

// ============================================================================

void Crypto::XTEA::setKey(
	const	Buffer &	pKey ) {

	if ( pKey.getLength() != 16 ) {
		throw InvalidArgument( "Key length must be 16!" );
	}

	const Uchar *	key = pKey.getCstPtr();

	Uint32	K[ 4 ];
	
	K[ 0 ] = LOAD32L( key      );
	K[ 1 ] = LOAD32L( key +  4 );
	K[ 2 ] = LOAD32L( key +  8 );
	K[ 3 ] = LOAD32L( key + 12 );
	
	Uint32	sum = 0;

	for ( Uint32 x = 0 ; x < 32 ; x++ ) {
		 A[x] = (sum + K[sum&3]) & 0xFFFFFFFFUL;
		 sum = (sum + 0x9E3779B9UL) & 0xFFFFFFFFUL;
		 B[x] = (sum + K[(sum>>11)&3]) & 0xFFFFFFFFUL;
	}
	
}

// ============================================================================

void Crypto::XTEA::encrypt(
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

	Uint32	y = LOAD32L( pt     );
	Uint32	z = LOAD32L( pt + 4 );

	const Uint32 *	b = &B[ 0 ];
	const Uint32 *	a = &A[ 0 ];

	for ( Uint32 r = 0 ; r < 32 ; r += 4 ) {
		 y += ((((z<<4)^(z>>5)) + z) ^ *a++);
		 z += ((((y<<4)^(y>>5)) + y) ^ *b++);
		 y += ((((z<<4)^(z>>5)) + z) ^ *a++);
		 z += ((((y<<4)^(y>>5)) + y) ^ *b++);
		 y += ((((z<<4)^(z>>5)) + z) ^ *a++);
		 z += ((((y<<4)^(y>>5)) + y) ^ *b++);
		 y += ((((z<<4)^(z>>5)) + z) ^ *a++);
		 z += ((((y<<4)^(y>>5)) + y) ^ *b++);
	}

	STORE32L( y, ct     );
	STORE32L( z, ct + 4 );

}

void Crypto::XTEA::decrypt(
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

	Uint32	y = LOAD32L( ct     );
	Uint32	z = LOAD32L( ct + 4 );

	const Uint32 *	b = &B[ 31 ];
	const Uint32 *	a = &A[ 31 ];

	for ( Uint32 r = 32 ; r ; r -= 4 ) {
		 z -= ((((y<<4)^(y>>5)) + y) ^ *b--);
		 y -= ((((z<<4)^(z>>5)) + z) ^ *a--);
		 z -= ((((y<<4)^(y>>5)) + y) ^ *b--);
		 y -= ((((z<<4)^(z>>5)) + z) ^ *a--);
		 z -= ((((y<<4)^(y>>5)) + y) ^ *b--);
		 y -= ((((z<<4)^(z>>5)) + z) ^ *a--);
		 z -= ((((y<<4)^(y>>5)) + y) ^ *b--);
		 y -= ((((z<<4)^(z>>5)) + z) ^ *a--);
	}

	STORE32L( y, pt     );
	STORE32L( z, pt + 4 );

}

// ============================================================================

void Crypto::XTEA::test() {

	static const unsigned char key[16] = 
		{ 0x78, 0x56, 0x34, 0x12, 0xf0, 0xcd, 0xcb, 0x9a,
		  0x48, 0x37, 0x26, 0x15, 0xc0, 0xbf, 0xae, 0x9d };
	static const unsigned char pt[8] = 
		{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
	static const unsigned char ct[8] = 
		{ 0x75, 0xd7, 0xc5, 0xbf, 0xcf, 0x58, 0xc9, 0x3f };

	Buffer	keybuf( key, 16 );
	Buffer	ptbuf( pt, 8 );
	Buffer	ctbuf( ct, 8 );

	setKey( keybuf );

	Buffer	ctnew = Cipher::encrypt( ptbuf );
	Buffer	ptnew = Cipher::decrypt( ctnew );

	if ( ctnew != ctbuf || ptnew != ptbuf ) { 
		throw InternalError( "Test1" );
	}

	ptnew = ptbuf = Buffer::chars( 8, 0x00 );

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		ptbuf = Cipher::encrypt( ptbuf );
	}

	for ( Uint32 y = 0 ; y < 1000 ; y++ ) {
		ptbuf = Cipher::decrypt( ptbuf );
	}

	if ( ptbuf != ptnew ) {
		throw InternalError( "Test2" );
	}

//	msgDbg( "Test OK :-)" );

}

// ============================================================================

static Crypto::CipherRegisterer< Crypto::XTEA > xteaRegisterer;

// ============================================================================

