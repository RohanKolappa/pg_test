// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.FrameFactory.h"
#include "VMP.FrameSerializer.h"
#include "VMP.MuxedFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( MuxedFrame )

// ============================================================================

const DL::TypeCPtr & MuxedFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "0d50cb1b-fc8c-4162-bebe-cdad3cb87484" ),
		"VMP.MuxedFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

SPtr MuxedFrame::clone() const {

	return new MuxedFrame( *this );

}

// ============================================================================

Uint32 MuxedFrame::getLength() const {

	return 1;

}

// ============================================================================

Buffer MuxedFrame::toBuffer() const {

	return Frame::toBuffer()
		+ ", indx: "
		+ Buffer( index )
		+ ", frame: (" + frame->toBuffer() + ")";

}

// ============================================================================

void MuxedFrame::doSerialize(
		IO::ByteOutputStreamPtr		out ) const {

	out->putLEUint32( index );

	FrameSerializer::doSerialize( out, frame );

}

void MuxedFrame::unSerialize(
		IO::ByteInputStreamPtr		in ) {

	index = in->getLEUint32();

	frame = FrameSerializer::unSerialize( in );

}

// ============================================================================

static FrameFactoryTmpl< MuxedFrame > muxedFrameFactory;

// ============================================================================

