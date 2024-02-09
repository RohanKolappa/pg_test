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
//	BFC.DOM.CharacterDataImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.CharacterDataImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, CharacterDataImpl )

// ============================================================================

DOM::CharacterDataImpl::CharacterDataImpl(
	const	NodeType	pNodeType,
		DocumentImplPtr	d,
		NodeImplPtr	p,
	const	Buffer &	data ) :

	NodeImpl( pNodeType, d, p ) {

	value = data;
	name = "#character-data";

}

DOM::CharacterDataImpl::CharacterDataImpl(
	const	NodeType	pNodeType,
		CharacterDataImplPtr	n,
		Bool		deep ) :

	NodeImpl( pNodeType, n, deep ) {

}

DOM::CharacterDataImpl::CharacterDataImpl(
		DocumentImplPtr	d,
		NodeImplPtr	p,
	const	Buffer &	data ) :

	NodeImpl( CharacterDataNode, d, p ) {

	value = data;
	name = "#character-data";

}

DOM::CharacterDataImpl::CharacterDataImpl(
		CharacterDataImplPtr	n,
		Bool		deep ) :

	NodeImpl( CharacterDataNode, n, deep ) {

}

// ============================================================================

DOM::NodeImplPtr DOM::CharacterDataImpl::cloneNode(
		Bool		deep ) {

	return new CharacterDataImpl( this, deep );

}

// ============================================================================

const Buffer & DOM::CharacterDataImpl::getData() const {

	return value;

}

void DOM::CharacterDataImpl::setData(
	const	Buffer &	pData ) {

	value = pData;

}

Uint32 DOM::CharacterDataImpl::getLength() const {

	return value.length();

}

Buffer DOM::CharacterDataImpl::substringData(
	const	Uint32		offset,
	const	Uint32		n ) const {

	return value( offset, n );

}

void DOM::CharacterDataImpl::appendData(
	const	Buffer &	arg ) {

	value += arg;

}

void DOM::CharacterDataImpl::insertData(
	const	Uint32		offset,
	const	Buffer &	arg ) {

	value.insert( offset, arg );

}

void DOM::CharacterDataImpl::deleteData(
	const	Uint32		offset,
	const	Uint32		n ) {

	value.remove( offset, n );

}

void DOM::CharacterDataImpl::replaceData(
	const	Uint32		offset,
	const	Uint32		n,
	const	Buffer &	arg ) {

	value.replace( offset, n, arg );

}

// ============================================================================

