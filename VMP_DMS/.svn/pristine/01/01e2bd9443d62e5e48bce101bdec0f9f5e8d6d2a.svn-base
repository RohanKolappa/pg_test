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
//	VMP.RTP.TSession.cpp
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

#include "VMP.RTP.TSession.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( RTP, TSession )

// ============================================================================

const DL::TypeCPtr & RTP::TSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "d9ca9211-05e8-4dac-877a-254259373616" ),
		"VMP.RTP.TSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

RTP::TSession::TSession() :

	DL::Object	( getClassType() ),
	Session		( Pausable ) {

}

RTP::TSession::TSession(
	const	Flags		pFlags,
		StreamPtr	pAVStream,
		SProgramCPtr	pSProgram,
		SEStreamCPtr	pSEStream ) :

	DL::Object	( getClassType() ),
	Session		( pAVStream, pFlags ),
	sProgram	( pSProgram ),
	sEStream	( pSEStream ) {

}

RTP::TSession::TSession(
	const	TSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	Session		( pOther ),
	sProgram	( pOther.sProgram ),
	sEStream	( pOther.sEStream ) {

}

// ============================================================================

VMP::SessionPtr RTP::TSession::clone() const {

	return new TSession( *this );

}

// ============================================================================

Bool RTP::TSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer RTP::TSession::toBuffer() const {

	Buffer res = Session::toBuffer() + "\n"
		+ "RTP::TSession" + "\n"
		+ "... N/A: ";

	return res;

}

// ============================================================================

void RTP::TSession::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Session::doSerialize( out );

	throw NotImplemented();

}

void RTP::TSession::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Session::unSerialize( in );

	throw NotImplemented();

}

// ============================================================================

static SessionFactoryTmpl< RTP::TSession > sessionFactory;

// ============================================================================

