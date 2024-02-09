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
//	VMP.RTP.NSession.cpp
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

#include "VMP.SessionFactory.h"

#include "VMP.RTP.NSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, NSession )

// ============================================================================

const DL::TypeCPtr & RTP::NSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9b647e42-94d6-4fc9-bb2f-51f8181a31d2" ),
		"VMP.RTP.NSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

RTP::NSession::NSession() :

	DL::Object	( getClassType() ),
	Session		( Pausable ) {

}

RTP::NSession::NSession(
	const	Flags		pFlags,
		StreamPtr	pAVStream,
		SProgramCPtr	pSProgram,
		SEStreamCPtr	pSEStream ) :

	DL::Object	( getClassType() ),
	Session		( pAVStream, pFlags ),
	sProgram	( pSProgram ),
	sEStream	( pSEStream ) {

}

RTP::NSession::NSession(
	const	NSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	Session		( pOther ),
	sProgram	( pOther.sProgram ),
	sEStream	( pOther.sEStream ) {

}

// ============================================================================

VMP::SessionPtr RTP::NSession::clone() const {

	return new NSession( *this );

}

// ============================================================================

Bool RTP::NSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer RTP::NSession::toBuffer() const {

	Buffer res = Session::toBuffer() + "\n"
		+ "RTP::NSession" + "\n"
		+ "... N/A: ";

	return res;

}

// ============================================================================

void RTP::NSession::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Session::doSerialize( out );

	throw NotImplemented();

}

void RTP::NSession::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Session::unSerialize( in );

	throw NotImplemented();

}

// ============================================================================

static SessionFactoryTmpl< RTP::NSession > sessionFactory;

// ============================================================================

