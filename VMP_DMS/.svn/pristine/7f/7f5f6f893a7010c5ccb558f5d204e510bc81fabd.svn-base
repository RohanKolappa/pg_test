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
//	VMP.L3.VideoProducerBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.VideoSession.h"

#include "VMP.L3.VideoProducerBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, VideoProducerBackend )

// ============================================================================

const DL::TypeCPtr & L3::VideoProducerBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "40d9782d-abe6-45a9-afb4-912d9c19be5a" ),
		"VMP.L3.VideoProducerBackend",
		L3::ProducerBackend::getClassType() );

	return i;

}

// ============================================================================

L3::VideoProducerBackend::VideoProducerBackend(
	const	DL::TypeCPtr &	pType ) :

	ProducerBackend( pType, VideoSession::getClassType() ) {

}

// ============================================================================

