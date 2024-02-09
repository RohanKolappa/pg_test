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
//	VMP.L3.ProcessorBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.ProcessorBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, ProcessorBackend )

// ============================================================================
// VMP::L3::ProcessorBackend
// ============================================================================

const DL::TypeCPtr & L3::ProcessorBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f002247c-08d7-49c7-baf0-b5c547fa9884" ),
		"VMP.L3.ProcessorBackend",
		L3::Backend::getClassType() );

	return i;

}

// ============================================================================

Uint64 L3::ProcessorBackend::convertOffset(
	const	Uint64		pOffset ) const {

	return pOffset;

}

// ============================================================================
// VMP::L3::AudioProcessorBackend
// ============================================================================

const DL::TypeCPtr & L3::AudioProcessorBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "94d41a28-7e8b-423b-8d9d-a80ebc8b8bc7" ),
		"VMP.L3.AudioProcessorBackend",
		L3::ProcessorBackend::getClassType() );

	return i;

}

// ============================================================================
// VMP::L3::VideoProcessorBackend
// ============================================================================

const DL::TypeCPtr & L3::VideoProcessorBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "488982b7-7bdb-4467-a8d2-6be7caa88c36" ),
		"VMP.L3.VideoProcessorBackend",
		L3::ProcessorBackend::getClassType() );

	return i;

}

// ============================================================================

