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
//	VMP.L3.ProducerBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.ProducerBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, ProducerBackend )

// ============================================================================

const DL::TypeCPtr & L3::ProducerBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "59b147aa-7d35-452e-806e-98697aeea582" ),
		"VMP.L3.ProducerBackend",
		L3::Backend::getClassType() );

	return i;

}

// ============================================================================

L3::ProducerBackend::ProducerBackend(
	const	DL::TypeCPtr &	pClassType,
	const	DL::TypeCPtr &	pSessionType ) :

	Backend( pClassType ),
	sessionType( pSessionType ) {

}

// ============================================================================

