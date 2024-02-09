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
//	VMP.M2S.PESFrame.cpp
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

#include "VMP.M2S.PESFrame.h"
#include "VMP.M2S.PESPacket.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PESFrame )

// ============================================================================

const DL::TypeCPtr & M2S::PESFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "f1f7fb97-8455-4040-b4e4-838f03464f89" ),
		"VMP.M2S.PESFrame",
		BytesFrame::getClassType() );

	return i;

}

// ============================================================================

M2S::PESFrame::PESFrame() :

	DL::Object	( getClassType() ) {

}

M2S::PESFrame::PESFrame(
		PESPacketCPtr	pPESPackt ) :

	DL::Object	( getClassType() ),
// FIXME: Spurious initialization, which requires the PESPacket to already be encoded.
	BytesFrame	( pPESPackt->packetDump ),
	pesPackt	( pPESPackt ) {

}

M2S::PESFrame::PESFrame(
	const	PESFrame &	pOther ) :

	SObject		(),
	Cloneable	(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	BytesFrame	( pOther ),
	pesPackt	( pOther.pesPackt ) {

}

// ============================================================================

SPtr M2S::PESFrame::clone() const {

	return new PESFrame( *this );

}

// ============================================================================

void M2S::PESFrame::doSerialize(
		IO::ByteOutputStreamPtr
				pBOStream ) const {

	BytesFrame::doSerialize( pBOStream );

}

void M2S::PESFrame::unSerialize(
		IO::ByteInputStreamPtr
				pBIStream ) {

	BytesFrame::unSerialize( pBIStream );

	PESPacketPtr t = new PESPacket;

	t->decode( getData() );

	pesPackt = t;

}

// ============================================================================

static FrameFactoryTmpl< M2S::PESFrame > frameFactory;

// ============================================================================

