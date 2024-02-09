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
//	VMP.L3.BytesProducerBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.BytesSession.h"

#include "VMP.L3.BytesProducerBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, BytesProducerBackend )

// ============================================================================

const DL::TypeCPtr & L3::BytesProducerBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "2442d226-3648-4313-a178-8b26f7e6cf83" ),
		"VMP.L3.BytesProducerBackend",
		L3::ProducerBackend::getClassType() );

	return i;

}

// ============================================================================

L3::BytesProducerBackend::BytesProducerBackend(
	const	DL::TypeCPtr &	pType ) :

	ProducerBackend( pType, BytesSession::getClassType() ) {

}

// ============================================================================

