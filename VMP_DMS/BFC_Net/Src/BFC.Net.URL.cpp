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
//	BFC.Net.URL.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Net.URL.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, URL )

// ============================================================================

const Uint16 Net::URL::InvalidPort = ( Uint16 )-1;

// ============================================================================

Net::URL::URL() :

	port	( InvalidPort ),
	valid	( false ) {

}

Net::URL::URL(
	const	Buffer &	pProtocol,
	const	Buffer &	pHost,
	const	Buffer &	pFile,
	const	Uint16		pPort ) {

	prot = pProtocol.toLower();

//	setScheme( prot );

	if ( pHost.hasContent() ) {
		host = pHost;
		port = pPort;
	}

	decodeFile(
		pFile,
		path,
		qury,
		rfrn );

	valid	= true;

}

Net::URL::URL(
	const	URL &		pBase,
	const	Buffer &	pSpec ) {

	if ( ! pBase.isValid() ) {
		throw InvalidArgument( "Not a valid Base URL!" );
	}

	Buffer	tmp = pSpec.trim();

	if ( tmp.beginsWith( "url:" ) ) {
		tmp = tmp( 4 );
	}

	if ( ! tmp.beginsWith( "#" ) ) {
		for ( Uint32 i = 0 ; i < tmp.getLength() ; i++ ) {
			Uchar c = tmp[ i ];
			if ( c == '/' ) {
				break;
			}
			if ( c == ':' ) {
				prot = tmp( 0, i ).toLower();
				tmp = tmp( i + 1 );
				break;
			}
		}
	}

	Bool	isRelative = false;

	if ( prot.isEmpty()
	  || prot == pBase.prot ) {

		// If the context is a hierarchical URL scheme and the spec
		// contains a matching scheme then maintain backwards
		// compatibility and treat it as if the spec didn't contain
		// the scheme; see 5.2.3 of RFC2396.

		if ( pBase.path.beginsWith( "/" ) ) {
			prot.kill();
		}

		if ( prot.isEmpty() ) {
			prot = pBase.prot;
			user = pBase.user;
			pass = pBase.pass;
			host = pBase.host;
			port = pBase.port;
			path = pBase.path;
			isRelative = true;
		}

	}

//	if ( prot.isEmpty() ) {
//		throw MalformedURL( "No protocol: " + pSpec );
//	}

//	setScheme( prot );

	Uint32 i = tmp.findFwd( '#' );

	if ( i < tmp.getLength() ) {
		rfrn = tmp( i + 1 );
		tmp = tmp( 0, i );
	}

	// Handle special case inheritance of query and fragment
	//implied by RFC2396 section 5.2.2.

	if ( isRelative && tmp.isEmpty() ) {
		qury = pBase.qury;
		if ( rfrn.isEmpty() ) {
			rfrn = pBase.rfrn;
		}
	}

	parseURL( tmp );

	valid	= true;

}

Net::URL::URL(
	const	Buffer &	spec ) {

	Buffer	tmp = spec.trim();

	if ( tmp.isEmpty() ) {
		return;
	}

	if ( tmp.beginsWith( "url:" ) ) {
		tmp = tmp( 4 );
	}

	Uint32	i;

	i = tmp.findFwd( ':' );

	if ( i < tmp.getLength() ) {
		prot = tmp( 0, i );
		tmp = tmp( i + 1 );
	}

//	if ( prot.isEmpty() ) {
//		throw MalformedURL( "No protocol in \"" + spec + "\"" );
//	}

//	setScheme( prot );

	if ( tmp.beginsWith( "//" ) ) {

		i = tmp.findFwd( '/', 2 );

		Buffer	auth;

		if ( i < tmp.getLength() ) {
			auth = tmp( 2, i - 2 );
			tmp = tmp( i + 1 );
		}
		else {
			auth = tmp( 2 );
			tmp.kill();
		}

		setAuth( auth );

	}

	if ( tmp.beginsWith( "/" ) ) {
		tmp = tmp( 1 );
	}

	setFile( tmp );

	valid	= true;

}

Net::URL::URL(
	const	URL &		pOther ) :

	SObject() /*,
	URI( pOther )*/ {

	if ( ( valid = pOther.valid ) ) {
		prot = pOther.prot;
		user = pOther.user;
		pass = pOther.pass;
		host = pOther.host;
		port = pOther.port;
		path = pOther.path;
		qury = pOther.qury;
		rfrn = pOther.rfrn;
	}
	else {
		port = InvalidPort;
	}

}

