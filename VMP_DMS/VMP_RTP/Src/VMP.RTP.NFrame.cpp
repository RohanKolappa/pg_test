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
//	VMP.RTP.NFrame.cpp
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

#include "VMP.RTP.NFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NFrame )

// ============================================================================

const DL::TypeCPtr & RTP::NFrame::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "65236abc-862e-4576-8eba-b367e8b468f9" ),
		"VMP.RTP.NFrame",
		Frame::getClassType() );

	return i;

}

// ============================================================================

RTP::NFrame::NFrame() :

	DL::Object	( getClassType() ) {

}

RTP::NFrame::NFrame(
		Net::SockAddrCPtr
				pSrcSAddr,
		Net::SockAddrCPtr
				pTgtSAddr,
	const	Buffer &	pBuffData,
	const	Time::Clock &	pBuffTime ) :

	DL::Object	( getClassType() ),
	Frame		(),
	srcSAddr	( pSrcSAddr ),
	tgtSAddr	( pTgtSAddr ),
	buffData	( pBuffData ),
	buffTime	( pBuffTime ) {

}

RTP::NFrame::NFrame(
	const	NFrame &	pOther ) :

	SObject		(),
	Cloneable	(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	Frame		( pOther ),
	srcSAddr	( pOther.srcSAddr ),
	tgtSAddr	( pOther.tgtSAddr ),
	buffData	( pOther.buffData ),
	buffTime	( pOther.buffTime ) {

}

// ============================================================================

SPtr RTP::NFrame::clone() const {

	return new NFrame( *this );

}

// ============================================================================

Uint32 RTP::NFrame::getLength() const {

	return buffData.getLength();

}

// ============================================================================

Buffer RTP::NFrame::toBuffer() const {

	return Frame::toBuffer()
		+ ", "
		+ buffTime.dateTimeToBuffer( true, true )
		+ ", "
		+ ( srcSAddr ? srcSAddr->toBuffer( false ) : Buffer( "???" ) )
		+ " ---> "
		+ ( tgtSAddr ? tgtSAddr->toBuffer( false ) : Buffer( "???" ) )
		+ ", "
		+ Utils::toHex( buffData( 0, 64 ) );

}

// ============================================================================

void RTP::NFrame::doSerialize(
		IO::ByteOutputStreamPtr
				pBOStream ) const {

	Frame::doSerialize( pBOStream );

	throw NotImplemented();

}

void RTP::NFrame::unSerialize(
		IO::ByteInputStreamPtr
				pBIStream ) {

	Frame::unSerialize( pBIStream );

	throw NotImplemented();

}

// ============================================================================

static FrameFactoryTmpl< RTP::NFrame > frameFactory;

// ============================================================================

