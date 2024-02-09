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
//	VMP.BytesFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.BytesFrame.h"
#include "VMP.FrameFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( BytesFrame )

// ============================================================================

const DL::TypeCPtr & BytesFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "723ca5fc-d0b2-43f9-9638-e1957f739666" ),
		"VMP.BytesFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

SPtr BytesFrame::clone() const {

	return new BytesFrame( *this );

}

// ============================================================================

Uint32 BytesFrame::getLength() const {

	return frmeData.length();

}

// ============================================================================

Buffer BytesFrame::toBuffer() const {

	return Frame::toBuffer()
		+ ", unitOffs: "
		+ ( unitOffs != NoOffset
			? Buffer( unitOffs, Buffer::Base16, 8 )
			: Buffer( "----------" ) )
		+ ", bytes: " + Utils::toHex( frmeData( 0, 128 ) );

}

// ============================================================================

void BytesFrame::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	Frame::doSerialize( pOStream );

	pOStream->putLEUint32( frmeData.length() );
	pOStream->putBytes( frmeData );
	pOStream->putLEUint64( unitOffs );

}

void BytesFrame::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	Frame::unSerialize( pIStream );

	frmeData	= pIStream->getBytes( pIStream->getLEUint32() );
	unitOffs	= pIStream->getLEUint64();

}

// ============================================================================

static FrameFactoryTmpl< BytesFrame > bytesFactory;

// ============================================================================

