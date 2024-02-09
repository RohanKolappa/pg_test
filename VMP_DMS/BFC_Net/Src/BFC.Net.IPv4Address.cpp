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
//	BFC.Net.IPv4Address.cpp
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
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Net.IPv4Address.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Net;

// ============================================================================

IPv4Address::IPv4Address() {

}

IPv4Address::IPv4Address(
	const	IPv4Address &	o) {

	addr    = o.addr;
	fqn     = o.fqn;

}

IPv4Address::IPv4Address(
	const	Char *		addr,
	const	Bool		tryResolve) {

	*this = IPv4Address( Buffer( addr, true ), tryResolve);

}

IPv4Address::IPv4Address(
	const	Buffer &	address,
	const	Bool		tryResolve) {

	if ( address.isEmpty() ) {
		throw InvalidArgument();
	}

//	if ( address == LOCALHOST ) {
//		initWithSelf();
//		return;
//	}

	hostent * host = NULL;
	hostent	copy;

	Buffer name = address;

	int c = (int)name[0];

	if (isdigit(c)) {
		in_addr	tmp_addr;
#if defined( PLATFORM_WIN32 )
		if ( ( tmp_addr.s_addr = inet_addr( name.getCStringPtr() ) )
				== INADDR_NONE ) {
#else
		if ( inet_aton( name.getCStringPtr(), &tmp_addr )
				== 0 ) {
#endif
			throw InvalidArgument();
		}
		addr = tmp_addr.s_addr;
		if ( ! tryResolve ) {
			return;
		}
		FastCSLocker l( resolverMutex );
		host = gethostbyaddr((char *)&tmp_addr.s_addr, 4, AF_INET);
		if ( host == NULL ) {
			return;
		}
		::memcpy( &copy, host, sizeof( copy ) );
	}
	else if (isalpha(c)) {
		FastCSLocker l( resolverMutex );
		host = gethostbyname( name.getCStringPtr() );
		if ( host == NULL ) {
			throw InvalidArgument( "Can't gethostbyname: " + address );
		}
		::memcpy( &copy, host, sizeof( copy ) );
	}
	else {
		throw InvalidArgument();
	}

	// OK. We got our Host struct, copied to `copy'! (libc NOT reentrant!)

	if (copy.h_addrtype != AF_INET || copy.h_length != 4) {
		// ouch. Not an IPv4 ???
		throw InvalidArgument( "Not IPv4!" );
	}

	addr = *(Uint32 *)(copy.h_addr_list[0]);
	if (isdigit(c))
		fqn = copy.h_name;
	else
		fqn = address;

}

IPv4Address::IPv4Address(
	const	Uint32		Address) {

	addr = Address;

	hostent *host;

	FastCSLocker l(resolverMutex);

	host = gethostbyaddr((char *)&addr, 4, AF_INET);

	if (host == NULL) {
		return;
	}

	hostent copy;
	memcpy(&copy, host, sizeof(copy));

	// OK. We got our Host struct, copied to `copy'! (libc NOT reentrant!)

	if (copy.h_addrtype != AF_INET || copy.h_length != 4) {
		// ouch. Not an IPv4 ???
		throw InvalidArgument( "No IPv4!" );
	}

	fqn = copy.h_name;

}

IPv4Address& IPv4Address::operator = (
	const	IPv4Address&		o) {

	if (this != &o ) {
		addr    = o.addr;
		fqn     = o.fqn;
	}

	return *this;

}

void IPv4Address::initWithSelf() {

	char		name[ 256 ];
	hostent *	host;

	// we try to obtain our own ip address

#if defined( PLATFORM_WIN32 )
	if ( gethostname( name, 256-1 ) == SOCKET_ERROR ) {
#else
	if ( gethostname( name, 256-1 ) == -1 ) {
#endif
		throw InvalidArgument( "Can't gethostname(): " + Buffer( strerror( errno ) ) );
	}

	FastCSLocker l( resolverMutex );

	host = ::gethostbyname( name );

	if ( host == NULL ) {
		throw InvalidArgument( "Can't gethostbyname(): " + Buffer( name ) );
	}

	hostent copy;
	memcpy( &copy, host, sizeof( copy ) );

	// OK. We got it.

	if ( copy.h_addrtype != AF_INET || copy.h_length != 4 ) {
		throw InvalidArgument( "No IPv4!" );
	}

	addr	= *(Uint32 *)(copy.h_addr_list[0]);
	fqn	= name;

}

// ============================================================================

Bool IPv4Address::isMulticast() const {

	return ( ( addr & 0x000000F0 ) == 0x000000E0 );

}

Bool IPv4Address::hasFQN() const {

	return ( fqn.hasContent() );

}

Buffer IPv4Address::getFQN() const {

	return fqn;

}

Buffer IPv4Address::getDot() const {

	struct in_addr tmp;

	tmp.s_addr = addr;

	return inet_ntoa(tmp);

}

// ============================================================================

Buffer IPv4Address::toBuffer() const {

	Buffer res;

	if ( hasFQN() ) {
		res = getFQN();
	}
	else {
		res = getDot();
	}

	res += " (" + getDot() + ")";

	return res;

}

// ============================================================================

