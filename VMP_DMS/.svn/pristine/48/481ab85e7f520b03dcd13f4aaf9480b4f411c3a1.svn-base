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
//	BFC.DOM.NameListImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.DOM.NameListImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, NameListImpl )

// ============================================================================

DOM::NameListImpl::NameListImpl() {

}

// ============================================================================

const Buffer & DOM::NameListImpl::getName(
	const	Uint32		pIndex ) const {

	return ( pIndex < map.getSize() ? map.getKey( pIndex ) : empty );

}

const Buffer & DOM::NameListImpl::getNamespaceURI(
	const	Uint32		pIndex ) const {

	return ( pIndex < map.getSize() ? map.getValue( pIndex ) : empty );

}

Uint32 DOM::NameListImpl::getLength() const {

	return map.getSize();

}

Bool DOM::NameListImpl::contains(
	const	Buffer &	pName ) const {

	return ( map.contains( pName ) );

}

Bool DOM::NameListImpl::containsNS(
	const	Buffer &	namespaceURI,
	const	Buffer &	pName ) const {

	return ( map.contains( pName )
	      && map[ pName ] == namespaceURI );

}

// ============================================================================

const Buffer DOM::NameListImpl::empty;

// ============================================================================

