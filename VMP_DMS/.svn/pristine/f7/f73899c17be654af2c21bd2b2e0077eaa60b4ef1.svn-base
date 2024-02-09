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
//	BFC.Net.URLStreamHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Net.IPAddrUtil.h"
#include "BFC.Net.URLStreamHandler.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, URLStreamHandler )

// ============================================================================

Net::URLConnectionPtr Net::URLStreamHandler::openConnection(
	const	URL &		/*pURL*/,
		ProxyPtr	/*pProxy*/ ) {

	throw NotImplemented();

}

void Net::URLStreamHandler::parseURL(
		URL &		pURL,
	const	Buffer &	pSpc ) const {

	Buffer	spec = pSpc;

	Buffer prot = pURL.getProtocol();
	Buffer auth = pURL.getAuth();
	Buffer user = pURL.getUserInfo();
	Buffer host = pURL.getHost();
	Uint16 port = pURL.getPort();
	Buffer path = pURL.getPath();
	Buffer qury = pURL.getQuery();

	Buffer ref = pURL.getRef();

	Bool isRelPath = false;
	Bool queryOnly = false;

	if ( spec.hasContent() ) {
		Uint32 queryStart = spec.findFwd( '?' );
		queryOnly = ( queryStart == 0 );
		if ( queryStart < spec.getLength() ) {
			qury = spec( queryStart + 1 );
			spec = spec( 0, queryStart );
		}
	}

	Uint32 i = 0;

	Bool isUNCName = ( spec.beginsWith( "////" ) );

	if ( ! isUNCName && spec.beginsWith( "//" ) ) {
		spec = spec( 2 );
		i = spec.findFwd( '/' );
		if ( i >= spec.getLength() ) {
			i = spec.findFwd( '?' );
			if ( i >= spec.getLength() )
				i = spec.getLength();
		}

		host = auth = spec( 0, i );

		Uint32 ind = auth.findFwd('@');
		if ( ind < auth.getLength() ) {
			user = auth( 0, ind );
			host = auth( ind + 1 );
		}
		else {
			user.kill();
		}
		if ( host.hasContent() ) {
			// If the host is surrounded by [ and ] then its an IPv6
			// literal address as specified in RFC2732
			if ( host.getLength()>0 && (host[ 0 ]== '[')) {
				if ((ind = host.findFwd(']')) > 2
				  && ind < host.getLength() ) {
					Buffer nhost = host;
					host = nhost( 0, ind + 1 );
					if ( ! IPAddrUtil::isIPv6LiteralAddress( host( 1, ind - 1 ) ) ) {
						throw InvalidArgument(
							"Invalid host: "+ host );
					}
					port = URL::InvalidPort;
					if ( nhost.getLength() > ind + 1 ) {
						if ( nhost[ ind + 1 ] == ':') {
							++ind;
							if (nhost.getLength() > (ind + 1)) {
								port = nhost( ind + 1 ).toUint16();
							}
						} else {
							throw InvalidArgument(
								"Invalid authority field: " + auth);
						}
					}
				} else {
					throw InvalidArgument(
						"Invalid authority field: " + auth);
				}
			} else {
				ind = host.findFwd( ':' );
				port = URL::InvalidPort;
				if ( ind < host.getLength() ) {
					// port can be null according to RFC2396
					if (host.getLength() > (ind + 1)) {
						port = host( ind + 1 ).toUint16();
					}
					host = host( 0, ind );
				}
			}
		} else {
			host.kill();
		}
		spec = spec( i );
		// If the authority is defined then the path is defined by the
		// spec only; See RFC 2396 Section 5.2.4.
		if ( auth.getLength() > 0 )
			path.kill();
	}

	if ( spec.hasContent() ) {
		if ( spec[ 0 ] == '/' ) {
			path = spec;
		}
		else if ( path.getLength() > 0) {
			isRelPath = true;
			Uint32 ind = path.findRev( '/' );
			Buffer seperator;
			if ( ind >= path.getLength() && auth.hasContent() )
				seperator = "/";
			path = path( 0, ind + 1 ) + seperator + spec;

		}
		else {
			Buffer seperator = ( auth.hasContent() ) ? "/" : "";
			path = seperator + spec;
		}
	}
	else if ( queryOnly && path.hasContent() ) {
		Uint32 ind = path.findRev( '/' );
		if ( ind >= path.getLength() )
			ind = 0;
		path = path( 0, ind ) + "/";
	}

	if (isRelPath) {
		// Remove embedded /./
		while ((i = path.findFwd("/./")) < path.getLength()) {
			path = path( 0, i ) + path( i + 2 );
		}
		// Remove embedded /../ if possible
		i = 0;
		while ((i = path.findFwd("/../", i)) < path.getLength()) {
			/*
			 * A "/../" will cancel the previous segment and itself,
			 * unless that segment is a "/../" itself
			 * i.e. "/a/b/../c" becomes "/a/c"
			 * but "/../../a" should stay unchanged
			 */
			Uint32 limit;
			if ( i > 0
			  && ( limit = path.findRev( '/', i - 1 ) ) < path.getLength()
			  && ( path.findFwd( "/../", limit ) != 0 ) ) {
				path = path(0, limit) + path(i + 3);
				i = 0;
			}
			else {
				i += 3;
			}
		}
		// Remove trailing .. if possible
		while ( path.endsWith("/..")) {
			i = path.findFwd("/..");
			Uint32 limit;
			if ((limit = path.findRev('/', i - 1)) < path.getLength()) {
				path = path( 0, limit + 1 );
			} else {
				break;
			}
		}
		// Remove starting .
		if (path.beginsWith("./") && path.getLength() > 2)
			path = path(2);

		// Remove trailing .
		if (path.endsWith("/."))
			path = path(0, path.getLength() -1);
	}

	setURL(pURL, prot, host, port, auth, user, path, qury, ref);

}

