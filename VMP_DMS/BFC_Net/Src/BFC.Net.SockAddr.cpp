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
//	BFC.Net.SockAddr.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.SockAddr.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, SockAddr )

// ============================================================================

Net::SockAddr::SockAddr() :

	addr	( IPAddr::getAnyLocal() ),
	port	( 0 ) {

}

Net::SockAddr::SockAddr(
	const	Uint16		pPort ) :

	addr	( IPAddr::getAnyLocal() ),
	port	( pPort ) {

}

Net::SockAddr::SockAddr(
		IPAddrCPtr	pAddr,
	const	Uint16		pPort ) :

	addr	( pAddr ? pAddr : IPAddr::getAnyLocal() ),
	port	( pPort ) {

}

Net::SockAddr::SockAddr(
	const	Buffer &	pHost,
	const	Uint16		pPort,
	const	Bool		pResolve ) :

	port	( pPort ) {

	if ( pResolve ) {
		try {
			addr = IPAddr::getByName( pHost );
		}
		catch ( Exception & ) {
			addr.kill();
		}
	}

	if ( ! addr ) {
		host = pHost;
	}

}

Net::SockAddr::SockAddr(
	const	SockAddr &	pOther ) :

	SObject		(),
	Comparable	(),
	Address		(),
	host		( pOther.host ),
	addr		( pOther.addr ),
	port		( pOther.port ) {

}

// ============================================================================

Net::SockAddr & Net::SockAddr::operator = (
	const	SockAddr &	pOther ) {

	if ( this != &pOther ) {
		addr = pOther.addr;
		port = pOther.port;
		host = pOther.host;
	}

	return *this;

}

// ============================================================================

Bool Net::SockAddr::operator == (
	const	SockAddr &	pOther ) const {

	return ( port == pOther.port
	      && ( ( addr && addr->equals( pOther.addr ) )
	        || ( host.hasContent() && host == pOther.host )
	        || ( ! pOther.addr && pOther.host.isEmpty() ) ) );

}

Int32 Net::SockAddr::compareTo(
		SCPtr		pOther ) const {

	SockAddrCPtr	other = pOther;

	if ( addr ) {
		Int32 res = ( addr->compareTo( other->addr ) );
		if ( res ) {
			return res;
		}
	}

	if ( host.hasContent() ) {
		Int32 res = ( host.compare( other->host ) );
		if ( res ) {
			return res;
		}
	}

	return ( port < other->port ? -1 : port > other->port ? 1 : 0 );

}

// ============================================================================

Buffer Net::SockAddr::getHost(
	const	Bool		pReverse ) const {

	if ( host.hasContent() ) {
		return host;
	}

	if ( addr ) {
		return ( pReverse || addr->hasHostName()
			? addr->getName()
			: addr->getAddr() );
	}

	return host;

}

Buffer Net::SockAddr::toBuffer(
	const	Bool		pReverse ) const {

	return ( isUnresolved()
		? host + ":" + Buffer( port )
		: pReverse || addr->hasHostName()
		? addr->toBuffer() + ":" + Buffer( port )
		: addr->getAddr() + ":" + Buffer( port ) );

}

// ============================================================================

