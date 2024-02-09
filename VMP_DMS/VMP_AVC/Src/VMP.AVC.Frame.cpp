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
//	VMP.AVC.Frame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "VMP.FrameFactory.h"

#include "VMP.AVC.NALU.h"
#include "VMP.AVC.Frame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, Frame )

// ============================================================================

const DL::TypeCPtr & AVC::Frame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "65236abc-862e-4576-8eba-b367e8b468f9" ),
		"VMP.AVC.Frame",
		BytesFrame::getClassType() );

	return i;

}

// ============================================================================

AVC::Frame::Frame() :

	DL::Object	( getClassType() ) {

}

AVC::Frame::Frame(
		NALUCPtr	pNALU ) :

	DL::Object	( getClassType() ),
	BytesFrame	( pNALU->getData() ),
	nalu		( pNALU ) {

}

AVC::Frame::Frame(
		VMP::FrameCPtr	pTmplFrme,
		NALUCPtr	pNALU ) :

	DL::Object	( getClassType() ),
	BytesFrame	( pTmplFrme, pNALU->getData() ),
	nalu		( pNALU ) {

}

AVC::Frame::Frame(
	const	Frame &	pOther ) :

	SObject		(),
	Cloneable	(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	BytesFrame	( pOther ),
	nalu		( pOther.nalu ) {

}

// ============================================================================

SPtr AVC::Frame::clone() const {

	return new Frame( *this );

}

// ============================================================================

void AVC::Frame::setNALU(
		NALUCPtr	pNALU ) {

	nalu = pNALU;

	setData( nalu->getData() );

}

// ============================================================================

Buffer AVC::Frame::toBuffer() const {

	return VMP::Frame::toBuffer()
		+ ", unitOffs: "
		+ ( hasUnitOffset()
			? Buffer( getUnitOffset(), Buffer::Base16, 8 )
			: Buffer( "----------" ) )
		+ ", NALU type: " + NALU::getTypeName( nalu->getType() )
		+ ", NALU size: " + Buffer( nalu->getData().getLength() );

}

// ============================================================================

void AVC::Frame::doSerialize(
		IO::ByteOutputStreamPtr
				pBOStream ) const {

	BytesFrame::doSerialize( pBOStream );

}

void AVC::Frame::unSerialize(
		IO::ByteInputStreamPtr
				pBIStream ) {

	BytesFrame::unSerialize( pBIStream );

	nalu = new NALU( getData() );

}

// ============================================================================

static FrameFactoryTmpl< AVC::Frame > frameFactory;

// ============================================================================