// ============================================================================

Net::URL & Net::URL::operator = (
	const	URL &		pOther ) {

	if ( this != &pOther ) {
		prot = pOther.prot;
		user = pOther.user;
		pass = pOther.pass;
		host = pOther.host;
		port = pOther.port;
		path = pOther.path;
		qury = pOther.qury;
		rfrn = pOther.rfrn;
		valid = pOther.valid;
	}

	return *this;

}

// ============================================================================

Buffer Net::URL::getAuth() const {

	Buffer auth;

	if ( host.hasContent() ) {
		if ( user.hasContent() ) {
			auth += user;
			if ( pass.hasContent() ) {
				auth += ":" + pass;
			}
			auth += "@";
		}
		auth += host;
		if ( port != InvalidPort
		  && port != getDefaultPort( prot ) ) {
			auth += ":" + Buffer( port );
		}
	}

	return auth;

}
 
// ============================================================================

Bool Net::URL::operator == (
	const	URL &		pOther ) const {

	return ( valid == pOther.valid
	      && prot == pOther.prot
	      && user == pOther.user
	      && pass == pOther.pass
	      && host == pOther.host
	      && port == pOther.port
	      && path == pOther.path
	      && qury == pOther.qury
	      && rfrn == pOther.rfrn );

}

// ============================================================================

Uint16 Net::URL::getDefaultPort(
	const	Buffer &	proto ) {

	if ( proto == "http" ) {
		return 80;
	}

	if ( proto == "rtsp" ) {
		return 554;
	}

	return InvalidPort;

}

// ============================================================================

void Net::URL::decodeAuth(
	const	Buffer &	pProt,
	const	Buffer &	pAuth,
		Buffer &	pUser,
		Buffer &	pPass,
		Buffer &	pHost,
		Uint16 &	pPort ) {

	Uint32	ind;

	ind = pAuth.findFwd( '@' );

	if ( ind < pAuth.getLength() ) {
		pUser = pAuth( 0, ind );
		pHost = pAuth( ind + 1 );
	}
	else {
		pUser.kill();
		pHost = pAuth;
	}

	ind = pUser.findFwd( ':' );

	if ( ind < pUser.getLength() ) {
		pPass = pUser( ind + 1 );
		pUser = pUser( 0, ind );
	}
	else {
		pPass.kill();
	}

	ind = pHost.findFwd( ':' );

	if ( ind < pHost.getLength() ) {
		pPort = pHost( ind + 1 ).toUint16();
		pHost = pHost( 0, ind );
	}
	else {
		pPort = getDefaultPort( pProt );
	}

}

Buffer Net::URL::encodeAuth(
	const	Buffer &	pProt,
	const	Buffer &	pUser,
	const	Buffer &	pPass,
	const	Buffer &	pHost,
	const	Uint16		pPort ) {

	Buffer res;

	if ( pHost.hasContent() ) {
		if ( pUser.hasContent() ) {
			res += pUser;
			if ( pPass.hasContent() ) {
				res += ":" + pPass;
			}
			res += "@";
		}
		res += pHost;
		if ( pPort != InvalidPort && pPort != getDefaultPort( pProt ) ) {
			res += ":" + Buffer( pPort );
		}
	}

	return res;

}

void Net::URL::decodeFile(
	const	Buffer &	pFile,
		Buffer &	pPath,
		Buffer &	pQury,
		Buffer &	pRfrn ) {

	Buffer	tmp;
	Uint32	ind;

	tmp = pFile;

	ind = tmp.findFwd( '#' );

	if ( ind < tmp.getLength() ) {
		pRfrn = tmp( ind + 1 );
		tmp = tmp( 0, ind );
	}
	else {
		pRfrn.kill();
	}

	ind = tmp.findFwd( '?' );

	if ( ind < tmp.getLength() ) {
		pQury = tmp( ind + 1 );
		tmp = tmp( 0, ind );
	} else {
		pQury.kill();
	}

	pPath = tmp;

}

