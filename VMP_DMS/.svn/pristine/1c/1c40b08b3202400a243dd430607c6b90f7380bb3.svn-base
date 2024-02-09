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
//	BFC.Net.IPAddr.cpp
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
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Net.IPAddr.h"
#include "BFC.Net.IPAddrUtil.h"
#include "BFC.Net.IPv4Addr.h"
#include "BFC.Net.IPv4Impl.h"
#include "BFC.Net.IPv6Addr.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, IPAddr )

// ============================================================================

Net::IPAddrCPtr Net::IPAddr::createNew(
	const	Buffer &	addr ) {

	return createNew( Buffer(), addr );

}

Net::IPAddrCPtr Net::IPAddr::createNew(
	const	Buffer &	pHost,
	const	Buffer &	pAddr ) {

	Buffer	host = pHost;
	Buffer	addr = pAddr;

//	if ( host.hasContent()
//	  && host[ 0 ] == '['
//	  && host.lastChar() == ']' ) {
//		host = host( 1, host.getLength() - 2 );
//	}

	if ( addr.getLength() == IPv4Addr::INADDRSZ ) {
		return new IPv4Addr( host, *( const Uint32 * )addr.getCstPtr() );
	}

//	if ( pAddr.getLength() == IPv6Addr::INADDRSZ ) {
//		Buffer newAddr = IPAddressUtil::convertFromIPv4MappedAddress( addr );
//		if ( newAddr.hasContent() ) {
//			return new IPv4Addr(host, newAddr);
//		}
//		else {
//			return new IPv6Addr(host, pAddr);
//		}
//	}

	throw InvalidArgument();

}

Net::IPAddrPtr Net::IPAddr::createNew(
	const	sockaddr *	pSockAddr ) {

	if ( ! pSockAddr ) {
		return 0;
	}

	if ( pSockAddr->sa_family == AF_INET ) {
		const sockaddr_in * addr = ( const sockaddr_in * )pSockAddr;
		return new IPv4Addr( ( Uint32 )( addr->sin_addr.s_addr ) );
	}

	return 0;

}

// ============================================================================

Buffer Net::IPAddr::toBuffer() const {

	return ( name.hasContent() ? name + "/" : Buffer( "" ) )
		+ getAddr();

}

// ============================================================================

Net::IPAddrCPtr Net::IPAddr::getAnyLocal() {

	return getImpl()->getAnyLocal();

}

Net::IPAddrCPtr Net::IPAddr::getLoopback() {

	return getImpl()->getLoopback();

}

// ============================================================================

