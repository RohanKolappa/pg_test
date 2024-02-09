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
//	VMP.AudioSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.AudioSession.h"
#include "VMP.SessionFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( AudioSession )

// ============================================================================

const DL::TypeCPtr & AudioSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "28f1ab30-b2f3-4c8b-9a6e-a20f3d8a4aec" ),
		"VMP.AudioSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

AudioSession::AudioSession(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	Session		( pFlags ) {

}

AudioSession::AudioSession(
		AudioStreamPtr	pStream,
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	Session		( pStream, pFlags ) {

}

AudioSession::AudioSession(
	const	AudioSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable	(),
	Session		( pOther ) {

}

// ============================================================================

SessionPtr AudioSession::clone() const {

	return new AudioSession( *this );

}

// ============================================================================

Bool AudioSession::isCompatibleWith(
		SessionCPtr	pSession ) const {

	return getContent()->isCompatibleWith( pSession->getStream() );

}

// ============================================================================

Buffer AudioSession::toBuffer() const {

	return Session::toBuffer() + "\n"
		+ "AudioSession" + "\n"
		+ "    /" + "\n"
		+ ( hasContent()
		  ? getContent()->toBuffer()
		  : Buffer( "No Stream Descriptor!" ) );

}

// ============================================================================

static SessionFactoryTmpl< AudioSession > audioFactory;

// ============================================================================

