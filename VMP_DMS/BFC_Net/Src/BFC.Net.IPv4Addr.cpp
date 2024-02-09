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
//	BFC.Net.IPv4Addr.cpp
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
//#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Net.IPv4Addr.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, IPv4Addr )

// ============================================================================

Net::IPv4Addr::IPv4Addr() :

	IPAddr	( IPv4 ),
	address	( 0x00000000 ) {

}

//Net::IPv4Addr::IPv4Addr(
//	const	Buffer &	pHost,
//	const	Buffer &	pAddr ) :
//
//	IPAddr	( IPv4, pHost ),
//	address	( pAddr.getLength() == INADDRSZ
//			? *( const Uint32 * )pAddr.getCstPtr()
//			: 0x00000000 ) {
//
//}

Net::IPv4Addr::IPv4Addr(
	const	Uint32		pAddr ) :

	IPAddr	( IPv4 ),
	address	( pAddr ) {

}

Net::IPv4Addr::IPv4Addr(
	const	Buffer &	pHost,
	const	Uint32		pAddr ) :

	IPAddr	( IPv4, pHost ),
	address	( pAddr ) {

}

Net::IPv4Addr::IPv4Addr(
	const	IPv4Address &	pOther ) :

	IPAddr	( IPv4 ),
	address	( pOther.getUint() ) {

}

// ============================================================================

Bool Net::IPv4Addr::equals(
	const	IPAddrCPtr &	pOther ) const {

	return ( pOther
	      && pOther.isA< IPv4Addr >()
	      && address == IPv4AddrCPtr( pOther )->address );

}

Int32 Net::IPv4Addr::compareTo(
		SCPtr		pOther ) const {

	IPv4AddrCPtr other = pOther;

	return ( address < other->address ? -1 : address > other->address ? 1 : 0 );

}

// ============================================================================

Buffer Net::IPv4Addr::getAddr() const {

	return numericToTextFormat( address );

}

Uint32 Net::IPv4Addr::getUint32() const {

	return address;

}

Buffer Net::IPv4Addr::getRawAddr() const {

	return Buffer( ( const Uchar * )&address, 4, true );

}

Bool Net::IPv4Addr::isMulticast() const {

	return ( ( System::BE2Host( address ) & 0xf0000000 ) == 0xe0000000 );

}

Bool Net::IPv4Addr::isAnyLocal() const {

	return ( address == 0 );

}

Bool Net::IPv4Addr::isLoopback() const {

	return ( *( const Uchar * )&address == 127 );

}

Bool Net::IPv4Addr::isLinkLocal() const {

	// link-local unicast in IPv4 (169.254.0.0/16)
	// defined in "Documenting Special Use IPv4 Address Blocks
	// that have been Registered with IANA" by Bill Manning
	// draft-manning-dsua-06.txt

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] == 169
	      && s[ 1 ] == 254 );

}

Bool Net::IPv4Addr::isSiteLocal() const {

	// refer to RFC 1918
	// 10/8 prefix
	// 172.16/12 prefix
	// 192.168/16 prefix

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] == 10
	      || ( s[ 0 ] == 172 && ( s[ 1 ] & 0xF0 ) == 16 )
	      || ( s[ 0 ] == 192 && s[ 1 ] == 168 ) );

}

Bool Net::IPv4Addr::isMCGlobal() const {

	// 224.0.1.0 to 238.255.255.255

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] >= 224
	      && s[ 0 ] <= 238
	      && ( s[ 0 ] != 224 || s[ 1 ] != 0 || s[ 2 ] != 0 ) );

}

Bool Net::IPv4Addr::isMCNodeLocal() const {

	// unless ttl == 0

	return false;

}

Bool Net::IPv4Addr::isMCLinkLocal() const {

	// 224.0.0/24 prefix and ttl == 1

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] == 224
	      && s[ 1 ] == 0
	      && s[ 2 ] == 0 );

}

Bool Net::IPv4Addr::isMCSiteLocal() const {

	// 239.255/16 prefix or ttl < 32

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] == 239
	      && s[ 1 ] == 255 );

}

Bool Net::IPv4Addr::isMCOrgLocal() const {

	// 239.192 - 239.195

	const Uchar * s = ( const Uchar * )&address;

	return ( s[ 0 ] == 239
	      && s[ 1 ] >= 192
	      && s[ 1 ] <= 195 );

}

// ============================================================================

Uint32 Net::IPv4Addr::convertDotToRaw(
	const	Buffer &	pAddr ) {

	Buffer addr = pAddr;

	in_addr	raw;

#if defined( PLATFORM_WIN32 )
	if ( ( raw.s_addr = ::inet_addr( ( const char * )addr.getCStringPtr() ) )
				== INADDR_NONE ) {
#else
	if ( ::inet_aton( ( const char * )addr.getCStringPtr(), &raw ) == 0 ) {
#endif
		throw InvalidArgument();
	}

	return ( Uint32 )raw.s_addr;

}

// ============================================================================

Buffer Net::IPv4Addr::lookupName() const {

	Buffer host = rslvRawToName( getRawAddr() );

	IPAddrCArray arr = rslvNameToAddr( host );

	for ( Uint32 i = 0 ; i < arr.getSize() ; i++ ) {
		if ( equals( arr[ i ] ) ) {
			return host;
		}
	}

	return getAddr();

}

Buffer Net::IPv4Addr::numericToTextFormat(
	const	Uint32		src ) {

	const Uchar * s = ( const Uchar * )&src;

	return Buffer( ( Uint32 )s[0] )
		+ "."
		+ Buffer( ( Uint32 )s[1] )
		+ "."
		+ Buffer( ( Uint32 )s[2] )
		+ "."
		+ Buffer( ( Uint32 )s[3] );

}

// ============================================================================

