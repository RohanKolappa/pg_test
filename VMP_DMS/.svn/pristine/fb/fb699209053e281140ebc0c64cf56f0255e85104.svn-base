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
//	BFC.Crypto.XCBC.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.CipherRepository.h"
#include "BFC.Crypto.MACRegisterer.h"
#include "BFC.Crypto.XCBC.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, XCBC )

// ============================================================================

const DL::TypeCPtr & Crypto::XCBC::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "eeeb3924-af3f-45e9-b70e-ef09352ea24f" ),
		"BFC.Crypto.XCBC",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"xcbc",
			"MAC",
			"XCBC MAC" ) );

	return i;

}

// ============================================================================

Crypto::XCBC::XCBC() :

	MAC( getClassType() ) {

}

// ============================================================================

void Crypto::XCBC::init(
		CipherPtr	pCipher,
	const	Buffer &	pKey ) {

	cipher	= pCipher;
	blkSze	= cipher->getBlockSize();
	IV	= Buffer::chars( blkSze, 0x00 );
	tmpBuf.kill();

	cipher->setKey( pKey );

	for ( Uint32 y = 0 ; y < 3 ; y++ ) {
		K[ y ] = cipher->encrypt( Buffer::chars( blkSze, y + 1 ) );
	}

	cipher->setKey( K[ 0 ] );

}

void Crypto::XCBC::process(
	const	Buffer &	pData ) {

	tmpBuf += pData;

	while ( tmpBuf.getLength() > blkSze ) {
		IV = cipher->encrypt( IV ^ tmpBuf( 0, blkSze ) );
		tmpBuf = tmpBuf( blkSze );
	}

}

void Crypto::XCBC::done(
		Buffer &	pTag ) {

	if ( tmpBuf.getLength() == blkSze ) {
		IV ^= tmpBuf;
		IV ^= K[ 1 ];
	}
	else {
		for ( Uint32 i = 0 ; i < tmpBuf.getLength() ; i++ ) {
			IV[ i ] ^= tmpBuf[ i ];
		}
		IV[ tmpBuf.getLength() ] ^= 0x80;
		IV ^= K[ 2 ];
	}

	pTag = cipher->encrypt( IV );

}

// ============================================================================

void Crypto::XCBC::test() {

	static const struct {
		Uint32	msglen;
		Uchar	K[16], M[34], T[16];
	} cases[] = {
	{
		0,
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0 },
	
		{ 0x75, 0xf0, 0x25, 0x1d, 0x52, 0x8a, 0xc0, 0x1c,
		0x45, 0x73, 0xdf, 0xd5, 0x84, 0xd7, 0x9f, 0x29 }
	},
	
	{
		3,
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0x00, 0x01, 0x02 },
	
		{ 0x5b, 0x37, 0x65, 0x80, 0xae, 0x2f, 0x19, 0xaf,
		0xe7, 0x21, 0x9c, 0xee, 0xf1, 0x72, 0x75, 0x6f }
	},
	
	{
		16,
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0xd2, 0xa2, 0x46, 0xfa, 0x34, 0x9b, 0x68, 0xa7,
		0x99, 0x98, 0xa4, 0x39, 0x4f, 0xf7, 0xa2, 0x63 }
	},
	
	{
		32,
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f },
	
		{ 0xf5, 0x4f, 0x0e, 0xc8, 0xd2, 0xb9, 0xf3, 0xd3,
		0x68, 0x07, 0x73, 0x4b, 0xd5, 0x28, 0x3f, 0xd4 }
	},
	
	{
		34,
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
	
		{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x21 },
	
		{ 0xbe, 0xcb, 0xb3, 0xbc, 0xcd, 0xb5, 0x18, 0xa3,
		0x06, 0x77, 0xd5, 0x48, 0x1f, 0xb6, 0xb4, 0xd8 },
	}

	};

	CipherPtr cipher = CipherRepository::instance()->getNew( "aes" );

	for ( Uint32 i = 0 ; i < (sizeof(cases) / sizeof(cases[0])) ; i++ ) {
		init( cipher, Buffer( cases[i].K, 16 ) );
		process( Buffer( cases[i].M, cases[i].msglen ) );
		Buffer tag;
		done( tag );
		if ( tag != Buffer( cases[ i ].T, 16 ) ) {
			throw InternalError( "Step " + Buffer( i ) );
		}
	}

}

// ============================================================================

static Crypto::MACRegisterer< Crypto::XCBC > xcbcRegisterer;

// ============================================================================

