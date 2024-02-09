// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Math.RNG.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Math_RNG_H_
#define _BFC_Math_RNG_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {
namespace Math {

// ============================================================================

/// \brief Random number generator.
///
/// \ingroup BFC_Base

class BFC_Light_DLL RNG {

public :

	RNG(
	);

	virtual ~RNG(
	);

	void randomInit(
		const	Uint32		seed
	);

	/// \brief Output a random Uint32 in the interval 0 <= x <= max.

	Uint32 rand(
		const	Uint32		max
	);

	/// \brief Output a random Uint64 in the interval 0 <= x <= max.

	Uint64 rand(
		const	Uint64		max
	);

	/// \brief Output random float number in the interval 0 <= x < 1.

	Double rand(
	);

	/// \brief Generate 32 random bits.

	Uint32 brand(
	);

protected :

private :

	// internal state...

#define MERS_N   624
#define MERS_M   397
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000

	Uint32	mt[MERS_N];	// state vector
	Uint32	mti;		// index into mt

	// others...

	RNG(
		const	RNG		&
	);

	RNG& operator = (
		const	RNG		&
	);

};

// ============================================================================

} // namespace Math
} // namespace BFC

// ============================================================================

#endif // _BFC_Math_RNG_H_

// ============================================================================

