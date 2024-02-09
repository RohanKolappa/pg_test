// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Math.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Math.RNG.h"
#include "TBFC.Math.Utils.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

static Math::RNG rng;

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

Bool Math::randBool() {

	return ( rand() <= 0x7FFFFFFF );

}

void Math::srand(
	const	Uint32		seed ) {

	rng.randomInit( seed );

}

// ============================================================================





