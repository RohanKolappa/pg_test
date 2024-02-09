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
//	VMP.L3.VideoEncoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.VideoSession.h"

#include "VMP.L3.VideoEncoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, VideoEncoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::VideoEncoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "dd38ff65-9312-4c51-951e-c9cd4e2d3952" ),
		"VMP.L3.VideoEncoderBackend",
		L3::StreamEncoderBackend::getClassType() );

	return i;

}

// ============================================================================

const DL::TypeCPtr & L3::VideoEncoderBackend::getInputType() const {

	return VideoSession::getClassType();

}

// ============================================================================

