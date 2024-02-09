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
//	BFC.Net.Socket.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_WIN32 )
#include <windows.h>
#else
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#endif

#if defined( PLATFORM_DARWIN )
#include <sys/sysctl.h> 
#include <net/if_dl.h> 
#include <arpa/inet.h> 
#include <errno.h> 
#include <stdio.h> 
#include <stdlib.h>
#endif

#include "BFC.Debug.ThreadFinder.h"

#include "BFC.IO.WouldBlock.h"

#include "BFC.Net.SDAutoCloser.h"
#include "BFC.Net.Socket.h"
#include "BFC.Net.SocketData.h"
#include "BFC.Net.SocketException.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, Socket )

// ============================================================================

Net::Socket::Socket(
		SocketData *	pSocketData ) {

	ld = ( pSocketData ? pSocketData : new SocketData );

}

Net::Socket::~Socket() {

	delete ld;

}

// ============================================================================

#if defined( PLATFORM_WIN32 )

Bool Net::Socket::initWinsock() {

	if ( initialized ) {
		return true;
	}

	WORD	wVersionRequested;
	WSADATA	wsaData;
	int	err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, & wsaData );

	if ( err ) {
		return false;
	}

	/* Confirm that the Window Socket DLL supports 1.1.	*/
	/* Note that if the DLL supports versions greater	*/
	/* than 1.1 in addition to 1.1, it will still return	*/
	/* 1.1 in wVersion since that is the version we		*/
	/* requested.						*/

	if ( LOBYTE( wsaData.wVersion ) != 1
	  || HIBYTE( wsaData.wVersion ) != 1 ) {
		WSACleanup();
		return false;
	}

	/* The Windows Socket DLL is acceptable. Proceed.	*/

	/* Since we only support 1.1, set both wVersion and	*/
	/* wHighVersion to 1.1.					*/

	wsaData.wVersion     = MAKEWORD( 1, 1 );
	wsaData.wHighVersion = MAKEWORD( 1, 1 );

	initialized = true;

	return true;

}

void Net::Socket::exitWinsock() {

	if ( ! initialized ) {
		return;
	}

	WSACleanup();

	initialized = false;

}

// ============================================================================

Bool Net::Socket::initialized = false;

// ============================================================================

#else

BufferArray Net::Socket::getDevices() {

	int sd = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );

	if ( sd < 0 ) {
		throwSocketException( "create()" );
	}

	SDAutoCloser autoCloser( sd );

	char buf[ 1024 ];

	::memset( buf, 0, sizeof( buf ) );

	struct ifconf 	ifc;

	ifc.ifc_len = sizeof( buf );
	ifc.ifc_buf = buf;

	if ( ::ioctl( sd, SIOCGIFCONF, (char *)&ifc ) < 0 ) {
		throwSocketException( "ioctl( SIOCGIFCONF )" );
	}

	BufferArray	res;
	Uint32		c = ifc.ifc_len / sizeof( struct ifreq );
	struct ifreq	*p = (struct ifreq *)ifc.ifc_buf;

	while ( c-- ) {
		res += Buffer( p->ifr_name );
		p++;
	}

	return res;

}

Net::MACAddress Net::Socket::getMACAddress(
	const	Buffer	&	deviceName ) {

	if ( deviceName.isEmpty() ) {
		throw InvalidArgument();
	}

	if ( deviceName.getLength() >= IFNAMSIZ ) {
		throw InvalidArgument();
	}

#if defined( PLATFORM_DARWIN )

	int	 mib[6];
	size_t len;
	char	 *buf; 
	unsigned char	 *ptr; 
	struct if_msghdr	*ifm; 
	struct sockaddr_dl	*sdl; 
	mib[0] = CTL_NET; 
	mib[1] = AF_ROUTE; 
	mib[2] = 0; 
	mib[3] = AF_LINK; 
	mib[4] = NET_RT_IFLIST; 
	if ((mib[5] = if_nametoindex((const char*)deviceName.getCstPtr())) == 0) { 
		throwSocketException( "if_nametoindex()" );
	}
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) { 
		throwSocketException( "sysctl()" );
	}
	if ((buf = (char*)malloc(len)) == NULL) { 
		throw InternalError();
	} 
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) { 
		throwSocketException( "sysctl()" );
	} 
	ifm = (struct if_msghdr *)buf; 
	sdl = (struct sockaddr_dl *)(ifm + 1); 
	ptr = (unsigned char *)LLADDR(sdl); 	
	Buffer raw( ptr, 6, true );
	