Uint16 Net::URLStreamHandler::getDefaultPort() const {

	return URL::InvalidPort;

}

Bool Net::URLStreamHandler::equals(
	const	URL &		pURL1,
	const	URL &		pURL2 ) const {

	return ( pURL1.getRef() == pURL2.getRef()
	      && sameFile( pURL1, pURL2 ) );

}

Bool Net::URLStreamHandler::sameFile(
	const	URL &		pURL1,
	const	URL &		pURL2 ) const {

	if ( pURL1.getProtocol().toLower() != pURL2.getProtocol().toLower() )
		return false;

	if ( pURL1.getFile() != pURL2.getFile() )
		return false;

	Uint16 port1, port2;

	port1 = (pURL1.getPort() != URL::InvalidPort) ? pURL1.getPort() : pURL1.getHandler()->getDefaultPort();
	port2 = (pURL2.getPort() != URL::InvalidPort) ? pURL2.getPort() : pURL2.getHandler()->getDefaultPort();

	if (port1 != port2)
		return false;

	if ( ! hostsEqual( pURL1, pURL2 ) )
		return false;

	return true;

}

Net::IPAddrCPtr Net::URLStreamHandler::getAddr(
	const	URL &		pURL ) const {

	if ( pURL.getHostAddress() ) {
		return pURL.getHostAddress();
	}

	Buffer host = pURL.getHost();

	if ( host.isEmpty() ) {
		return IPAddrCPtr();
	}

	return IPAddr::getByName( host );

}

Bool Net::URLStreamHandler::hostsEqual(
	const	URL &		pURL1,
	const	URL &		pURL2 ) const {

	IPAddrCPtr a1 = getAddr( pURL1 );
	IPAddrCPtr a2 = getAddr( pURL2 );

	if ( a1 && a2 ) {
		return a1->equals( a2 );
	}
	else if ( pURL1.getHost().hasContent() && pURL2.getHost().hasContent() ) {
		return pURL1.getHost().toLower() == pURL2.getHost().toLower();
	}
	else {
		return pURL1.getHost().isEmpty() && pURL2.getHost().isEmpty();
	}

}

Buffer Net::URLStreamHandler::toExternalForm(
	const	URL &		pURL ) const {

	Buffer result;

	result += pURL.getProtocol();
	result += ":";

	if ( pURL.getAuth().getLength() > 0 ) {
		result += "//";
		result += pURL.getAuth();
	}
	if ( pURL.getPath().hasContent() ) {
		result += pURL.getPath();
	}
	if ( pURL.getQuery().hasContent() ) {
		result += '?';
		result += pURL.getQuery();
	}
	if ( pURL.getRef().hasContent() ) {
		result += "#";
		result += pURL.getRef();
	}

	return result;

}

void Net::URLStreamHandler::setURL(
		URL &		pURL,
	const	Buffer &	/* pPrt */,
	const	Buffer &	pHost,
	const	Uint16		pPort,
	const	Buffer &	pAuth,
	const	Buffer &	pInfo,
	const	Buffer &	pPath,
	const	Buffer &	pQury,
	const	Buffer &	pRef ) const {

	if ( pURL.getHandler() != this ) {
		throw InternalError( "handler for url different from this handler" );
	}

	pURL.set( pURL.getProtocol(), pHost, pPort, pAuth, pInfo, pPath, pQury, pRef );

}

#endif // 0

// ============================================================================

