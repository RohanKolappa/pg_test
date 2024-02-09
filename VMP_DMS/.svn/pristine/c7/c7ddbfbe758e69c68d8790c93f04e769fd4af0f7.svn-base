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
//	BFC.Math.RNG.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_RNG_H_
#define _BFC_RNG_H_

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {

BFC_PTR_DECL(JBS_DLL_NONE,RNG)

} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {

// ============================================================================

class JBS_DLL_NONE RNG : virtual public BFC::SObject {

public :

	RNG(
	);

	virtual ~RNG(
	);

	void randomInit(
		const	BFC::Uint32	seed
	);

	/// \brief Output a random Uint32 in the interval 0 <= x <= max.

	BFC::Uint32 rand(
		const	BFC::Uint32	max
	);

	/// \brief Output a random Uint64 in the interval 0 <= x <= max.

	BFC::Uint64 rand(
		const	BFC::Uint64	max
	);

	/// \brief Output random float number in the interval 0 <= x < 1.

	BFC::Double rand(
	);

	/// \brief Generate 32 random bits.

	BFC::Uint32 brand(
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

	BFC::Uint32	mt[MERS_N];	// state vector
	BFC::Uint32	mti;		// index into mt

	// others...

	RNG(
		const	RNG&
	);

	RNG& operator = (
		const	RNG&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_RNG_H_

// ============================================================================

