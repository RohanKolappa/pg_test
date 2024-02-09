// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TimeStamp.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.M2S.TimeStamp.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, TimeStamp )

// ============================================================================

M2S::TimeStamp M2S::TimeStamp::operator - (
	const	TimeStamp &	pOther ) const {

	TimeStamp res;

	if ( ext >= pOther.ext ) {
		res.ext = ext - pOther.ext;
		res.val = val - pOther.val;
	}
	else {
		res.ext = ext - pOther.ext + 300;
		res.val = val - pOther.val - 1;
	}

	return res;

}

// ============================================================================

void M2S::TimeStamp::decode_33_6_9(
	const	Uchar *		ptr ) {

	val = ( Uint64 )( *ptr++ );
	val <<= 8;
	val |= ( Uint64 )( *ptr++ );
	val <<= 8;
	val |= ( Uint64 )( *ptr++ );
	val <<= 8;
	val |= ( Uint64 )( *ptr++ );
	val <<= 1;
	val |= ( Uint64 )( ( *ptr & 0x80 ) >> 7 );

	ext = ( Uint32 )( *ptr++ & 0x01 );
	ext <<= 8;
	ext |= ( Uint32 )( *ptr++ );

}

// ============================================================================

void M2S::TimeStamp::decode_4_3_1_15_1_15_1(
	const	Uchar *		ptr ) {

	Uint32	b32 = ( ( Uint32 )( ptr[ 0 ] & 0x08 ) >>  3 );	// 32
	Uint32	b00 = ( ( Uint32 )( ptr[ 0 ] & 0x06 ) << 29 )	// 31..30
		    | ( ( Uint32 )( ptr[ 1 ]        ) << 22 )	// 29..22
		    | ( ( Uint32 )( ptr[ 2 ] & 0xFE ) << 14 )	// 21..15
		    | ( ( Uint32 )( ptr[ 3 ]        ) <<  7 )	// 14..7
		    | ( ( Uint32 )( ptr[ 4 ] & 0xFE ) >>  1 );	// 6..0

	val = ( ( Uint64 )b32 << 32 )
	    | ( ( Uint64 )b00       );
	ext = 0;

}

void M2S::TimeStamp::encode_4_3_1_15_1_15_1(
		Uchar *		ptr ) {

	Uint64 i = val;

	ptr[ 0 ] = ( Uchar )( ( i & CU64( 0x00000001C0000000 ) ) >> 29 );
	ptr[ 1 ] = ( Uchar )( ( i & CU64( 0x000000003FC00000 ) ) >> 22 );
	ptr[ 2 ] = ( Uchar )( ( i & CU64( 0x00000000003F8000 ) ) >> 14 );
	ptr[ 3 ] = ( Uchar )( ( i & CU64( 0x0000000000007F80 ) ) >>  7 );
	ptr[ 4 ] = ( Uchar )( ( i & CU64( 0x000000000000007F ) ) <<  1 );

}

// ============================================================================

void M2S::TimeStamp::decode_2_3_1_15_1_15_1_9_1(
	const	Uchar *		ptr ) {

	Uint32	b32 = ( ( Uint32 )( ptr[ 0 ] & 0x20 ) >>  5 );
	Uint32	b00 = ( ( Uint32 )( ptr[ 0 ] & 0x18 ) << 27 )	// 31..30
		    | ( ( Uint32 )( ptr[ 0 ] & 0x03 ) << 28 )	// 29..28 
		    | ( ( Uint32 )( ptr[ 1 ]        ) << 20 )	// 27..20
		    | ( ( Uint32 )( ptr[ 2 ] & 0xF8 ) << 12 )	// 19..15
		    | ( ( Uint32 )( ptr[ 2 ] & 0x03 ) << 13 )	// 14..13
		    | ( ( Uint32 )( ptr[ 3 ]        ) <<  5 )	// 12..5
		    | ( ( Uint32 )( ptr[ 4 ] & 0xF8 ) >>  3 );	// 4..0

	val = ( ( Uint64 )b32 << 32 )
	    | ( ( Uint64 )b00       );

	ext = ( ( Uint32 )( ptr[ 4 ] & 0x03 ) << 7 )	// 8..7
	    | ( ( Uint32 )( ptr[ 5 ] & 0xFE ) >> 1 );	// 6..0

}

// ============================================================================

Buffer M2S::TimeStamp::toBuffer() const {

	return Buffer( val ) + ":" + Buffer( ext );

}

// ============================================================================

void M2S::TimeStamp::fromClock(
	const	Time::Clock &	pClock ) {

	Uint64	t = pClock * 27;

	val = t / 300;
	ext = t % 300;

}

Time::Clock M2S::TimeStamp::toClock() const {

	// val is in units of a clock at 90 kHz
	// ext is in units of a clock at 27 MHz

	return Time::Clock( ( val * ( Uint64 )300 + ( Uint64 )ext ) / ( Uint64 )27 );

}

// ============================================================================

