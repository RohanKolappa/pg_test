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
//	VMP.L3.MuxedDecoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedSession.h"

#include "VMP.L3.MuxedDecoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, MuxedDecoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::MuxedDecoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "b21b4512-4926-4613-aa7f-619db5077c5d" ),
		"VMP.L3.MuxedDecoderBackend",
		L3::StreamDecoderBackend::getClassType() );

	return i;

}

// ============================================================================

const DL::TypeCPtr & L3::MuxedDecoderBackend::getOutputType() const {

	return MuxedSession::getClassType();

}

// ============================================================================

