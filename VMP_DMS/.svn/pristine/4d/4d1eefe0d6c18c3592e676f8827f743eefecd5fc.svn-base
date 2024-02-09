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
//	BFC.Crypto.Pelican.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.Crypto.AES.h"
#include "BFC.Crypto.Functions.h"
#include "BFC.Crypto.Pelican.h"
#include "BFC.Crypto.MACRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, Pelican )

// ============================================================================

const DL::TypeCPtr & Crypto::Pelican::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "871ee9aa-2828-40b4-912d-7151ea41b769" ),
		"BFC.Crypto.Pelican",
		Crypto::MAC::getClassType(),
		new DL::Descr(
			"pelican",
			"MAC",
			"Pelican MAC" ) );

	return i;

}

// ============================================================================

Crypto::Pelican::Pelican() :

	MAC( getClassType() ) {

}

// ============================================================================

void Crypto::Pelican::init(
	const	Buffer &	pKey ) {

	cipher = new AES;

	cipher->setKey( pKey );

	state	= cipher->encrypt( Buffer::chars( 16, 0x00 ) );
	buf.kill();

}

void Crypto::Pelican::process(
	const	Buffer &	pData ) {

	buf += pData;

	while ( buf.getLength() >= 16 ) {
		state ^= buf( 0, 16 );
		four_rounds();
		buf = buf( 16 );
	}

}

void Crypto::Pelican::done(
		Buffer &	pTag ) {

	for ( Uint32 i = 0 ; i < buf.getLength() ; i++ ) {
		state[ i ] ^= buf[ i ];
	}

	state[ buf.getLength() ] ^= 0x80;

	cipher->encrypt( state, pTag );

}

// ============================================================================

void Crypto::Pelican::test() {

	static const struct {
		Uchar	K[32], MSG[64], T[16];
		Uint32	keylen, ptlen;
	} tests[] = {
		/* K=16, M=0 */
		{
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0 },
			{ 0xeb, 0x58, 0x37, 0x15, 0xf8, 0x34, 0xde, 0xe5,
			0xa4, 0xd1, 0x6e, 0xe4, 0xb9, 0xd7, 0x76, 0x0e, },
			16, 0
		},
		
		/* K=16, M=3 */
		{
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0x00, 0x01, 0x02 },
			{ 0x1c, 0x97, 0x40, 0x60, 0x6c, 0x58, 0x17, 0x2d,
			0x03, 0x94, 0x19, 0x70, 0x81, 0xc4, 0x38, 0x54, },
			16, 3
		},
		
		/* K=16, M=16 */
		{
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0x03, 0xcc, 0x46, 0xb8, 0xac, 0xa7, 0x9c, 0x36,
			0x1e, 0x8c, 0x6e, 0xa6, 0x7b, 0x89, 0x32, 0x49, },
			16, 16
		},
		
		/* K=16, M=32 */
		{
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },
			{ 0x89, 0xcc, 0x36, 0x58, 0x1b, 0xdd, 0x4d, 0xb5,
			0x78, 0xbb, 0xac, 0xf0, 0xff, 0x8b, 0x08, 0x15, },
			16, 32
		},
		
		/* K=16, M=35 */
		{
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
			{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
			0x20, 0x21, 0x23 },
			{ 0x4a, 0x7d, 0x45, 0x4d, 0xcd, 0xb5, 0xda, 0x8d,
			0x48, 0x78, 0x16, 0x48, 0x5d, 0x45, 0x95, 0x99, },
			16, 35
		},
	};

	for ( Uint32 i = 0 ; i < (sizeof(tests) / sizeof(tests[0])) ; i++ ) {
		init( Buffer( tests[i].K, tests[i].keylen ) );
		process( Buffer( tests[i].MSG, tests[i].ptlen ) );
		Buffer tag;
		done( tag );
		if ( tag != Buffer( tests[ i ].T, 16 ) ) {
			throw InternalError();
		}
	}

}

// ============================================================================

void Crypto::Pelican::four_rounds() {

	if ( state.getLength() != 16 ) {
		throw InternalError();
	}

	Uchar * p = state.getVarPtr();

	Uint32 s0, s1, s2, s3, t0, t1, t2, t3;

	s0 = LOAD32H( p      );
	s1 = LOAD32H( p +  4 );
	s2 = LOAD32H( p +  8 );
	s3 = LOAD32H( p + 12 );

	for ( Uint32 r = 0 ; r < 4 ; r++ ) {

		t0 =
			AES::Te0(getByte(s0, 3)) ^
			AES::Te1(getByte(s1, 2)) ^
			AES::Te2(getByte(s2, 1)) ^
			AES::Te3(getByte(s3, 0));
		t1 =
			AES::Te0(getByte(s1, 3)) ^
			AES::Te1(getByte(s2, 2)) ^
			AES::Te2(getByte(s3, 1)) ^
			AES::Te3(getByte(s0, 0));
		t2 =
			AES::Te0(getByte(s2, 3)) ^
			AES::Te1(getByte(s3, 2)) ^
			AES::Te2(getByte(s0, 1)) ^
			AES::Te3(getByte(s1, 0));
		t3 =
			AES::Te0(getByte(s3, 3)) ^
			AES::Te1(getByte(s0, 2)) ^
			AES::Te2(getByte(s1, 1)) ^
			AES::Te3(getByte(s2, 0));

		s0 = t0; s1 = t1; s2 = t2; s3 = t3;

	}

	STORE32H( s0, p      );
	STORE32H( s1, p +  4 );
	STORE32H( s2, p +  8 );
	STORE32H( s3, p + 12 );

}

// ============================================================================

static Crypto::MACRegisterer< Crypto::Pelican > hmacRegisterer;

// ============================================================================

