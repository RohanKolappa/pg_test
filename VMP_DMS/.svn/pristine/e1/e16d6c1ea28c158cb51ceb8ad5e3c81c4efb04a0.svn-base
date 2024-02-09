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
//	VMP.BytesSession.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Utils.h"

#include "VMP.BytesSession.h"
#include "VMP.ConfigInfo.h"
#include "VMP.ConfigInfoSerializer.h"
#include "VMP.IndexSerializer.h"
#include "VMP.SessionFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( VMP, BytesSession )

// ============================================================================

const DL::TypeCPtr & BytesSession::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "cd8d06e6-e57e-4814-ac4d-21e9709687c8" ),
		"VMP.BytesSession",
		Session::getClassType() );

	return i;

}

// ============================================================================

BytesSession::BytesSession(
	const	Flags		pFlags ) :

	DL::Object	( getClassType() ),
	Session		( pFlags ),
	bitRate		( 0 ),
	gopSize		( 0 ) {

}

BytesSession::BytesSession(
	const	BytesSession &	pOther ) :

	SObject		(),
	DObject		(),
	DL::Object	( pOther ),
	IO::Serializable(),
	Session		( pOther ),
	bitRate		( pOther.bitRate ),
	gopSize		( pOther.gopSize ) {

	if ( pOther.hasUnitsIndex() ) {
		index = pOther.getUnitsIndex()->clone();
	}

}

// ============================================================================

SessionPtr BytesSession::clone() const {

	return new BytesSession( *this );

}

// ============================================================================

Bool BytesSession::isCompatibleWith(
		SessionCPtr	/* pSession */ ) const {

	return true;

}

// ============================================================================

Buffer BytesSession::toBuffer() const {

	Buffer res = Session::toBuffer() + "\n"
		+ "BytesSession" + "\n"
		+ "    bitrate: " + Buffer( bitRate ) + "\n"
		+ "    gopsize: " + Buffer( gopSize ) + "\n";

	if ( cnfgInfo ) {
		res += "    cfgInfo: " + Utils::toHex( cnfgInfo->encode() ) + "\n";
	}

	res	+= ( hasContent()
		   ? getContent()->toBuffer()
		   : Buffer( "No Stream Descriptor!" ) ) + "\n"
		 + ( hasUnitsIndex()
		   ? getUnitsIndex()->toBuffer()
		   : Buffer( "No Index!" ) );

	return res;

}

// ============================================================================

void BytesSession::doSerialize(
		IO::ByteOutputStreamPtr	out ) const {

	Session::doSerialize( out );

	out->putLEUint32( bitRate );
	out->putLEUint32( gopSize );

	if ( hasUnitsIndex() ) {
		out->putUchar( 0x01 );
		IndexSerializer::doSerialize( out, getUnitsIndex() );
	}
	else {
		out->putUchar( 0x00 );
	}

	if ( hasConfigInfo() ) {
		out->putUchar( 0x01 );
		ConfigInfoSerializer::doSerialize( out, getConfigInfo() );
	}
	else {
		out->putUchar( 0x00 );
	}

}

void BytesSession::unSerialize(
		IO::ByteInputStreamPtr	in ) {

	Session::unSerialize( in );

	bitRate = in->getLEUint32();
	gopSize = in->getLEUint32();

	Uchar c;

	c = in->getUchar();

	if ( c == 0x01 ) {
		setUnitsIndex( IndexSerializer::unSerialize( in ) );
	}
	else if ( c == 0x00 ) {
		resetUnitsIndex();
	}
	else {
		throw InternalError( "Can't unserialize BytesSession: corrupted stream!" );
	}

	c = in->getUchar();

	if ( c == 0x01 ) {
		setConfigInfo( ConfigInfoSerializer::unSerialize( in ) );
	}
	else if ( c == 0x00 ) {
		resetConfigInfo();
	}
	else {
		throw InternalError( "Can't unserialize BytesSession: corrupted stream!" );
	}

}

// ============================================================================

static SessionFactoryTmpl< BytesSession > bytesFactory;

// ============================================================================

