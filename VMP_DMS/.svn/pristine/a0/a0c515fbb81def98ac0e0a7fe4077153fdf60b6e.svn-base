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
//	VMP.SegUtils.SegmentSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.SessionFactory.h"
#include "VMP.StreamSerializer.h"

#include "VMP.SegUtils.SegmentSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( SegUtils, SegmentSession )

// ============================================================================

const DL::TypeCPtr & SegUtils::SegmentSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9795277c-7420-4739-b0f0-82e13de36e6c" ),
		"VMP.SegUtils.SegmentSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

SegUtils::SegmentSession::SegmentSession(
	const	Flags		pSessFlgs ) :

	DL::Object	( getClassType() ),
	Session		( pSessFlgs ) {

}

SegUtils::SegmentSession::SegmentSession(
		BytesSessionPtr	pByteSess,
	const	Frac64 &	pSegmDrtn,
	const	Flags		pSessFlgs ) :

	DL::Object	( getClassType() ),
	Session		( pSessFlgs ),

	byteSess	( pByteSess ),
	segmDrtn	( pSegmDrtn ) {

}

SegUtils::SegmentSession::SegmentSession(
	const	SegmentSession &
				pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Session		( pOther ),

	byteSess	( pOther.byteSess ),
	segmDrtn	( pOther.segmDrtn ) {

}

// ============================================================================

SessionPtr SegUtils::SegmentSession::clone() const {

	return new SegmentSession( *this );

}

// ============================================================================

Bool SegUtils::SegmentSession::isCompatibleWith(
		SessionCPtr	pSession ) const {

	return ( pSession.isA< SegmentSession >()
	      && byteSess->isCompatibleWith( SegmentSessionCPtr( pSession )->byteSess ) );

}

// ============================================================================

Buffer SegUtils::SegmentSession::toBuffer() const {

	return Session::toBuffer() + "\n"
		+ "SegmentSession" + "\n"
		+ byteSess->toBuffer();

}

// ============================================================================

void SegUtils::SegmentSession::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	Session::doSerialize( pOStream );

	byteSess->doSerialize( pOStream );

	pOStream->putLEInt64( segmDrtn.getNum() );
	pOStream->putLEInt64( segmDrtn.getDen() );

}

void SegUtils::SegmentSession::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	Session::unSerialize( pIStream );

	byteSess = new BytesSession;

	byteSess->unSerialize( pIStream );

	segmDrtn.setNum( pIStream->getLEInt64() );
	segmDrtn.setDen( pIStream->getLEInt64() );

}

// ============================================================================

static SessionFactoryTmpl< SegUtils::SegmentSession > sessionFactory;

// ============================================================================

