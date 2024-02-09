// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::Base".
// 
// "VMP::Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.MuxedSession.h"
#include "VMP.SessionFactory.h"
#include "VMP.SessionSerializer.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( MuxedSession )

// ============================================================================

const DL::TypeCPtr & MuxedSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "109d544f-5492-451c-8c9b-6fff5092759f" ),
		"VMP.MuxedSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

MuxedSession::MuxedSession(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	Session		( pFlags ),
	bitRate		( 0 ) {

	setStream( new MuxedStream );

}

MuxedSession::MuxedSession(
	const	MuxedSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	Serializable(),
	Session		( pOther ),
	bitRate		( 0 ) {

	setStream( new MuxedStream );

	for ( Uint32 i = 0 ; i < pOther.session.getSize() ; i++ ) {
		addComponent( pOther.session[ i ]->clone() );
	}

	if ( pOther.hasMuxedIndex() ) {
		muxedIndex = new MuxedIndex( *pOther.getMuxedIndex() );
	}

}

// ============================================================================

SessionPtr MuxedSession::clone() const {

	return new MuxedSession( *this );

}

// ============================================================================

Bool MuxedSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	throw NotImplemented( "MuxedSession::isCompatibleWith()" );

}

// ============================================================================

Uint32 MuxedSession::addComponent(
		SessionPtr	pSession ) {

	Uint32 res = session.getSize();

	if ( ! pSession->hasIndex() ) {
		pSession->setIndex( res );

	}
	else if ( pSession->getIndex() != res ) {
		throw InternalError( "MuxedSession::addComponent(): child has an index!" );
	}

	session += pSession;

	getMuxedStream()->addComponent( pSession->getStream() );

	return res;

}

// ============================================================================

SessionCPtr MuxedSession::findComponentWithStream(
		StreamCPtr	pStream ) const {

	SessionCPtr s;

	for ( Uint32 i = 0 ; i < session.getSize() ; i++ ) {
		if ( session[ i ]->getStream() == pStream ) {
			s = session[ i ];
			break;
		}
	}

	return s;

}

// ============================================================================

Buffer MuxedSession::toBuffer() const {

	Buffer res = Session::toBuffer() + "\n"
		+ "MuxedSession" + "\n"
		+ "    bitRate: " + Buffer( bitRate );

	for ( Uint32 i = 0 ; i < session.size() ; i++ ) {
		res += "\n"
			"Sub Session[" + Buffer( i ) + "]\n"
			+ session[ i ]->toBuffer();
	}

	return res;

}

// ============================================================================

void MuxedSession::doSerialize(
		BFC::IO::ByteOutputStreamPtr	out ) const {

	Session::doSerialize( out );

	out->putLEUint32( bitRate );

	out->putLEUint32( session.size() );

	for ( Uint32 i = 0 ; i < session.size() ; i++ ) {
		SessionSerializer::doSerialize( out, session[ i ] );
	}

}

void MuxedSession::unSerialize(
		BFC::IO::ByteInputStreamPtr	in ) {

	Session::unSerialize( in );

	bitRate = in->getLEUint32();

	Uint32 s = in->getLEUint32();

	setStream( new MuxedStream );

	while ( s-- ) {
		addComponent( SessionSerializer::unSerialize( in ) );
	}

}

// ============================================================================

static SessionFactoryTmpl< MuxedSession > muxedSessionFactory;

// ============================================================================

