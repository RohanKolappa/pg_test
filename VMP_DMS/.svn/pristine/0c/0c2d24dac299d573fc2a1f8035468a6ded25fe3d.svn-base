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
//	VMP.AVC.Session.cpp
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

#include "VMP.AVC.Session.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, Session )

// ============================================================================

const DL::TypeCPtr & AVC::Session::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "86c50b21-e69b-44bd-8128-1be4adf47399" ),
		"VMP.AVC.Session",
		BytesSession::getClassType() );

	return i;

}

// ============================================================================

AVC::Session::Session() :

	DL::Object	( getClassType() ),
	BytesSession	( Pausable ),
	pcktMode	( NALUPacket ),
	strmMode	( ModeNALU ) {

}

AVC::Session::Session(
	const	BytesSession &	pOther,
	const	PacketMode	pPcktMode,
	const	StreamMode	pStrmMode ) :

	DL::Object	( getClassType() ),
	BytesSession	( pOther ),
	pcktMode	( pPcktMode ),
	strmMode	( pStrmMode ) {

}

AVC::Session::Session(
	const	Flags		pFlags,
	const	PacketMode	pPcktMode,
	const	StreamMode	pStrmMode ) :

	DL::Object	( getClassType() ),
	BytesSession	( pFlags ),
	pcktMode	( pPcktMode ),
	strmMode	( pStrmMode ) {

}

AVC::Session::Session(
	const	Session &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	BytesSession	( pOther ),
	pcktMode	( pOther.pcktMode ),
	strmMode	( pOther.strmMode ) {

}

// ============================================================================

VMP::SessionPtr AVC::Session::clone() const {

	return new Session( *this );

}

// ============================================================================

Bool AVC::Session::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer AVC::Session::toBuffer() const {

	Buffer res = BytesSession::toBuffer() + "\n"
		+ "Session" + "\n"
		+ "... framing: " + Buffer( ( Uint32 )pcktMode ) + "\n"
		+ "... streaming: " + Buffer( ( Uint32 )strmMode );

	return res;

}

// ============================================================================

void AVC::Session::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	BytesSession::doSerialize( out );

	out->putUchar( ( Uchar )pcktMode );
	out->putUchar( ( Uchar )strmMode );

}

void AVC::Session::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	BytesSession::unSerialize( in );

	pcktMode = ( PacketMode )in->getUchar();
	strmMode = ( StreamMode )in->getUchar();

}

// ============================================================================

static SessionFactoryTmpl< AVC::Session > sessionFactory;

// ============================================================================

