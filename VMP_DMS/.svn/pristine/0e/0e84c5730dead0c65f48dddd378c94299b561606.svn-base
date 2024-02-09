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
//	BFC.Dyn.TBuffer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Registerer.h"
#include "BFC.Dyn.TBuffer.h"
#include "BFC.Dyn.VBuffer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, TBuffer )

// ============================================================================

Dyn::TBuffer::TBuffer() :

	Type	( /*TypeBuffer*/ ) {

	setName( "buffer" );
	setUUId( UUId( "3ecce0ec-e05c-4ed4-aa9b-4b97247f0ecd" ) );
	setLocked();

}

// ============================================================================

Dyn::TBufferPtr Dyn::TBuffer::instance() {

	static TBufferPtr i = new TBuffer;

	return i;

}

// ============================================================================

SPtr Dyn::TBuffer::clone() const {

	return instance();

}

Dyn::VarPtr Dyn::TBuffer::create() {

	return new VBuffer( this );

}

// ============================================================================

static Dyn::Registerer registerer( Dyn::TBuffer::instance() );

// ============================================================================

