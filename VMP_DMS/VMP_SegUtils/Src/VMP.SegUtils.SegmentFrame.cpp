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
//	VMP.SegUtils.SegmentFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferSerializer.h"

#include "VMP.FrameFactory.h"

#include "VMP.SegUtils.SegmentFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, SegmentFrame )

// ============================================================================

const DL::TypeCPtr & SegUtils::SegmentFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "7c136593-4a9d-4c1c-ba1b-00c77e05b6d6" ),
		"VMP.SegUtils.SegmentFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

SegUtils::SegmentFrame::SegmentFrame() :

	DL::Object	( getClassType() ),

	segmIndx	( ( Uint64 )-1 ) {

}

SegUtils::SegmentFrame::SegmentFrame(
	const	Uint64		pSegmIndx,
	const	Frac64 &	pSegmDrtn,
	const	Buffer &	pFileName ) :

	DL::Object	( getClassType() ),

	segmIndx	( pSegmIndx ),
	segmDrtn	( pSegmDrtn ),
	fileName	( pFileName ) {

}

SegUtils::SegmentFrame::SegmentFrame(
	const	SegmentFrame &	pOther ) :

	SObject		(),
	Cloneable	(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Frame		( pOther ),

	segmIndx	( pOther.segmIndx ),
	segmDrtn	( pOther.segmDrtn ),
	fileName	( pOther.fileName ) {

}

// ============================================================================

SPtr SegUtils::SegmentFrame::clone() const {

	return new SegmentFrame( *this );

}

// ============================================================================

Uint32 SegUtils::SegmentFrame::getLength() const {

	return 1;

}

// ============================================================================

void SegUtils::SegmentFrame::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	Frame::doSerialize( pOStream );

	pOStream->putLEUint64( segmIndx );
	pOStream->putLEInt64( segmDrtn.getNum() );
	pOStream->putLEInt64( segmDrtn.getDen() );

	BufferSerializer::doSerialize( fileName, pOStream );

}

void SegUtils::SegmentFrame::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	Frame::unSerialize( pIStream );

	segmIndx = pIStream->getLEUint64();
	segmDrtn.setNum( pIStream->getLEInt64() );
	segmDrtn.setDen( pIStream->getLEInt64() );

	fileName = BufferSerializer::unSerialize( pIStream );

}

// ============================================================================

static FrameFactoryTmpl< SegUtils::SegmentFrame > frameFactory;

// ============================================================================

