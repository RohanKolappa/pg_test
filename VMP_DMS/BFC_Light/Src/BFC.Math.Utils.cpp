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
//	BFC.Math.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <math.h>

#include "BFC.Math.RNG.h"
#include "BFC.Math.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

static Math::RNG rng;

// ============================================================================

Double Math::sqrt(
	const	Double		d ) {

	return ::sqrt(d);

}

// ============================================================================

Uint32 Math::getRandMax() {

	return 0xFFFFFFFF;

}

Uint32 Math::rand() {

	return rng.rand( 0xFFFFFFFF );

}

Uint32 Math::rand(
	const	Uint32		range ) {

	return rng.rand( range );

}

Uint64 Math::rand(
	const	Uint64		range ) {

	return rng.rand( range );

}

Bool Math::randBool() {

	return ( rand() <= 0x7FFFFFFF );

}

void Math::srand(
	const	Uint32		seed ) {

	rng.randomInit( seed );

}

// ============================================================================