Buffer Net::IPAddr::getHostName() {

	char buf[ 256 ];

#if defined( PLATFORM_WIN32 )
	if ( ::gethostname( buf, 255 ) == SOCKET_ERROR ) {
#else
	if ( ::gethostname( buf, 255 ) == -1 ) {
#endif
		throw InternalError( ::strerror( errno ) );
	}

	FastCSLocker l( lock );

	hostent * hst1 = ::gethostbyname( buf );

	if ( ! hst1 || ! hst1->h_addr_list || ! hst1->h_addr_list[ 0 ] ) {
		throw InternalError( "Can't get host by name (host: \""
			+ Buffer( buf )
			+ "\")!" );
	}

	hostent * hst2 = ::gethostbyaddr( hst1->h_addr_list[ 0 ], hst1->h_length, hst1->h_addrtype );

	if ( ! hst2 ) {
		throw InternalError( "Can't get host by addr!" );
	}

	// If gethostbyaddr() found a fully qualified host name, returns that
	// name. Otherwise, returns the hostname found by gethostname().

	Buffer	buf1( ( const Char * )buf, true );
	Buffer	buf2( ( const Char * )hst2->h_name, true );

	return ( buf2.beginsWith( buf1 + "." ) ? buf2 : buf1 );

}

Net::IPAddrCPtr Net::IPAddr::getLocalhost() {

	Buffer local = getHostName();

	if ( local == "localhost" ) {
		return getLoopback();
	}

	return rslvNameToAddr( local, 0 ).getElt( 0 );

}

// ============================================================================

Buffer Net::IPAddr::rslvRawToName(
	const	Buffer &	pRawAddr ) {

	Uint32	af;
	Uint32	len = pRawAddr.getLength();

	if ( len == 4 ) {
		af = AF_INET;
	}
#if ! defined( PLATFORM_WIN32 )
	else if ( len == 16 ) {
		af = AF_INET6;
	}
#endif
	else {
		throw InvalidArgument( "Length == " + Buffer( len ) );
	}

	FastCSLocker l( lock );

	hostent * hp = ::gethostbyaddr( (const char*) pRawAddr.getCstPtr(), len, af );

	if ( ! hp ) {
		throw InternalError( "Can't get host by addr!" );
	}

	return Buffer( ( const Char * )hp->h_name, true );

}

Net::IPAddrCPtr Net::IPAddr::getByName(
	const	Buffer &	pHost ) {

	return rslvNameToAddr( pHost )[ 0 ];

}

Net::IPAddrCArray Net::IPAddr::rslvNameToAddr(
	const	Buffer &	hostname,
		IPAddrCPtr	reqAddr ) {

	IPAddrCArray res;

	if ( hostname.isEmpty() ) {
		msgWrn( "rslvNameToAddr(): empty argument!" );
		res += getLoopback();
		return res;
	}

	Buffer tmp = hostname;

	FastCSLocker l( lock );

	hostent * hp = ::gethostbyname( tmp.getCStringPtr() );

	if ( ! hp ) {
		throw InvalidArgument( "No such host!" );
	}

	if ( hp->h_addrtype == AF_INET ) {
		for ( in_addr ** p = ( in_addr ** )hp->h_addr_list ; *p ; p++ ) {
			res += new IPv4Addr(
				hostname,
				( Uint32 )(*p)->s_addr );
//			msgDbg( "rslvNameToAddr(): \""
//				+ hostname + "\" --> \""
//				+ res.getLast()->getAddr() + "\"..." );
		}
		if ( res.isEmpty() ) {
			throw InvalidArgument( "No address found for \""
				+ hostname + "\"!" );
		}
	}
#if ! defined( PLATFORM_WIN32 )
	else if ( hp->h_addrtype == AF_INET6 ) {
		throw NotImplemented();
	}
#endif
	else {
		throw InternalError();
	}

	if ( reqAddr && res.getSize() > 1 && ! res[ 0 ]->equals( reqAddr ) ) {
		Uint32 i = 1;
		for ( ; i < res.getSize() ; i++ ) {
			if ( res[ i ]->equals( reqAddr ) ) {
				res.pop( i );
				res.insert( 0, reqAddr );
				break;
			}
		}
	}

	return res;

}

// ============================================================================

Uint32 Net::IPAddr::checkNumericZone(
	const	Buffer &	s ) {

	Uint32	percent = s.findFwd( '%' );

	if ( percent == s.getLength() ) {
		return ( Uint32 )-1;
	}

	Uint32	zone = 0;

	for ( Uint32 i = percent + 1 ; i < s.getLength() ; i++ ) {
		Uchar c = s[ i ];
		if ( c == ']' ) {
			if ( i == percent + 1 ) {
				/* empty per-cent field */
				return ( Uint32 )-1;
			}
			break;
		}
		Uint32 digit;
		if ( ( digit = digitToVal( c ) ) == ( Uint32 )-1 ) {
			return ( Uint32 )-1;
		}
		zone = ( zone * 10 ) + digit;
	}

	return zone;

}

// ============================================================================

FastCS Net::IPAddr::lock;
Net::IPImplPtr Net::IPAddr::impl = Net::IPv4Impl::instance();

// ============================================================================

