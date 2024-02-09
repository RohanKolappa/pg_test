// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.ConsumerBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.L3.ConsumerBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, ConsumerBackend )

// ============================================================================

const DL::TypeCPtr & L3::ConsumerBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "6362bc28-21ab-4ffd-b169-7a41117703ba" ),
		"VMP.L3.ConsumerBackend",
		L3::Backend::getClassType() );

	return i;

}

// ============================================================================

L3::ConsumerBackend::ConsumerBackend(
	const	DL::TypeCPtr &	pClassType,
	const	DL::TypeCPtr &	pSessionType ) :

	Backend( pClassType ),
	sessionType( pSessionType ) {

}

// ============================================================================