Buffer Net::URL::encodeFile(
	const	Buffer &	pPath,
	const	Buffer &	pQury,
	const	Buffer &	pRfrn,
	const	Bool		pEscape ) {

	Buffer res;

	if ( pPath.hasContent() ) {
		if ( pEscape ) {
//			res += percentEncode( pPath, PathCharsToEncode, false );
			res += pPath;
		}
		else {
			res += pPath;
		}
	}

	if ( pQury.hasContent() ) {
		if ( pEscape ) {
//			res += "?" + percentEncode( pQury, QueryCharsToEncode, false );
			res += "?" + pQury;
		}
		else {
			res += "?" + pQury;
		}
	}

	if ( pRfrn.hasContent() ) {
		if ( pEscape ) {
//			res += "#" + percentEncode( pRfrn, FragmentCharsToEncode, false );
			res += "#" + pRfrn;
		}
		else {
			res += "#" + pRfrn;
		}
	}

	return res;

}

// ============================================================================

Buffer Net::URL::toBuffer() const {

//	msgDbg( "toBuffer(): path == \"" + path + "\"" );

	return ( prot.hasContent() ? prot + ":" : Buffer() )
		+ "//"
		+ encodeAuth( prot, user, pass, host, port )
		+ "/"
		+ encodeFile( path, qury, rfrn );

}

// ============================================================================

void Net::URL::kill() {

	prot.kill();
	user.kill();
	pass.kill();
	host.kill();
	port = InvalidPort;
	path.kill();
	qury.kill();
	rfrn.kill();

	valid = false;

}

// ============================================================================

void Net::URL::parseURL(
	const	Buffer &	spec ) {

//	msgDbg( "parseURL(): parsing \"" + spec + "\"" );

	Buffer	tmp = spec;

	Buffer tmpAuth	= getAuth();
	Buffer tmpPath	= path;

//	msgDbg( "parseURL(): tmpPath == \"" + tmpPath + "\"" );
//	msgDbg( "parseURL(): tmpAuth == \"" + tmpAuth + "\"" );

//	Bool isRelPath = false;

	// Strip off the query part

	Uint32	i = tmp.findFwd( '?' );
	Bool	queryOnly = ( tmp.hasContent() && i == 0 );

	if ( i < tmp.getLength() ) {
		qury = tmp( i + 1 );
		tmp = tmp( 0, i );
	}

	if ( ! tmp.beginsWith( "////" )
	  && tmp.beginsWith( "//" ) ) {

		tmp = tmp( 2 );

		i = tmp.findFwd( '/' );

		if ( i >= tmp.getLength() ) {
			i = tmp.findFwd( '?' );
		}

		tmpAuth = tmp( 0, i );
		tmp = tmp( i );

		if ( tmpAuth.getLength() > 0 ) {
			tmpPath.kill();
		}

		decodeAuth( prot, tmpAuth,
			user, pass, host, port );

	}

//	msgDbg( "parseURL(): tmp == \"" + tmp + "\"" );

	if ( tmp.hasContent() ) {
		if ( tmp[ 0 ] == '/' ) {
			tmpPath = tmp;
		}
		else if ( tmpPath.hasContent() ) {
//			isRelPath = true;
			i = tmpPath.findRev( '/' );
			if ( i == ( Uint32 )-1
			  && tmpAuth.hasContent() ) {
				tmpPath = tmp;
			}
			else {
				tmpPath = tmpPath( 0, i + 1)
					+ tmp;
			}
		}
		else {
			tmpPath = tmp;
		}
	}
	else if ( queryOnly && tmpPath.hasContent() ) {
		i = tmpPath.findRev( '/' );
		if ( i == ( Uint32 )-1 ) {
			tmpPath = "/";
		}
		else {
			tmpPath = tmpPath( 0, i + 1 );
		}
	}

	path = tmpPath;

//	msgDbg( "parseURL(): path == \"" + path + "\"" );

}

// ============================================================================

void Net::URL::set(
	const	Buffer &	/*protocol*/,
	const	Buffer &	/*host*/,
	const	Uint16		/*port*/,
	const	Buffer &	/*authority*/,
	const	Buffer &	/*userInfo*/,
	const	Buffer &	/*path*/,
	const	Buffer &	/*query*/,
	const	Buffer &	/*ref*/ ) {

	throw NotImplemented();

}

// ============================================================================

