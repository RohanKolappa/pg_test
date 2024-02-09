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
//	BFC.Dyn.TBool.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Registerer.h"
#include "BFC.Dyn.TBool.h"
#include "BFC.Dyn.VBool.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TBool )

// ============================================================================

Dyn::TBool::TBool() :

	Type	( /*TypeBool*/ ) {

	setName( "bool" );
	setUUId( UUId( "b9726801-5525-4a91-8285-ad4e25d25b97" ) );
	setLocked();

}

// ============================================================================

Dyn::TBoolPtr Dyn::TBool::instance() {

	static TBoolPtr i = new TBool;

	return i;

}

// ============================================================================

SPtr Dyn::TBool::clone() const {

	return instance();

}

Dyn::VarPtr Dyn::TBool::create() {

	return new VBool( /*this*/ );

}

// ============================================================================

static Dyn::Registerer registerer( Dyn::TBool::instance() );

// ============================================================================

