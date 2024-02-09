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
//	VMP.VideoFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.FrameFactory.h"
#include "VMP.VideoFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( VideoFrame )

// ============================================================================

const DL::TypeCPtr & VideoFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9f7f4481-de70-4034-a106-61c1067b4bc1" ),
		"VMP.VideoFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

SPtr VideoFrame::clone() const {

	return new VideoFrame( *this );

}

// ============================================================================

Uint32 VideoFrame::getLength() const {

	return 1;

}

// ============================================================================

Buffer VideoFrame::toBuffer() const {

	return Frame::toBuffer() + ", video: TBD";

}

// ============================================================================

void VideoFrame::doSerialize(
		IO::ByteOutputStreamPtr		out ) const {

	Frame::doSerialize( out );

	pic.doSerialize( out );

}

void VideoFrame::unSerialize(
		IO::ByteInputStreamPtr		in ) {

	Frame::unSerialize( in );

	pic.unSerialize( in );

}

// ============================================================================

static FrameFactoryTmpl< VideoFrame > videoFactory;

// ============================================================================

