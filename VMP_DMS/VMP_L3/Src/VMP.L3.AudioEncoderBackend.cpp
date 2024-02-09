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
//	VMP.L3.AudioEncoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.AudioEncoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, AudioEncoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::AudioEncoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "1dd9e350-e8de-4a1a-94c9-3f68e1323195" ),
		"VMP.L3.AudioEncoderBackend",
		StreamEncoderBackend::getClassType() );

	return i;

}

// ============================================================================

const DL::TypeCPtr & L3::AudioEncoderBackend::getInputType() const {

	return AudioSession::getClassType();

}

// ============================================================================

