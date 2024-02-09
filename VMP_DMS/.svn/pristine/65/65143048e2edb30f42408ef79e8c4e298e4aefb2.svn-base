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
//	VMP.L3.BytesEncoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.BytesEncoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, BytesEncoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::BytesEncoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "694851c5-6460-4d48-b1c3-b9c9d7c95edb" ),
		"VMP.L3.BytesEncoderBackend",
		L3::BlockEncoderBackend::getClassType() );

	return i;

}

// ============================================================================

Bool L3::BytesEncoderBackend::canEncodeFrame(
		FramePtr		pPayload ) {

	return canEncode( BytesFramePtr( pPayload )->getData() );

}

BytesFramePtr L3::BytesEncoderBackend::encodeFrame(
		FramePtr		pPayload,
		SessionPtr		/* pSession */ ) {

	return new BytesFrame( encode( BytesFramePtr( pPayload )->getData() ) );

}

// ============================================================================

Bool L3::BytesEncoderBackend::canEncode(
	const	Buffer &		/* pPayload */ ) {

	return true;

}

// ============================================================================

