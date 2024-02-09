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
//	BFC.Net.TCPSocketProvider.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <string.h>
#if defined( PLATFORM_WIN32 )
#include <windows.h>
#include "BFC.Net.DllInitializer.h"
#else
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "BFC.Base.WouldBlock.h"

#include "BFC.Debug.ThreadFinder.h"

#include "BFC.Math.Utils.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.SocketData.h"
#include "BFC.Net.TCPSocketProvider.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, TCPSocketProvider )

// ============================================================================

Net::TCPSocketProvider::TCPSocketProvider() {

#if defined( PLATFORM_WIN32 )
	DllInitializer::instance();
#endif

	data	= new SocketData;
	port	= ( Uint16 )-1;

}

Net::TCPSocketProvider::~TCPSocketProvider() {

	try {
		if ( isInitialized() ) {
			kill();
		}
	}
	catch ( Exception & ) {
	}

	delete data;

}

// ============================================================================

void Net::TCPSocketProvider::init(
	const	Uint16		pPort,
	const	Uint32		pBack,
	const	Uint32		pOpts) {

	init( IPv4Address( "0.0.0.0" ), pPort, pBack, pOpts );

}

void Net::TCPSocketProvider::init(
	const	IPv4Address &	pAddr,
	const	Uint16		pPort,
	const	Uint32		pBack,
	const	Uint32		pOpts) {

	if ( isInitialized() ) {
		throw AlreadyInitialized();
	}

	if ( ! pBack ) {
		throw NullArgument();
	}

	// Let's first create a valid socket.

	data->openSD( SocketData::Stream );

	if ( ! data->isValid() ) {
		throw CantSystemCreate( ::strerror( getErrno() ) );
	}

#if defined( PLATFORM_WIN32 )

	if ( pOpts ) {		// keep compiler happy :-)
	}

#else

	// If we have to use any particular socket option, we do!

	if ( ( pOpts & ReuseAddr ) ) {
		Uint32 optval = 1;
		if ( ::setsockopt( data->getSD(),
		                 SOL_SOCKET,
		                 SO_REUSEADDR,
		                 &optval,
		                 sizeof(Uint32)) == -1)
			throw CantSystemSetSockOpt( ::strerror( getErrno() ) );
	}

#endif

	// Let's fill our `registration card'.

	sockaddr_in myName;

	myName.sin_family      = AF_INET;
	myName.sin_port        = htons( pPort );
	myName.sin_addr.s_addr = pAddr.getUint();

	if ( ::bind( data->getSD(), ( sockaddr * )&myName, sizeof( sockaddr_in ) )
			== SocketError ) {
		int e = getErrno();
		data->closeSD();
		throw CantSystemBind( ::strerror( e ) );
	}

	if ( ::listen( data->getSD(), pBack ) == SocketError ) {
		int e = getErrno();
		data->closeSD();
		throw CantSystemListen( ::strerror( e ) );
	}

	sockaddr_in	self;
	SocketLenType	structlen = sizeof( sockaddr_in );

	if ( ::getsockname( data->getSD(), ( sockaddr * )&self, &structlen ) == 0 ) {
		port = ntohs( self.sin_port );
	}

}

// ============================================================================

Bool Net::TCPSocketProvider::isInitialized() const {

	return ( data->isValid() );

}

// ============================================================================

void Net::TCPSocketProvider::kill() {

	try {
		if ( isInitialized() ) {
			data->closeSD();
		}
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

Net::TCPSocketPtr Net::TCPSocketProvider::accept(
	const	Time::Delta &	pWaitTime ) {

	if ( ! isInitialized() ) {
		throw NotInitialized();
	}

// #if defined( PLATFORM_WIN32 )

	// Wait until ready to accept a new connection without blocking...

	Time::Clock	waitStop = Time::now() + pWaitTime;

	for (;;) {

//		msgDbg( "TCPSocketProvider::accepting..." );

		SocketType	tmp = data->getSD();

		if ( tmp == InvSocket ) {
			throw SocketKilled();
		}

		fd_set readfds;

		FD_ZERO( &readfds );
		FD_SET( tmp, &readfds );

		Time::Delta	currWait( 1, 0 );

		if ( pWaitTime ) {
			currWait = Math::getMin( currWait, waitStop - Time::now() );
			currWait = Math::getMax( currWait, Time::Delta( 0, 1 ) );
		}

		struct timeval timeout;

		timeout.tv_sec = ( Int64 )currWait / 1000000;
		timeout.tv_usec = ( Int64 )currWait % 1000000;

		int c = ::select(
			(int)tmp + 1,	// ignored anyway!
			&readfds,
			0,
			0,
			&timeout
		);

		if ( DObject::getThreadFinder()->selfShouldStop() ) {
			throw ThreadStopped();
		}

		if ( c == InvSocket && getErrno() == SocketIntr ) {
			// ignore EINT when not thread stopped
			continue;
		}

		if ( c == InvSocket ) {
			throw CantSystemSelect( ::strerror( getErrno() ) );
		}

		if ( c == 0 ) {
			// Timeout!
			if ( pWaitTime && Time::now() >= waitStop ) {
				throw WouldBlock();
			}
			continue;
		}

		break;

	}

// #endif

	// OK. Let's accept a new connection.

	SocketType	newsd;
	sockaddr_in	peer;
	SocketLenType	structlen = sizeof( sockaddr_in );

	// loop until not interrupted
	for (;;) {
		newsd = ::accept( data->getSD(), ( sockaddr * )&peer, &structlen );
		if ( newsd == InvSocket ) {
			int e = getErrno();
			if ( e == SocketIntr ) {
				if ( DObject::getThreadFinder()->selfShouldStop() ) {
					throw ThreadStopped();
				}
				continue;
			}
			throw CantSystemAccept( ::strerror( e ) );
		}
		break;
	}

	//if ( ( newsd = ::accept( data->getSD(), ( sockaddr * )&peer, &structlen ) )
	//		== InvSocket ) {
	//	int e = getErrno();
	//	if ( e == SocketIntr ) {
	//		throw ThreadStopped();
	//	}
	//	throw CantSystemAccept( ::strerror( e ) );
	//}

	SocketData * newData = new SocketData( newsd );

	sockaddr_in self;

	structlen = sizeof( sockaddr_in );

	if ( ::getsockname( newData->getSD(), (struct sockaddr *)&self, &structlen)
			== SocketError ) {
		newData->closeSD();
		delete newData;
		throw CantSystemGetSockName( ::strerror( getErrno() ) );
	}

	TCPSocketPtr newSock = new TCPSocket(
			newData, // TCPSocket takes ownership!
			ntohs( peer.sin_port ),
			IPv4Address( ( Uint32 )peer.sin_addr.s_addr ),
			ntohs( self.sin_port ),
			IPv4Address( ( Uint32 )self.sin_addr.s_addr ) );

	return newSock;

}

//void Net::TCPSocketProvider::myClose( SocketData * data ) {
//
//#if defined( PLATFORM_WIN32 )
//	if (::closesocket( data->sd ) == SOCKET_ERROR)
//#else
//	if (::close( data->sd ) == -1)
//#endif
//		throw CantSystemClose(strerror(getErrno()));
//
//}

// ============================================================================




