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
//                          Boston, MA 02111( Uint32 )-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.XML.Attributes.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.XML.Attributes.h"

// ============================================================================

using namespace BFC;

// ============================================================================

XML::Attributes::Attributes() {



}

XML::Attributes::~Attributes() {



}

// ============================================================================

Uint32 XML::Attributes::getIndex(
	const	Buffer &	qName ) const {

	for ( Uint32 i = 0 ; i < attList.getSize() ; ++i ) {
		if ( attList[ i ].qname == qName )
			return i;
	}

	throw NoSuchAttribute( qName );

}

Uint32 XML::Attributes::getIndex(
	const	Buffer &	uri,
	const	Buffer &	localPart ) const {

	for ( Uint32 i = 0; i < attList.getSize(); ++i ) {
		const Attribute &att = attList[ i ];
		if ( att.uri == uri && att.localname == localPart )
			return i;
	}

	throw NoSuchAttribute();

}

// ============================================================================

const Buffer & XML::Attributes::getLocalName(
	const	Uint32		index ) const {

	if ( index >= attList.getSize() ) {
		throw NoSuchAttribute();
	}

	return attList[ index ].localname;

}

const Buffer & XML::Attributes::getQualifiedName(
	const	Uint32		index ) const {

	if ( index >= attList.getSize() ) {
		throw NoSuchAttribute();
	}

	return attList[ index ].qname;

}

const Buffer & XML::Attributes::getUri(
	const	Uint32		index ) const {

	if ( index >= attList.getSize() ) {
		throw NoSuchAttribute();
	}

	return attList[ index ].uri;

}

// ============================================================================

const Buffer & XML::Attributes::getValue(
	const	Uint32		index ) const {

	if ( index >= attList.getSize() ) {
		throw NoSuchAttribute();
	}

	return attList[ index ].value;

}

const Buffer & XML::Attributes::getValue(
	const	Buffer &	qName ) const {

	return attList[ getIndex( qName ) ].value;

}

const Buffer & XML::Attributes::getValue(
	const	Buffer &	uri,
	const	Buffer &	localName ) const {

	return attList[ getIndex( uri, localName ) ].value;

}

// ============================================================================

void XML::Attributes::append(
	const	Buffer &	qName,
	const	Buffer &	uri,
	const	Buffer &	localPart,
	const	Buffer &	value ) {

	Attribute att;

	att.qname = qName;
	att.uri = uri;
	att.localname = localPart;
	att.value = value;

	attList += att;

}

// ============================================================================

const Buffer XML::Attributes::emptyBuffer;
const Buffer XML::Attributes::cdataBuffer = "CDATA";

// ============================================================================

