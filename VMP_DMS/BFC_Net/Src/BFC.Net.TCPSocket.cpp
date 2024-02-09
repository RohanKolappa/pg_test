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
//	BFC.Net.TCPSocket.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <winsock2.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h> /* for TCP_NODELAY */
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "BFC.IO.File.h"
#include "BFC.IO.WouldBlock.h"

#include "BFC.Net.IPv4Addr.h"
#include "BFC.Net.Packet.h"
#include "BFC.Net.SignalIgnorer.h"
#include "BFC.Net.SocketData.h"
#include "BFC.Net.TCPSocket.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, TCPSocket )

// ============================================================================

Net::TCPSocket::TCPSocket(
	const	IPv4Address &	pPeerAddr,
	const	Uint16		pPeerPort ) {

	setObjectName( "BFC.Net.TCPSocket" );

	peerSock = new SockAddr( new IPv4Addr( pPeerAddr ), pPeerPort );

}

Net::TCPSocket::TCPSocket(
		SockAddrCPtr	pPeerAddr,
		SockAddrCPtr	pSelfAddr ) {

	setObjectName( "BFC.Net.TCPSocket" );

	peerSock = pPeerAddr;
	selfSock = pSelfAddr;

}

Net::TCPSocket::TCPSocket(
		SocketData *	pSockData,
	const	Uint16		pPeerPort,
	const	IPv4Address &	pPeerAddr,
	const	Uint16		pSelfPort,
	const	IPv4Address &	pSelfAddr ) :

	Socket	( pSockData ) {

	setObjectName( "BFC.Net.TCPSocket" );

	selfSock = new SockAddr( new IPv4Addr( pSelfAddr ), pSelfPort );
	peerSock = new SockAddr( new IPv4Addr( pPeerAddr ), pPeerPort );

	offset = 0;

	setAccess( ReadAccess | WriteAccess );

}

Net::TCPSocket::~TCPSocket() {

	safeClose();

}

// ============================================================================

#if ! defined( PLATFORM_WIN32 )

void Net::TCPSocket::setKernelKeepAliveOptions(
	const	Uint32		probes,
	const	Uint32		time ) {

	const char	*prfilen = "/proc/sys/net/ipv4/tcp_keepalive_probes",
			*tifilen = "/proc/sys/net/ipv4/tcp_keepalive_time";

	if ( ::geteuid() ) {
		throw InternalError( "Must be root!" );
	}

	if ( ! IO::File::exists( prfilen )
	  || ! IO::File::exists( tifilen ) ) {
		throw InternalError( "/proc interface not found!" );
	}

	try {
		Buffer buffer;
		// probes...
		buffer = Buffer( probes );
		IO::File prfile( prfilen, IO::File::Write );
		prfile.open();
		prfile.putBytes( buffer );
		prfile.close();
		// time...
		buffer = Buffer( time );
		IO::File tifile( tifilen, IO::File::Write );
		tifile.open();
		tifile.putBytes( buffer );
		tifile.close();
	}
	catch ( Exception & e ) {
		throw InternalError( e );
	}

}

void Net::TCPSocket::getKernelKeepAliveOptions(
		Uint32 &	probes,
		Uint32 &	time ) {

	const char	*prfilen = "/proc/sys/net/ipv4/tcp_keepalive_probes",
			*tifilen = "/proc/sys/net/ipv4/tcp_keepalive_time";

	if ( ! IO::File::exists( prfilen )
	  || ! IO::File::exists( tifilen ) ) {
		throw InternalError( "/proc interface not found!" );
	}

	try {
		// probes...
		IO::File prfile( prfilen, IO::File::Read );
		prfile.open();
		probes = prfile.getBytes().toUint32();
		prfile.close();
		// time...
		IO::File tifile( tifilen, IO::File::Read );
		tifile.open();
		time = tifile.getBytes().toUint32();
		tifile.close();
	}
	catch ( Exception & e ) {
		throw InternalError( e );
	}

}

#endif

// ============================================================================

