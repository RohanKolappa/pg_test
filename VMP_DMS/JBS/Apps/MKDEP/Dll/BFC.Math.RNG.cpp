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
//	BFC.RNG.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.Math.RNG.h"
#include "BFC.Base.Time.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL(RNG)

// ============================================================================

RNG::RNG() {

	randomInit( Time::now().getSecondsSinceEpoch() );

}

RNG::~RNG() {



}

// ============================================================================

void RNG::randomInit(
	const	Uint32		seed ) {

	mt[0]= seed;

	for ( mti = 1 ; mti < MERS_N ; mti++ ) {
		mt[mti] = ( 1812433253UL * ( mt[mti-1] ^ ( mt[mti-1] >> 30 ) ) + mti );
	}

	mti = MERS_N;

}

Uint32 RNG::rand(
	const	Uint32		max ) {

	Uint32 r = (Uint32)( ( (Double)( max ) + 1.0 ) * rand() );

	if ( r > max )
		r = max;

	return r;

}

Uint64 RNG::rand(
	const	Uint64		max ) {

#if defined( PLATFORM_WIN32 )
	Double dmax = (Double)(Int64)( max );
	if ( dmax < 0.0 ) {
		dmax += (Double)( Int64Max ) + 1.0;
		dmax += (Double)( Int64Max ) + 1.0;
	}
#else
	Double dmax = (Double)( max );
#endif

	Uint64 r = (Uint64)( ( dmax + 1.0 ) * rand() );

	if ( r > max )
		r = max;

	return r;

}

Double RNG::rand() {

	Uint32 r = brand();

	return	(Double)r * ( 1.0 / ( (Double)(Uint32)(-1L) + 1.0 ) );

}


Uint32 RNG::brand() {

	Uint32 y;

	if ( mti >= MERS_N ) {

		// generate MERS_N words at one time

		const Uint32 LOWER_MASK = (1LU << MERS_R) - 1;	// lower MERS_R bits
		const Uint32 UPPER_MASK = (Uint32)-1 << MERS_R;	// upper (32 - MERS_R) bits
		static const Uint32 mag01[2] = {0, MERS_A};

		int kk;
		for (kk=0; kk < MERS_N-MERS_M; kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
			mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];}

		for (; kk < MERS_N-1; kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
			mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];}

		y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
		mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
		mti = 0;

	}

	y = mt[mti++];

	y ^= ( y >> MERS_U );
	y ^= ( y << MERS_S ) & MERS_B;
	y ^= ( y << MERS_T ) & MERS_C;
	y ^= ( y >> MERS_L );

	return y;

}

// ============================================================================

