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
//	BFC.Crypto.PMAC.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.Crypto.CipherRepository.h"
#include "BFC.Crypto.MACRegisterer.h"
#include "BFC.Crypto.PMAC.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, PMAC )

// ============================================================================

const DL::TypeCPtr & Crypto::PMAC::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e3985219-2017-4d1d-a23f-2c37e595e616" ),
		"BFC.Crypto.PMAC",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"pmac",
			"MAC",
			"PMAC MAC" ) );

	return i;

}

// ============================================================================

Crypto::PMAC::PMAC() :

	MAC( getClassType() ) {

}

// ============================================================================

void Crypto::PMAC::init(
		CipherPtr	pCipher,
	const	Buffer &	pKey ) {

	cipher	= pCipher;
	blkSze	= cipher->getBlockSize();
	Lr	= Buffer::chars( blkSze, 0x00 );
	Li	= Buffer::chars( blkSze, 0x00 );
	block	= Buffer::chars( blkSze, 0x00 );
	chkSum	= Buffer::chars( blkSze, 0x00 );
	blkIdx	= 1;
	tmpBuf.kill();

	Uint32 poly;

	for ( poly = 0 ; poly < (sizeof(polys)/sizeof(polys[0])) ; poly++ ) {
		if ( polys[ poly ].len == blkSze ) {
			break;
		}
	}

	if ( polys[ poly ].len != blkSze ) {
		throw InvalidArgument();
	}

	cipher->setKey( pKey );

	Buffer L = cipher->encrypt( Buffer::chars( blkSze, 0x00 ) );

	Ls[ 0 ] = L;

	for ( Uint32 x = 1 ; x < 32 ; x++ ) {
		Ls[ x ] = Buffer::chars( blkSze, 0x00 );
		Uint32 m = Ls[ x - 1 ][ 0 ] >> 7;
		for ( Uint32 y = 0 ; y < blkSze - 1 ; y++ ) {
			Ls[ x ][ y ]
				= ( Ls[ x - 1 ][ y     ] << 1 )
				| ( Ls[ x - 1 ][ y + 1 ] >> 7 );
		}
		Ls[ x ][ blkSze - 1 ] = Ls[ x - 1 ][ blkSze - 1 ] << 1;
		if ( m == 1 ) {
			Ls[ x ] ^= Buffer( polys[ poly ].poly_mul, blkSze );
		}
	}

	Uint32 m = L[blkSze-1] & 1;

	for ( Uint32 x = blkSze - 1; x > 0; x--) {
		Lr[x] = (L[x] >> 1) | (L[x-1] << 7);
	}

	Lr[0] = L[0] >> 1;

	if ( m == 1 ) {
		Lr ^= Buffer( polys[ poly ].poly_div, blkSze );
	}


}

void Crypto::PMAC::process(
	const	Buffer &	pData ) {

	tmpBuf += pData;

	while ( tmpBuf.getLength() > blkSze ) {
		Li ^= Ls[ ntz( blkIdx++ ) ];
		chkSum ^= cipher->encrypt( Li ^ tmpBuf( 0, blkSze ) );
		tmpBuf = tmpBuf( blkSze );
	}

}

void Crypto::PMAC::done(
		Buffer &	pTag ) {

	if ( tmpBuf.getLength() == blkSze ) {
		chkSum ^= tmpBuf ^ Lr;
	}
	else {
		for ( Uint32 x = 0 ; x < tmpBuf.getLength() ; x++ ) {
			chkSum[ x ] ^= tmpBuf[ x ];
		}
		chkSum[ tmpBuf.getLength() ] ^= 0x80;
	}

	pTag = cipher->encrypt( chkSum );

}

// ============================================================================

void Crypto::PMAC::test() {

	static const struct {
		const char *	key;
		const char *	msg;
		const char *	tag;
	} cases[] = {

	/* PMAC-AES-128-0B */

	{
		"000102030405060708090a0b0c0d0e0f",
		"",
		"4399572cd6ea5341b8d35876a7098af7"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"000102",
		"256ba5193c1b991b4df0c51f388a9e27"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"000102030405060708090a0b0c0d0e0f",
		"ebbd822fa458daf6dfdad7c27da76338"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"000102030405060708090a0b0c0d0e0f10111213",
		"0412ca150bbf79058d8c75a58c993f55"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
		"e97ac04e9e5e3399ce5355cd7407bc75"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2021",
		"5cba7d5eb24f7c86ccc54604e53d5512"
	},
	{
		"000102030405060708090a0b0c0d0e0f",
		"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
			"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
		"c2c9fa1d9985f6f0d2aff915a0e8d910"
	}

	};

	CipherPtr cipher = CipherRepository::instance()->getNew( "aes" );

	for ( Uint32 i = 0 ; i < (sizeof(cases) / sizeof(cases[0])) ; i++ ) {

		Buffer	key = Utils::fromHex( cases[ i ].key, false );
		Buffer	msg = Utils::fromHex( cases[ i ].msg, false );
		Buffer	tag = Utils::fromHex( cases[ i ].tag, false );
		Buffer	out;

		init( cipher, key );
		process( msg );
		done( out );

		if ( out != tag ) {
			throw InternalError( "Case: " + Buffer( i ) );
		}
	}

}

// ============================================================================

Uint32 Crypto::PMAC::ntz( Uint32 x ) {

	if ( ! x ) {
		throw InternalError();
	}

	Uint32 c = 0;

	while ((x & 1) == 0) {
		++c;
		x >>= 1;
	}

	return c;

}

// ============================================================================

const Crypto::PMAC::Poly Crypto::PMAC::polys[ 2 ] = {
	{
		8,
		{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B }
	}, {
		16,
		{ 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87 }
	}
};

// ============================================================================

static Crypto::MACRegisterer< Crypto::PMAC > pmacRegisterer;

// ============================================================================

