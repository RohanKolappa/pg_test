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
//	BFC.DOM.TextImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.TextImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, TextImpl )

// ============================================================================

DOM::TextImpl::TextImpl(
	const	NodeType	pNodeType,
		DocumentImplPtr	d,
		NodeImplPtr	parent,
	const	Buffer &	val ) :

	CharacterDataImpl( pNodeType, d, parent, val ) {

	name = "#text";

}

DOM::TextImpl::TextImpl(
	const	NodeType	pNodeType,
		TextImplPtr	n,
		Bool		deep ) :

	CharacterDataImpl( pNodeType, n, deep ) {

}

DOM::TextImpl::TextImpl(
		DocumentImplPtr	d,
		NodeImplPtr	parent,
	const	Buffer &	val ) :

	CharacterDataImpl( TextNode, d, parent, val ) {

	name = "#text";

}

DOM::TextImpl::TextImpl(
		TextImplPtr	n,
		Bool		deep ) :

	CharacterDataImpl( TextNode, n, deep ) {

}

// ============================================================================

DOM::NodeImplPtr DOM::TextImpl::cloneNode(
		Bool		deep ) {

	return new TextImpl( this, deep );

}

// ============================================================================

DOM::XTextPtr DOM::TextImpl::splitText(
	const	Uint32		offset ) {

	if ( ! getParent() ) {
		throw InvalidArgument( "Can't split orphan Text!" );
	}

	TextImplPtr t = new TextImpl( getOwnerDocument(), 0, value( offset ) );
	value = value( 0, offset );

	getParent()->insertAfter( t, this );

	return t;

}

Bool DOM::TextImpl::isElementContentWhitespace() const {

	throw NotImplemented();

}

const Buffer & DOM::TextImpl::getWholeText() const {

	throw NotImplemented();

}

DOM::XTextPtr DOM::TextImpl::replaceWholeText(
	const	Buffer &	/*content*/ ) {

	throw NotImplemented();

}

// ============================================================================

void DOM::TextImpl::save(
		IO::DevicePtr	s,
		int,
		int ) const {

	s->putBytes( encodeAttr( value, ! ( getParent() && getParent()->isElement() ) ) );

}

// ============================================================================