void Net::TCPSocket::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! peerSock ) {
		throw InternalError();
	}

	if ( ! peerSock->getAddr()->isIPv4() ) {
		throw InternalError( "Only IPv4!" );
	}

	offset = 0;

	// Let's first create a valid socket.

	getSocketData()->openSD( SocketData::Stream );

	if ( ! getSocketData()->isValid() ) {
		throwSocketException( "create()" );
	}

	if ( hasPutTimeout() ) {
		Time::Delta d = getRelPutTimeout();
		if ( d <= 0 ) {
			throw IO::WouldBlock();
		}
		setTxTimeout( d );
	}

	getSocketData()->connectSD( peerSock );

	selfSock = getSocketData()->getSockName();

	setAccess( ReadAccess | WriteAccess );

}

void Net::TCPSocket::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	getSocketData()->closeSD();

	resetAccess();

}

// ============================================================================

void Net::TCPSocket::setKeepAlive(
#if defined( PLATFORM_WIN32 )
	const	Bool		/* enable */ ) {
#else
	const	Bool		enable ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	// If there are any socket option, ...

	Uint32 optval = ( enable ? 1 : 0 );

	if ( ::setsockopt( getSocketData()->getSD(),
	                 SOL_SOCKET,
	                 SO_KEEPALIVE,
	                 &optval,
	                 sizeof( Uint32 ) ) == -1 ) {
		throwSocketException( "setsockopt( SO_KEEPALIVE )" );
	}

#endif
}

void Net::TCPSocket::setNoDelay(
	const	Bool		enable ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Uint32 optval = ( enable ? 1 : 0 );

	if ( ::setsockopt( getSocketData()->getSD(),
	                 IPPROTO_TCP,
	                 TCP_NODELAY,
#if defined( PLATFORM_WIN32 )
	                 (char*)&optval,
#else
	                 &optval,
#endif
					 sizeof( Uint32 ) ) != 0 ) {
		throwSocketException( "setsockopt( TCP_NODELAY )" );
	}

}

void Net::TCPSocket::setNonBlocking(
#if defined( PLATFORM_WIN32 )
	const	Bool		/* enable */ ) {
#else
	const	Bool		enable ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	int v;

	if ( ( v = ::fcntl( getSocketData()->getSD(), F_GETFL, 0 ) ) < 0 ) {
		throw InternalError( "Can't fcntl( GETFL )!" );
	}

	if ( enable ) {
		v |= O_NONBLOCK;
	}
	else {
		v &= ~O_NONBLOCK;
	}

	if ( ::fcntl( getSocketData()->getSD(), F_SETFL, v ) < 0 ) {
		throw InternalError( "Can't fcntl( SETFL, O_NONBLOCK)!" );
	}
#endif
}

// ============================================================================

Uint16 Net::TCPSocket::getPeerPort() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return peerSock->getPort();

}

Net::IPv4Address Net::TCPSocket::getPeerAddr() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return IPv4Address( IPv4AddrCPtr( peerSock->getAddr() )->getUint32() );

}

Uint16 Net::TCPSocket::getSelfPort() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return selfSock->getPort();

}

Net::IPv4Address Net::TCPSocket::getSelfAddr() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return IPv4Address( IPv4AddrCPtr( selfSock->getAddr() )->getUint32() );

}

// ============================================================================

Bool Net::TCPSocket::hasData() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	SocketType tmp = getSocketData()->getSD();

	if ( tmp == InvSocket ) {
		throw InternalError( "Socket descriptor killed!" );
	}

	fd_set readfds;

	FD_ZERO( &readfds );
	FD_SET( tmp, &readfds );

	int c;

	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	c = ::select(
		(int)tmp + 1,
		&readfds,
		0,
		0,
		&timeout
	);

	if ( c == -1 ) {
		throwSocketException( "select()" );
	}

	return ( c != 0 );

}

