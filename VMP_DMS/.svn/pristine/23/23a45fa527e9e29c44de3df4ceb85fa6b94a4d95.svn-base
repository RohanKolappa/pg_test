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
//	VMP.L3.MultiProcessorBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.MultiProcessorBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, MultiProcessorBackend )

// ============================================================================
// VMP::L3::MultiProcessorBackend
// ============================================================================

const DL::TypeCPtr & L3::MultiProcessorBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "36019fc1-c8bb-48ae-b042-c64f4650914e" ),
		"VMP.L3.MultiProcessorBackend",
		L3::Backend::getClassType() );

	return i;

}

// ============================================================================

Uint64 L3::MultiProcessorBackend::convertOffset(
	const	Uint64		pOffset ) const {

	return pOffset;

}

// ============================================================================
// VMP::L3::VideoMultiProcessorBackend
// ============================================================================

const DL::TypeCPtr & L3::VideoMultiProcessorBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "96528632-a61d-4894-99aa-96c47504cca2" ),
		"VMP.L3.VideoMultiProcessorBackend",
		L3::MultiProcessorBackend::getClassType() );

	return i;

}

// ============================================================================

