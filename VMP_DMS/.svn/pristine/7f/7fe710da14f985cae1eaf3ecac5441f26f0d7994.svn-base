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
//	BFC.Dyn.TSignal.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Registerer.h"
#include "BFC.Dyn.TSignal.h"
#include "BFC.Dyn.VSignal.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TSignal )

// ============================================================================

Dyn::TSignal::TSignal() :

	Type	( /*TypeSignal*/ ) {

	setName( "signal" );
	setUUId( UUId( "ec048844-0605-4dbb-9a2b-c86f1a514690" ) );
	setLocked();

}

// ============================================================================

Dyn::TSignalPtr Dyn::TSignal::instance() {

	static TSignalPtr i = new TSignal;

	return i;

}

// ============================================================================

SPtr Dyn::TSignal::clone() const {

	return instance();

}

Dyn::VarPtr Dyn::TSignal::create() {

	return new VSignal( this );

}

// ============================================================================

static Dyn::Registerer registerer( Dyn::TSignal::instance() );

// ============================================================================