Buffer Net::TCPSocket::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	// Wait until ready to read without blocking... Allows Thread to be
	// stopped :-)

	for (;;) {

		SocketType tmp = getSocketData()->getSD();

		if ( tmp == InvSocket ) {
			throw InternalError( "Socket descriptor killed!" );
		}

		fd_set readfds;

		FD_ZERO( &readfds );
		FD_SET( tmp, &readfds );

		Time::Delta	maxWait( 0, 250000 );

		Bool finalTimeout = false;

		if ( hasGetTimeout() ) {
			Time::Delta d = getRelGetTimeout();
			if ( d <= 0 ) {
				throw IO::WouldBlock();
			}
			if ( d < maxWait ) {
				maxWait = d;
				finalTimeout = true;
			}
		}

		struct timeval timeout;

		timeout.tv_sec	= ( long )( Int64 )maxWait / ( Int64 )1000000;
		timeout.tv_usec	= ( long )( Int64 )maxWait % ( Int64 )1000000;

		int c = ::select(
			(int)tmp + 1,
			&readfds,
			0,
			0,
			&timeout
		);

		try {
			checkCode( c );
		}
		catch ( Exception & ) {
			msgErr( "Can't select!" );
			throw;
		}

		if ( c != 0 ) {
			break;
		}

		if ( finalTimeout ) {
			throw IO::WouldBlock();
		}

		// Timeout! Loop :-)

	}

	// Now, read as many bytes as possible...

	const Uint32	toRead = ( 1 << 16 );	// 64 kB
	Uchar		buffer[ toRead ];
	int		count;

	count = ::recv(
		getSocketData()->getSD(),
#if defined( PLATFORM_WIN32 )
		( Char * )buffer,
#else
		( void * )buffer,
#endif
		toRead,
		0 );

	try {
		checkCode( count );
	}
	catch ( Exception & ) {
		msgErr( "Can't recv!" );
		throw;
	}

	if ( ! count ) {
		throw IO::EndOfStream();
	}

	offset += count;

	return Buffer( buffer, ( Uint32 )count, true );

}

void Net::TCPSocket::putBytes(
	const	Buffer &	input ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	// Wait until ready to write without blocking...

	for (;;) {

		SocketType tmp = getSocketData()->getSD();

		if ( tmp == InvSocket ) {
			throw InternalError( "Socket descriptor killed!" );
		}

		fd_set writefds;

		FD_ZERO( &writefds );
		FD_SET( tmp, &writefds );

		Time::Delta	maxWait( 0, 250000 );

		Bool finalTimeout = false;

		if ( hasPutTimeout() ) {
			Time::Delta d = getRelPutTimeout();
			if ( d <= 0 ) {
				throw IO::WouldBlock();
			}
			if ( d < maxWait ) {
				maxWait = d;
				finalTimeout = true;
			}
		}

		struct timeval timeout;

		timeout.tv_sec	= ( long )( Int64 )maxWait / ( Int64 )1000000;
		timeout.tv_usec	= ( long )( Int64 )maxWait % ( Int64 )1000000;

		int c = ::select(
			(int)tmp + 1,
			0,
			&writefds,
			0,
			&timeout
		);

		checkCode( c );

		if ( c != 0 ) {
			break;
		}

		if ( finalTimeout ) {
			throw IO::WouldBlock();
		}

	}

	// Now, write our buffer!

	if ( hasPutTimeout() ) {
		Time::Delta d = getRelPutTimeout();
		if ( d <= 0 ) {
			throw IO::WouldBlock();
		}
		setTxTimeout( d );
	}
	else {
		resetTxTimeout();
	}

	const Uchar *	buffer = input.getBufferAddr();
	const Uint32	towrite = input.length();

	int count;

	{
		SignalIgnorer signalIgnorer;
		count = ::send(
				getSocketData()->getSD(),
				( const Char * )buffer,
				towrite,
				0 );
	}

	checkCode( count );

	if ( ( Uint32 )count != towrite ) {
		safeClose();
		throw IO::BrokenPipe();
	}

	offset += towrite;

}

// ============================================================================

Uint64 Net::TCPSocket::seek(
	const	Uint64		/* newOffset */ ) {

	throw NoRandomAccess();

}

Uint64 Net::TCPSocket::tell() const {

	return offset;

}

Uint64 Net::TCPSocket::length() const {

	return 0;

}

// ============================================================================

Net::PacketPtr Net::TCPSocket::getPacket() {

	return new Packet( getBytes(), peerSock, selfSock );

}

void Net::TCPSocket::putPacket(
		PacketCPtr	pPacket ) {

	putBytes( pPacket->getBuffer() );

}

// ============================================================================

