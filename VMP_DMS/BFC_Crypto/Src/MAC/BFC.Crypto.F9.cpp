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
//	BFC.Crypto.F9.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.CipherRepository.h"
#include "BFC.Crypto.F9.h"
#include "BFC.Crypto.MACRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, F9 )

// ============================================================================

const DL::TypeCPtr & Crypto::F9::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "0682e272-cf24-4c83-b39e-039550e704bc" ),
		"BFC.Crypto.F9",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"f9",
			"F9",
			"F9 MAC" ) );

	return i;

}

// ============================================================================

Crypto::F9::F9() :

	MAC( getClassType() ) {

}

// ============================================================================

void Crypto::F9::init(
		CipherPtr	pCipher,
	const	Buffer &	pKey ) {

	cipher	= pCipher;

	cipher->setKey( pKey );

	blkSze	= cipher->getBlockSize();
	key2	= pKey ^ Buffer::chars( pKey.getLength(), 0xAA );
	ACC	= Buffer::chars( blkSze, 0x00 );
	IV	= Buffer::chars( blkSze, 0x00 );
	buf.kill();

}

void Crypto::F9::process(
	const	Buffer &	pData ) {

	buf += pData;

	while ( buf.getLength() > blkSze ) {
		IV ^= buf( 0, blkSze );
		IV = cipher->encrypt( IV );
		ACC ^= IV;
		buf = buf( blkSze );
	}

//	while ( inlen ) {
//		if (f9->buflen == f9->blkSze) {
//			cipher_descriptor[f9->cipher].ecb_encrypt(f9->IV, f9->IV, &f9->key);
//			for (x = 0; x < f9->blkSze; x++) {
//				f9->ACC[x] ^= f9->IV[x];
//			}
//			f9->buflen = 0;
//		}
//		f9->IV[f9->buflen++] ^= *in++;
//		--inlen;
//	}

}

void Crypto::F9::done(
		Buffer &	pTag ) {

	if ( buf.hasContent() ) {
		for ( Uint32 i = 0 ; i < buf.getLength() ; i++ ) {
			IV[ i ] ^= buf[ i ];
		}
		IV = cipher->encrypt( IV );
		buf.kill();
		ACC ^= IV;
	}

	cipher->setKey( key2 );

	ACC = cipher->encrypt( ACC );

	pTag = ACC;

}

// ============================================================================

void Crypto::F9::test() {

	static const struct {
		Uint32	msglen;
		Uchar	K[16], M[128], T[4];
	} tests[] = {
		{
			20,
			{ 0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00, 0x95, 0x2C, 0x49, 0x10, 0x48, 0x81, 0xFF, 0x48 },
			{ 0x38, 0xA6, 0xF0, 0x56, 0xB8, 0xAE, 0xFD, 0xA9, 0x33, 0x32, 0x34, 0x62, 0x63, 0x39, 0x38, 0x61, 0x37, 0x34, 0x79, 0x40 },
			{ 0x46, 0xE0, 0x0D, 0x4B }
		},
		{
			105,
			{ 0x83, 0xFD, 0x23, 0xA2, 0x44, 0xA7, 0x4C, 0xF3, 0x58, 0xDA, 0x30, 0x19, 0xF1, 0x72, 0x26, 0x35 },
			{ 0x36, 0xAF, 0x61, 0x44, 0x4F, 0x30, 0x2A, 0xD2,
			  0x35, 0xC6, 0x87, 0x16, 0x63, 0x3C, 0x66, 0xFB, 0x75, 0x0C, 0x26, 0x68, 0x65, 0xD5, 0x3C, 0x11, 0xEA, 0x05, 0xB1, 0xE9, 0xFA, 0x49, 0xC8, 0x39, 0x8D, 0x48, 0xE1, 0xEF, 0xA5, 0x90, 0x9D, 0x39,
			  0x47, 0x90, 0x28, 0x37, 0xF5, 0xAE, 0x96, 0xD5, 0xA0, 0x5B, 0xC8, 0xD6, 0x1C, 0xA8, 0xDB, 0xEF, 0x1B, 0x13, 0xA4, 0xB4, 0xAB, 0xFE, 0x4F, 0xB1, 0x00, 0x60, 0x45, 0xB6, 0x74, 0xBB, 0x54, 0x72,
			  0x93, 0x04, 0xC3, 0x82, 0xBE, 0x53, 0xA5, 0xAF, 0x05, 0x55, 0x61, 0x76, 0xF6, 0xEA, 0xA2, 0xEF, 0x1D, 0x05, 0xE4, 0xB0, 0x83, 0x18, 0x1E, 0xE6, 0x74, 0xCD, 0xA5, 0xA4, 0x85, 0xF7, 0x4D, 0x7A,
			  0x40|0x80 },
			{ 0x95, 0xAE, 0x41, 0xBA },
		}
	};

	CipherPtr kasumi = CipherRepository::instance()->getNew( "kasumi" );

	for ( Uint32 x = 0 ; x < (sizeof(tests)/sizeof(tests[0])) ; x++ ) {
		init( kasumi, Buffer( tests[x].K, 16 ) );
		process( Buffer( tests[x].M, tests[x].msglen ) );
		Buffer	out;
		done( out );
		if ( out( 0, 4 ) != Buffer( tests[x].T, 4 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

static Crypto::MACRegisterer< Crypto::F9 > f9Registerer;

// ============================================================================