#else

	int s;

	if ( ( s = ::socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 ) {
		throwSocketException( "create()" );
	}

	SDAutoCloser autoCloser( s );

	struct ifreq ifr;

	deviceName.dumpToBuffer((uchar *)ifr.ifr_name, IFNAMSIZ);

	if (::ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
		throwSocketException( "ioctl( SIOCGIFHWADDR )" );
	}

	Buffer raw( ( const uchar * )ifr.ifr_hwaddr.sa_data, 6, true );

#endif	

	MACAddress res;

	res.buildFromRawBuffer(raw);

	return res;

}

Net::IPv4Address Net::Socket::getIPAddress(
	const Buffer&	deviceName) {

	if ( deviceName.isEmpty() ) {
		throw InvalidArgument();
	}

	if ( deviceName.getLength() >= IFNAMSIZ ) {
		throw InvalidArgument();
	}

	int s;

	if ((s = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		throwSocketException( "create()" );
	}

	SDAutoCloser autoCloser( s );

	struct ifreq ifr;

	deviceName.dumpToBuffer((uchar *)ifr.ifr_name, IFNAMSIZ);

	if (::ioctl(s, SIOCGIFADDR, &ifr) < 0) {
		throwSocketException( "ioctl( SIOCGIFADDR )" );
	}

	struct in_addr	addr;	// addr.s_addr is uint32_t

	union {
		const Uint8 *	p1;
		const Uint32 *	p4;
	} p;

	p.p1 = ( const Uint8 * )ifr.ifr_addr.sa_data + 2;

	addr.s_addr = *p.p4;

	IPv4Address res((Uint32)addr.s_addr);

	return res;

}

#endif

// ============================================================================

int Net::Socket::getSD() const {

	if ( ! ld ) {
		throw InternalError();
	}

	return ( int )ld->getSD();

}

// ============================================================================

void Net::Socket::setRxTimeout(
	const	Time::Delta &	pDelta ) {

	struct timeval optval;

	optval.tv_sec	= ( long )( Int64 )pDelta / ( Int64 )1000000;
	optval.tv_usec	= ( long )( Int64 )pDelta % ( Int64 )1000000;

	if ( ::setsockopt(
			ld->getSD(),
			SOL_SOCKET,
			SO_RCVTIMEO,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		throwSocketException( "setsockopt( SO_RCVTIMEO )" );
	}

}

void Net::Socket::setTxTimeout(
	const	Time::Delta &	pDelta ) {

	struct timeval optval;

	optval.tv_sec	= ( long )( Int64 )pDelta / ( Int64 )1000000;
	optval.tv_usec	= ( long )( Int64 )pDelta % ( Int64 )1000000;

	if ( ::setsockopt(
			ld->getSD(),
			SOL_SOCKET,
			SO_SNDTIMEO,
			( const char * )&optval,
			sizeof( optval ) ) == SocketError ) {
		throwSocketException( "setsockopt( SO_SNDTIMEO )" );
	}

}

// ============================================================================

void Net::Socket::checkCode(
	const	Int32		pCode ) {

	if ( pCode >= 0 ) {
		return;
	}

	if ( pCode != SocketError ) {
		throw InternalError( "Invalid error code!" );
	}

	int e = getErrno();

	if ( e == CodeAgain || e == CodeWouldBlock ) {
		throw IO::WouldBlock();
	}

	if ( e == SocketIntr ) {
		if ( DObject::getThreadFinder()->selfShouldStop() ) {
			safeClose();
			throw ThreadStopped();
		}
//		throw InternalError( "Spurious EINTR!" );
		return;
	}

	safeClose();

	throw IO::BrokenPipe( ::strerror( e ) );

}

void Net::Socket::throwSocketException(
	const	Buffer &	pPrefix,
	const	Bool		pStrErrno ) {

	if ( pStrErrno ) {
		throw SocketException( pPrefix + ": " + ::strerror( errno ) );
	}
	else {
		throw SocketException( pPrefix );
	}

}

// ============================================================================

