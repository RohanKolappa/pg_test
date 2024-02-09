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
//	VMP.VideoSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.SessionFactory.h"
#include "VMP.VideoSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( VideoSession )

// ============================================================================

const DL::TypeCPtr & VideoSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "a547990e-df94-4b53-a4e2-51f0f9695ccf" ),
		"VMP.VideoSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

VideoSession::VideoSession(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	Session		( pFlags ) {

}

VideoSession::VideoSession(
	const	VideoSession&	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Session		( pOther ) {

}

// ============================================================================

SessionPtr VideoSession::clone() const {

	return new VideoSession( *this );

}

// ============================================================================

Bool VideoSession::isCompatibleWith(
		SessionCPtr	pSession ) const {

	return getContent()->isCompatibleWith( pSession->getStream() );

}

// ============================================================================

Buffer VideoSession::toBuffer() const {

	return Session::toBuffer() + "\n"
		+ "VideoSession" + "\n"
		+ "    /" + "\n"
		+ ( hasContent()
		  ? getContent()->toBuffer()
		  : Buffer( "No Stream Descriptor!" ) );

}

// ============================================================================

static SessionFactoryTmpl< VideoSession > videoFactory;

// ============================================================================

