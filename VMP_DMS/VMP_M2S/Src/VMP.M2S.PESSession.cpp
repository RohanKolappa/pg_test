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
//	VMP.M2S.PESSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.SessionFactory.h"

#include "VMP.M2S.PESSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( M2S, PESSession )

// ============================================================================

const DL::TypeCPtr & M2S::PESSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "205b0ae8-60e1-4bb8-b9a1-05346d525e3e" ),
		"VMP.M2S.PESSession",
		BytesSession::getClassType() );

	return i;

}

// ============================================================================

M2S::PESSession::PESSession(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	BytesSession	( pFlags ) {

}

M2S::PESSession::PESSession(
	const	PESSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	BytesSession	( pOther ) {

}

// ============================================================================

SessionPtr M2S::PESSession::clone() const {

	return new PESSession( *this );

}

// ============================================================================

Bool M2S::PESSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer M2S::PESSession::toBuffer() const {

	Buffer res = BytesSession::toBuffer() + "\n"
		+ "PESSession" + "\n";

	return res;

}

// ============================================================================

//void M2S::PESSession::doSerialize(
//		IO::ByteOutputStreamPtr	out ) const {
//
//	BytesSession::doSerialize( out );
//
//}
//
//void M2S::PESSession::unSerialize(
//		IO::ByteInputStreamPtr	in ) {
//
//	BytesSession::unSerialize( in );
//
//}

// ============================================================================

static SessionFactoryTmpl< M2S::PESSession > sessionFactory;

// ============================================================================

