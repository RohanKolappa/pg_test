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
//	VMP.L3.PictureEncoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.VideoFrame.h"

#include "VMP.L3.PictureEncoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, PictureEncoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::PictureEncoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "e2a0c424-8a94-422d-9936-7cefe728d215" ),
		"VMP.L3.PictureEncoderBackend",
		L3::BlockEncoderBackend::getClassType() );

	return i;

}

// ============================================================================

VideoCompression L3::PictureEncoderBackend::getVideoCompression() const {

	return VideoCompression::Unknown;

}

// ============================================================================

Bool L3::PictureEncoderBackend::canEncodeFrame(
		FramePtr		pDecoded ) {

	return canEncode( VideoFramePtr( pDecoded )->getPic() );

}

BytesFramePtr L3::PictureEncoderBackend::encodeFrame(
		FramePtr		pDecoded,
		SessionPtr		/* pSession */ ) {

	return new BytesFrame( encode( VideoFramePtr( pDecoded )->getPic() ) );

}

// ============================================================================

Bool L3::PictureEncoderBackend::canEncode(
	const	Image::Picture &	/* pPicture */ ) {

	return true;

}

// ============================================================================

