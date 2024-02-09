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
//	BFC.Net.HttpCookie.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Net.HttpCookie.h"

#include "BFC.TL.BufferArray.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, HttpCookie )

// ============================================================================

Net::HttpCookie::HttpCookie(
	const	Buffer &	pName,
	const	Buffer &	pValue ) :

	name		( pName.trim() ),
	value		( pValue ),
	toDiscard	( false ),
	maxAge		( MAX_AGE_UNSPECIFIED ),
	secure		( false ),
	version		( 1 ),
	whenCreated	( Time::now() ) {

	if ( name.isEmpty() || ! isToken( name ) || isReserved( name ) ) {
		throw InvalidArgument( "Illegal cookie name" );
	}

}

// ============================================================================

Bool Net::HttpCookie::domainMatches(
	const	Buffer &	domain,
	const	Buffer &	host ) {

	if ( domain.isEmpty() || host.isEmpty() ) {
		return false;
	}

	Bool isLocalDomain = ( domain.toLower() == ".local" );
	Uint32 embeddedDotInDomain = domain.findFwd('.');
	if (embeddedDotInDomain == 0)
		embeddedDotInDomain = domain.findFwd('.', 1);
	if ( ! isLocalDomain
	  && embeddedDotInDomain >= domain.getLength() - 1 ) {
		return false;
	}

	// if the host name contains no dot and the domain name is .local
	Uint32 firstDotInHost = host.findFwd('.');
	if (firstDotInHost == host.getLength() && isLocalDomain)
		return true;

	Uint32 domainLength = domain.getLength();
	Int32 lengthDiff = host.getLength() - domainLength;

	if ( lengthDiff == 0 ) {
		// if the host name and the domain name are just string-compare euqal
		return ( host.toLower() == domain.toLower() );
	}
	else if ( lengthDiff > 0 ) {
		// need to check H & D component
		Buffer H = host(0, lengthDiff);
		Buffer D = host(lengthDiff);
		return ( H.findFwd('.') == H.getLength()
		      && D.toLower() == domain.toLower() );
	}
	else if ( lengthDiff == -1 ) {
		// if domain is actually .host
		return ( domain[ 0 ] == '.'
		      && host.toLower() == domain( 1 ).toLower() );
	}

	return false;

}

// ============================================================================

Bool Net::HttpCookie::isToken(
	const	Buffer &	value ) {

	static const Buffer tspecials = ",;";

	const Uchar * p = value.getCstPtr();
	Uint32 l = value.getLength();

	while ( l-- ) {
		Uchar c = *p++;
		if ( c < 0x20 || c >= 0x7f || tspecials.contains( c ) ) {
			return false;
		}
	}

	return true;

}

Bool Net::HttpCookie::isReserved(
	const	Buffer &	name ) {

	static const Char * tbl[] = {
		"comment",
		"commenturl",
		"discard",
		"domain",
		"expires",
		"max-age",
		"path",
		"port",
		"secure",
		"version",
		0
	};

	static const BufferArray arr( tbl );

	return ( arr.contains( name.toLower() )
	      || ( name.hasContent() && name[ 0 ] == '$' ) );

}

// ============================================================================

const Time::Delta Net::HttpCookie::MAX_AGE_UNSPECIFIED( -1 );
const Buffer Net::HttpCookie::NETSCAPE_COOKIE_DATE_FORMAT = "EEE',' dd-MMM-yyyy HH:mm:ss 'GMT'";
const Buffer Net::HttpCookie::SET_COOKIE = "set-cookie:";
const Buffer Net::HttpCookie::SET_COOKIE2 = "set-cookie2:";

// ============================================================================

