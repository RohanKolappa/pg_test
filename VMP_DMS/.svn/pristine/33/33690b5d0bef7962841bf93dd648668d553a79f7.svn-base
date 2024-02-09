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
//	BFC.Net.URLConnection.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if 0

#include "BFC.Net.URLConnection.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Net, URLConnection )

// ============================================================================

Net::URLConnection::URLConnection(
		URLPtr		__url ) {

	url = __url;
	connected = false;
	doInput = true;
	doOutput = false;
	allowUserInteraction = defaultAllowUserInteraction;
	useCaches = defaultUseCaches;

}

Net::URLConnection::~URLConnection() {



}

// ============================================================================

Net::NObjectPtr Net::URLConnection::getContent() {

	return getContentHandler()->getContent( this );

}

// ============================================================================

void Net::URLConnection::setDoInput(
	const	Bool		doinput ) {

	if ( connected ) {
		throw IllegalState();
	}

	doInput = doinput;

}

void Net::URLConnection::setDoOutput(
	const	Bool		dooutput ) {

	if ( connected ) {
		throw IllegalState();
	}

	doOutput = dooutput;

}

void Net::URLConnection::setAllowUserInteraction(
	const	Bool		allowuserinteraction ) {

	if ( connected ) {
		throw IllegalState();
	}

	allowUserInteraction = allowuserinteraction;

}

void Net::URLConnection::setUseCaches(
	const	Bool		usecaches ) {

	if ( connected ) {
		throw IllegalState();
	}

	useCaches = usecaches;

}

void Net::URLConnection::setIfModifiedSince(
	const	Time::Clock &	ifmodifiedsince ) {

	if ( connected ) {
		throw IllegalState();
	}

	ifModifiedSince = ifmodifiedsince;

}

void Net::URLConnection::setRequestProperty(
	const	Buffer &	key,
	const	Buffer &	value ) {

	if ( connected ) {
		throw IllegalState();
	}

	requests.setOrAdd( key, value );

}

void Net::URLConnection::addRequestProperty(
	const	Buffer &	key,
	const	Buffer &	value ) {

	if ( connected ) {
		throw IllegalState();
	}

	requests.add( key, value );

}

const Buffer & Net::URLConnection::getRequestProperty(
	const	Buffer &	key ) {

	if ( connected ) {
		throw IllegalState();
	}

	return requests[ key ];

}

// ============================================================================

Net::ContentHandlerPtr Net::URLConnection::getContentHandler() {

	return handlers[ stripOffParameters( getContentType() ) ];

}

Buffer Net::URLConnection::stripOffParameters(
	const	Buffer &	contentType ) {

	Uint32 index = contentType.findFwd( ';' );

	if ( index < contentType.getLength() ) {
		return contentType( 0, index );
	}

	return contentType;

}

// ============================================================================

Bool	Net::URLConnection::defaultAllowUserInteraction = false;
Bool	Net::URLConnection::defaultUseCaches = true;

Net::ContentHandlerFactoryPtr		Net::URLConnection::factory;
Map< Buffer, Net::ContentHandlerPtr >	Net::URLConnection::handlers;

#endif // 0

// ============================================================================

