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
//	BFC.Net.SocketData.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <errno.h>
#include <string.h>
#if defined( PLATFORM_WIN32 )
#include <windows.h>
#else
#include <netinet/in.h>
#endif

#include "BFC.IO.WouldBlock.h"

#include "BFC.Net.IPv4Addr.h"
#include "BFC.Net.SockAddr.h"
#include "BFC.Net.SocketData.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_WIN32 )

const Net::SocketType Net::InvSocket		= INVALID_SOCKET;
const Net::SocketType Net::SocketError		= SOCKET_ERROR;
const Net::SocketType Net::SocketIntr		= WSAEINTR;
// TODO: WSAEPIPE does not exist, is ERROR_BROKEN_PIPE correct???
//const Net::SocketType Net::CodeBrokenPipe	= WSAEPIPE;
const Net::SocketType Net::CodeAgain		= WSAEWOULDBLOCK;
const Net::SocketType Net::CodeBrokenPipe	= ERROR_BROKEN_PIPE;
const Net::SocketType Net::CodeConnReset	= WSAECONNRESET;
const Net::SocketType Net::CodeInProgress	= WSAEINPROGRESS;
const Net::SocketType Net::CodeWouldBlock	= WSAEWOULDBLOCK;

#else

const Net::SocketType Net::InvSocket		= -1;
const Net::SocketType Net::SocketError		= -1;
const Net::SocketType Net::SocketIntr		= EINTR;
const Net::SocketType Net::CodeAgain		= EAGAIN;
const Net::SocketType Net::CodeBrokenPipe	= EPIPE;
const Net::SocketType Net::CodeConnReset	= ECONNRESET;
const Net::SocketType Net::CodeInProgress	= EINPROGRESS;
const Net::SocketType Net::CodeWouldBlock	= EWOULDBLOCK;

#endif

// ============================================================================

void Net::SocketData::openSD(
	const	Mode		mode ) {

	desc = ::socket(
		AF_INET,
		( mode == DGram ? SOCK_DGRAM : SOCK_STREAM ),
		0 );

}

void Net::SocketData::closeSD() {

#if defined( PLATFORM_WIN32 )
	::closesocket( desc );
#else
	::close( desc );
#endif
	desc = InvSocket;

}

// ============================================================================

void Net::SocketData::bindSD(
		SockAddrCPtr	pSockAddr ) {

	Uint16		pPort = pSockAddr->getPort();
	IPAddrCPtr	pAddr = pSockAddr->getAddr();

	if ( pAddr->isIPv4() ) {

		sockaddr_in	addr;
		SocketLenType	slen = sizeof( addr );
	
		addr.sin_family		= AF_INET;
		addr.sin_port		= htons( pPort );
		addr.sin_addr.s_addr	= IPv4AddrCPtr( pAddr )->getUint32();
	
		if ( ::bind( desc, ( sockaddr * )&addr, slen ) == SocketError ) {
			throw InternalError(
				"Can't bind "
				+ pAddr->toBuffer() + ":" + Buffer( pPort )
				+ ": "
				+ ::strerror( getErrno() ) );
		}

	}

	else if ( pAddr->isIPv6() ) {
		throw NotImplemented();
	}

	else {
		throw InternalError();
	}

}

void Net::SocketData::connectSD(
		SockAddrCPtr	pSockAddr ) {

	Uint16		pPort = pSockAddr->getPort();
	IPAddrCPtr	pAddr = pSockAddr->getAddr();

	if ( pAddr->isIPv4() ) {

		sockaddr_in	addr;
		SocketLenType	slen = sizeof( addr );
	
		addr.sin_family		= AF_INET;
		addr.sin_port		= htons( pPort );
		addr.sin_addr.s_addr	= IPv4AddrCPtr( pAddr )->getUint32();
	
		if ( ::connect( desc, ( sockaddr * )&addr, slen ) == SocketError ) {
			int e = getErrno();
			if ( e == SocketIntr ) {
				throw ThreadStopped();
			}
			if ( e == CodeInProgress ) {
				closeSD();
				throw IO::WouldBlock();
			}
			throw InternalError(
				"Can't connect "
				+ pAddr->toBuffer() + ":" + Buffer( pPort )
				+ ": "
				+ ::strerror( getErrno() ) );
		}

	}

	else if ( pAddr->isIPv6() ) {
		throw NotImplemented();
	}

	else {
		throw InternalError();
	}

}

Net::SockAddrCPtr Net::SocketData::getSockName() {

	sockaddr_in	addr;
	SocketLenType	slen = sizeof( addr );

	if ( ::getsockname(
			desc,
			( sockaddr * )&addr,
			&slen ) == SocketError ) {
		throw InternalError(
			Buffer( "Can't find local addr/port! --> " )
			+ ::strerror( getErrno() ) );
	}

	if ( addr.sin_family == AF_INET ) {
		return new SockAddr(
			new IPv4Addr( ( Uint32 )addr.sin_addr.s_addr ),
			ntohs( addr.sin_port ) );
	}
	else {
		throw NotImplemented();
	}

}

// ============================================================================

