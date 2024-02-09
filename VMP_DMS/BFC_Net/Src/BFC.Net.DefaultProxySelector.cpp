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
//	BFC.Net.DefaultProxySelector.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>

#include "BFC.Net.DefaultProxySelector.h"

#include "BFC.TL.Map.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, DefaultProxySelector )

// ============================================================================

Net::ProxyCArray Net::DefaultProxySelector::select(
	const	URL &		url ) {

//	Buffer	prot = url.getScheme().toLower();
	Buffer	prot = url.getProtocol().toLower();
	Buffer	host = url.getHost().toLower();

	ProxyCArray res;

	if ( isLoopback( host ) ) {
		res += Proxy::NO_PROXY;
	}
	else {
		res += getProxy( prot, host );
	}

	return res;

}

void Net::DefaultProxySelector::connectFailed(
	const	URL &		/* url */,
	const	SockAddrCPtr &	/* addr */ ) {

}

// ============================================================================

typedef Map< Buffer, Buffer >	PropsMap;
typedef PropsMap::InitItem	PropsItem;
typedef PropsMap::InitTable	PropsTable;

static PropsTable propsTbl = {
	{ "http",	"http_proxy" },
	{ "https",	"https_proxy" },
	{ "ftp",	"ftp_proxy" }
};

static Map< Buffer, Buffer > props( propsTbl, 3 );

Net::ProxyCPtr Net::DefaultProxySelector::getProxy(
	const	Buffer &	pProt,
	const	Buffer &	/*pHost*/ ) {

	ProxyCPtr res = Proxy::NO_PROXY;

	if ( ! props.contains( pProt ) ) {
		return res;
	}

	Buffer	varName = props[ pProt ];
	Buffer	varData = ::getenv( varName.getCStringPtr() );

	if ( varData.isEmpty() ) {
		return res;
	}

	URL	proxyURL( varData );

	if ( ! proxyURL.isValid() ) {
		return res;
	}

	Buffer	proxyHost = proxyURL.getHost();
	Uint16	proxyPort = proxyURL.getPort();

	SockAddrCPtr
		proxyAddr = new SockAddr( proxyHost, proxyPort, false );

	return new Proxy( Proxy::HTTP, proxyAddr );

}

// ============================================================================

Bool Net::DefaultProxySelector::isLoopback(
	const	Buffer &	pHost ) {

	return ( pHost == "localhost"
	      || pHost.beginsWith( "127." )
	      || pHost.endsWith( ":1" ) );

}

// ============================================================================

