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
//	VMP.RTP.TFrame.cpp
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

#include "BFC.Net.SockAddr.h"

#include "VMP.FrameFactory.h"

#include "VMP.RTP.TFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TFrame )

// ============================================================================

const DL::TypeCPtr & RTP::TFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "bf76f482-a143-4390-8549-3502a7fcea63" ),
		"VMP.RTP.TFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

RTP::TFrame::TFrame() :

	DL::Object	( getClassType() ) {

}

RTP::TFrame::TFrame(
	const	TFrame &	pOther ) :

	SObject		(),
	Cloneable	(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	Frame		(),

	srcSAddr	( pOther.srcSAddr ),
	tgtSAddr	( pOther.tgtSAddr ),
	readTime	( pOther.readTime ),
	M		( pOther.M ),
	PT		( pOther.PT ),
	SN		( pOther.SN ),
	TS		( pOther.TS ),
	SSRC		( pOther.SSRC ),
	data		( pOther.data ) {

}

// ============================================================================

SPtr RTP::TFrame::clone() const {

	return new TFrame( *this );

}

// ============================================================================

Uint32 RTP::TFrame::getLength() const {

	return 1;

}

// ============================================================================

Buffer RTP::TFrame::toBuffer() const {

	return Frame::toBuffer()
		+ ", "
		+ "rx: " + readTime.dateTimeToBuffer( false, true )
		+ ", "
		+ ( srcSAddr ? srcSAddr->toBuffer( false ) : Buffer( "???" ) )
		+ " ---> "
		+ ( tgtSAddr ? tgtSAddr->toBuffer( false ) : Buffer( "???" ) )
		+ ", flgs: "
		+ ( M ? "M" : "." )
		+ ", pt: " + Buffer( PT )
		+ ", sn: " + Buffer( SN, Buffer::Base16, 4 )
		+ ", ts: " + Buffer( TS )
		+ ", ssrc: " + Buffer( SSRC, Buffer::Base16, 8 )
		+ ", size: " + Buffer( data.getLength(), Buffer::Base16, 4 )
		+ ", data: " + Utils::toHex( data( 0, 64 ) );

}

// ============================================================================

void RTP::TFrame::doSerialize(
		IO::ByteOutputStreamPtr
				pBOStream ) const {

	Frame::doSerialize( pBOStream );

	throw NotImplemented();

}

void RTP::TFrame::unSerialize(
		IO::ByteInputStreamPtr
				pBIStream ) {

	Frame::unSerialize( pBIStream );

	throw NotImplemented();

}

// ============================================================================

static FrameFactoryTmpl< RTP::TFrame > frameFactory;

// ============================================================================

