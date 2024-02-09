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
//	BFC.XML.NamespaceSupport.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

//#include "BFC.Debug.DObject.h"

#include "BFC.XML.NamespaceSupport.h"

// ============================================================================

using namespace BFC;
using namespace BFC::XML;

// ============================================================================

NamespaceSupport::NamespaceSupport() {

	ns.add( "xml", "http://www.w3.org/XML/1998/namespace" );

}

NamespaceSupport::~NamespaceSupport() {



}

// ============================================================================

void NamespaceSupport::setPrefix(
	const	Buffer &	pre,
	const	Buffer &	uri ) {

	ns.setOrAdd( pre, uri );

}

// ============================================================================

const Buffer & NamespaceSupport::getPrefix(
	const	Buffer &	uri ) const {

	for ( Uint32 i = 0 ; i < ns.getSize() ; ) {
		if ( ns.getValue( i ) == uri
		  && ns.getKey( i ).hasContent() ) {
			return ns.getKey( i );
		}
	}

	return emptyBuffer;

}

// ============================================================================

const Buffer & NamespaceSupport::getUri(
	const	Buffer &	prefix ) const {

//	msgDbg( "getURI(): prefix == \"" + prefix + "\"" );

	return ns[ prefix ];

}

// ============================================================================

void NamespaceSupport::splitName(
	const	Buffer &	qname,
		Buffer &	prefix,
		Buffer &	localname ) const {

	Uint32 pos = qname.findFwd( ':', 0 );

	prefix = qname( 0, pos );
	localname = qname( pos + 1 );

}

// ============================================================================

void NamespaceSupport::processName(
	const	Buffer &	qname,
	const	Bool		isAttribute,
		Buffer &	nsuri,
		Buffer &	localname ) const {

//	msgDbg( "processName(): qname == \"" + qname + "\"" );

	Uint32 pos = qname.findFwd( ':', 0 );

	if ( pos < qname.getLength() ) {
		nsuri = getUri( qname( 0, pos ) );
		localname = qname( pos + 1 );
		return;
	}

	nsuri.kill();

	if ( ! isAttribute && ns.exists( "", pos ) ) {
		nsuri = ns.getValue( pos );
	}

	localname = qname;

}

// ============================================================================

BufferArray NamespaceSupport::getPrefixes() const {

	NamespaceMap::KeyArrayType res = ns.getAllKeys();

	for ( Uint32 i = 0 ; i < res.getSize() ; ) {
		if ( res[ i ].isEmpty() ) {
			res.pop( i );
		}
		else {
			i++;
		}
	}

	return res;

}

BufferArray NamespaceSupport::prefixes(
	const	Buffer &	uri ) const {

	BufferArray	res;

	for ( Uint32 i = 0 ; i < ns.getSize() ; i++ ) {
		if ( ns.getKey( i ).hasContent()
		  && ns.getValue( i ) == uri ) {
			res += ns.getKey( i );
		}
	}

	return res;

}

// ============================================================================

void NamespaceSupport::pushContext() {

	nsStack.push( ns );

}

void NamespaceSupport::popContext() {

	if ( ! nsStack.isEmpty() ) {
		ns = nsStack.top();
		nsStack.pop();
	}

}

// ============================================================================

void NamespaceSupport::reset() {

	nsStack.kill();
	ns.kill();

}

// ============================================================================

const Buffer NamespaceSupport::emptyBuffer;

// ============================================================================

