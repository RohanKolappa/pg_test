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
//	BFC.DOM.NamedNodeMapImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.SPtrImpl.h"

#include "BFC.DOM.NamedNodeMapImpl.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, NamedNodeMapImpl )

// ============================================================================

DOM::NamedNodeMapImpl::NamedNodeMapImpl(
		NodeImplPtr	n ) :

	parent	( n, false, true ),
	readonly( false ),
	appendToParent	( false ) {

}

// ============================================================================

DOM::NamedNodeMapImplPtr DOM::NamedNodeMapImpl::clone(
		NodeImplPtr	p ) {

	NamedNodeMapImplPtr m = new NamedNodeMapImpl(p);

	m->readonly = readonly;
	m->appendToParent = appendToParent;

	for ( Uint32 i = 0 ; i < map.getSize() ; i++ ) {
		NodeImplPtr new_node = map.getValue( i )->cloneNode();
		new_node->setParent(p);
		m->setNamedItem(new_node);
	}

	return m;

}

// ============================================================================

void DOM::NamedNodeMapImpl::clearMap() {

	map.kill();

}

// ============================================================================

Uint32 DOM::NamedNodeMapImpl::length() const {

	return map.getSize();

}

// ============================================================================

DOM::NodeImplPtr DOM::NamedNodeMapImpl::getNamedItem(
	const	Buffer		& name ) const {

	Uint32	pos;

	if ( map.contains( name, pos ) ) {
		return map.getValue( pos );
	}

//	msgWrn( "NodeImpl: requesting unexisting node [\""
//		+ name + "\"]!" );

//	map.add( name, 0 );

	return 0;

}

DOM::NodeImplPtr DOM::NamedNodeMapImpl::getNamedItemNS(
	const	Buffer &	pNsUri,
	const	Buffer &	localName ) const {

	NodeImplPtr n;

	for ( Uint32 i = 0 ; i < map.getSize() ; i++ ) {
		n = map.getValue( i );
		if (!n->prefix.isEmpty()) {
			// node has a namespace
			if (n->nsURI == pNsUri && n->name == localName)
				return n;
		}
	}

	return 0;

}

DOM::NodeImplPtr DOM::NamedNodeMapImpl::setNamedItem(
		NodeImplPtr	arg ) {

	if (readonly || !arg)
		return 0;

	if (appendToParent)
		return parent->appendChild(arg);

	NodeImplPtr n;

	Uint32	pos;

	if ( map.contains( arg->getNodeName(), pos ) ) {
		n = map.getValue( pos );
	}
	else {
		//msgWrn( "NodeImpl::setNamedItem(): auto-adding [\""
		//	+ arg->getNodeName()
		//	+ "\"]!" );
		// map.add( arg->getNodeName(), 0 );
		//n = 0;
	}

	// We take a reference

	map.add(arg->getNodeName(), arg, true);

	return n;

}

DOM::NodeImplPtr DOM::NamedNodeMapImpl::setNamedItemNS(
		NodeImplPtr	arg ) {

	if (readonly || !arg)
		return 0;

	if (appendToParent)
		return parent->appendChild(arg);

	if (!arg->prefix.isEmpty()) {
		// node has a namespace
		NodeImplPtr n = getNamedItemNS( arg->nsURI, arg->name );
		// We take a reference
		map.add(arg->getNodeName(), arg, true);
		return n;
	} else {
		// ### check the following code if it is ok
		return setNamedItem(arg);
	}

}

DOM::NodeImplPtr DOM::NamedNodeMapImpl::removeNamedItem(
	const	Buffer &	name ) {

	if ( readonly ) {
		return NodeImplPtr();
	}

	NodeImplPtr p = getNamedItem( name );

	if ( ! p ) {
		return NodeImplPtr();
	}

	if ( appendToParent ) {
		return parent->removeChild( p );
	}

	map.del( p->getNodeName() );

	return p;

}

DOM::NodeImplPtr DOM::NamedNodeMapImpl::getItem(
	const	Uint32		index ) const {

	return ( index >= length() ? NodeImplPtr() : map.getValue( index ) );

}

// ============================================================================

Bool DOM::NamedNodeMapImpl::contains(
	const	Buffer &	name ) const {

	return map.contains( name );

}

Bool DOM::NamedNodeMapImpl::containsNS(
	const	Buffer &	pNsUri,
	const	Buffer &	localName ) const {

	return getNamedItemNS( pNsUri, localName );

}

// ============================================================================

