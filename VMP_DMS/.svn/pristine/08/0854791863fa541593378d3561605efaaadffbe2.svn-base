// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.TFloat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Registerer.h"
#include "BFC.Dyn.TFloat.h"
#include "BFC.Dyn.VFloat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TFloat )

// ============================================================================

Dyn::TFloat::TFloat() :

	Type	( /*TypeFloat*/ ) {

	setName( "float" );
	setUUId( UUId( "5fa0791b-c699-46c9-9afd-150f57bd73c8" ) );
	setLocked();

}

// ============================================================================

Dyn::TFloatPtr Dyn::TFloat::instance() {

	static TFloatPtr i = new TFloat;

	return i;

}

// ============================================================================

SPtr Dyn::TFloat::clone() const {

	return instance();

}

Dyn::VarPtr Dyn::TFloat::create() {

	return new VFloat( instance() ); // Win32...

}

// ============================================================================

static Dyn::Registerer registerer( Dyn::TFloat::instance() );

// ============================================================================

