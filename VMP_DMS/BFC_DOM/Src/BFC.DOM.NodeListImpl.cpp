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
//	BFC.DOM.NodeListImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.SPtrImpl.h"

#include "BFC.DOM.NodeImpl.h"
#include "BFC.DOM.NodeListImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, NodeListImpl )

// ============================================================================

DOM::NodeListImpl::NodeListImpl(
	const	NodeImplPtr &	pNode ) :

	node	( pNode, false, true ),
	stamp	( -1 ) {

}

DOM::NodeListImpl::NodeListImpl(
	const	NodeImplPtr &	pNode,
	const	Buffer &	pName ) :

	node	( pNode, false, true ),
	tagname	( pName ),
	stamp	( -1 ) {

}

DOM::NodeListImpl::NodeListImpl(
	const	NodeImplPtr &	pNode,
	const	Buffer &	pNsUri,
	const	Buffer &	localName ) :

	node	( pNode, false, true ),
	tagname	( localName ),
	nsURI	( pNsUri ),
	stamp	( -1 ) {

}

// ============================================================================

Bool DOM::NodeListImpl::operator == (
	const	NodeListImpl &	other ) const {

	return ( node == other.node && tagname == other.tagname );

}

Bool DOM::NodeListImpl::operator != (
	const	NodeListImpl &	other ) const {

	return ( node != other.node || tagname != other.tagname );

}

// ============================================================================

void DOM::NodeListImpl::createList() const {

	if ( ! node ) {
		return;
	}

	stamp	= nodeListTime;

	NodeImplPtr p( node->first, false, true );

	list.kill();

	if ( tagname.isEmpty() ) {
		while ( p ) {
			list += p;
			p = p->next;
		}
	}
	else if ( nsURI.isEmpty() ) {
		while ( p && p != node ) {
			if ( p->isElement() && p->getNodeName() == tagname ) {
				list += p;
			}
			if ( p->first ) {
				p = p->first;
			}
			else if ( p->next ) {
				p = p->next;
			}
			else {
				p = p->getParent();
				while ( p && p != node && !p->next ) {
					p = p->getParent();
				}
				if ( p && p != node ) {
					p = p->next;
				}
			}
		}
	}
	else {
		while ( p && p != node ) {
			if ( p->isElement() && p->name == tagname && p->nsURI == nsURI ) {
				list += p;
			}
			if ( p->first ) {
				p = p->first;
			}
			else if ( p->next ) {
				p = p->next;
			}
			else {
				p = p->getParent();
				while ( p && p != node && !p->next ) {
					p = p->getParent();
				}
				if ( p && p != node ) {
					p = p->next;
				}
			}
		}
	}

}

// ============================================================================

DOM::NodeImplPtr DOM::NodeListImpl::getItem(
	const	Uint32		index ) const {

	if ( stamp < nodeListTime ) {
		createList();
	}

	return list[ index ];

}

Uint32 DOM::NodeListImpl::length() const {

	if ( stamp < nodeListTime ) {
		createList();
	}

	return list.getSize();

}

// ============================================================================

