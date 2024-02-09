// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::DOM".
// 
// "BFC::DOM" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::DOM"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.AttrImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.AttrImpl.h"
#include "BFC.DOM.DocumentImpl.h"
#include "BFC.DOM.TextImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, AttrImpl )

// ============================================================================

DOM::AttrImpl::AttrImpl(
		DocumentImplPtr	d,
		NodeImplPtr	parent,
	const	Buffer &	pName ) :

	NodeImpl( AttributeNode, d, parent ) {

	name = pName;
	m_specified = false;

}

DOM::AttrImpl::AttrImpl(
		DocumentImplPtr	d,
		NodeImplPtr	p,
	const	Buffer &	pNsUri,
	const	Buffer &	qName ) :

	NodeImpl( AttributeNode, d, p ) {

	splitNamespace( prefix, name, qName, ! pNsUri.isEmpty() );
	nsURI = pNsUri;
	createdWithDom1Interface = false;
	m_specified = false;

}

DOM::AttrImpl::AttrImpl(
		AttrImplPtr	n,
		Bool		deep ) :

	NodeImpl( AttributeNode, n, deep ) {

	m_specified = n->isSpecified();

}

// ============================================================================

void DOM::AttrImpl::setNodeValue(
	const	Buffer		& v ) {

	value = v;

	TextImplPtr t = new TextImpl( 0, this, v );

	if ( first ) {
		removeChild( first );
	}

	appendChild( t );

	m_specified = true;

}

// ============================================================================

DOM::NodeImplPtr DOM::AttrImpl::cloneNode(
		Bool		deep ) {

	return new AttrImpl( this, deep );

}

// ============================================================================

const Buffer & DOM::AttrImpl::getName() const {

	return name;

}

Bool DOM::AttrImpl::isSpecified() const {

	return m_specified;

}

const Buffer & DOM::AttrImpl::getValue() const {

	return value;

}

void DOM::AttrImpl::setValue(
	const	Buffer &	pValue ) {

	setNodeValue( pValue );

}

DOM::XElementPtr DOM::AttrImpl::getOwnerElement() const {

	throw NotImplemented();

}

DOM::TypeInfoCPtr DOM::AttrImpl::getSchemaTypeInfo() const {

	throw NotImplemented();

}

Bool DOM::AttrImpl::isId() const {

	throw NotImplemented();

}

// ============================================================================

void DOM::AttrImpl::save(
		IO::DevicePtr	s,
		int,
		int ) const {

	if ( nsURI.isEmpty() ) {
		s->putBytes( name );
		s->putBytes( "=\"" );
		s->putBytes( encodeAttr( value ) );
		s->putBytes( "\"" );
	}
	else {
		s->putBytes( prefix );
		s->putBytes( ":" );
		s->putBytes( name );
		s->putBytes( "=\"" );
		s->putBytes( encodeAttr( value ) );
		s->putBytes( "\" xmlns:" );
		s->putBytes( prefix );
		s->putBytes( "=\"" );
		s->putBytes( encodeAttr( nsURI ) );
		s->putBytes( "\"" );
	}

}

// ============================================================================

