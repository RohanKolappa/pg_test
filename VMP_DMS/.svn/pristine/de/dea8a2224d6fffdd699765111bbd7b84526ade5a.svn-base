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
//	BFC.DOM.NodeList.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.SPtrImpl.h"

#include "BFC.DOM.NodeList.h"
#include "BFC.DOM.NodeListImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::NodeList::NodeList() :

	impl	( 0, true, true ) {

}

DOM::NodeList::NodeList(
	const	NodeListImplPtr	&	p ) :

	impl	( p, true, true ) {

}

DOM::NodeList::NodeList(
	const	NodeList &	pOther ) :

	SObject	(),
	impl	( pOther.impl, true, true ) {

}

// ============================================================================

DOM::NodeList & DOM::NodeList::operator = (
	const	NodeList &	pOther ) {

	impl = pOther.impl;

	return *this;

}

// ============================================================================

Bool DOM::NodeList::operator == (
	const	NodeList &	pOther ) const {

	if ( impl == pOther.impl ) {
		return true;
	}

	if ( ! impl || ! pOther.impl ) {
		return false;
	}

	return ( *impl == *pOther.impl );

}

Bool DOM::NodeList::operator != (
	const	NodeList &	pOther ) const {

	return ( ! operator == ( pOther ) );

}

// ============================================================================

DOM::Node DOM::NodeList::getItem(
	const	Uint32		index ) const {

	if ( ! impl ) {
		return Node();
	}

	return Node( impl->getItem( index ) );

}

Uint32 DOM::NodeList::length() const {

	if ( ! impl ) {
		return 0;
	}

	return impl->length();

}

// ============================================================================

