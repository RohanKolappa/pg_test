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
//	BFC.DOM.NodeImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include <iostream>

#include "BFC.DOM.NodeImpl.h"
#include "BFC.DOM.TextImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, NodeImpl )

// ============================================================================

DOM::NodeImpl::NodeImpl(
	const	NodeType	pNodeType,
	const	DocumentImplPtr	&
				doc,
	const	NodeImplPtr &	par ) :

	prev	( 0, false, true ),
	next	( 0, true, true ),
	ownr	( 0, false, true ),
	first	( 0, true, true ),
	last	( 0, true, true ),

	nodeType( pNodeType ) {

	if ( par ) {
		setParent( par );
	}
	else {
		setOwnerDocument( doc );
	}

	createdWithDom1Interface = true;

}

DOM::NodeImpl::NodeImpl(
	const	NodeType	pNodeType,
	const	NodeImplPtr &	n,
	const	Bool		deep ) :

	prev	( 0, false, true ),
	next	( 0, true, true ),
	ownr	( 0, false, true ),
	first	( 0, true, true ),
	last	( 0, true, true ),

	nodeType( pNodeType ) {

	setOwnerDocument( n->getOwnerDocument() );

	name = n->name;
	value = n->value;
	prefix = n->prefix;
	nsURI = n->nsURI;
	createdWithDom1Interface = n->createdWithDom1Interface;

	if ( deep ) {
		for ( NodeImplPtr x = n->first ; x ; x = x->next ) {
			appendChild( x->cloneNode( true ) );
		}
	}

}

DOM::NodeImpl::~NodeImpl() {

//	if ( hasParent ) {
//		cerr << "Told you so (type: " << getNodeType() << ")!" << endl;
//	}

	NodeImplPtr p = first;
	NodeImplPtr n;

	while ( p ) {
		n = p->next;
		p->setNoParent();
		p = n;
	}

}

// ============================================================================

DOM::DocumentImplPtr DOM::NodeImpl::getOwnerDocument() {

	NodeImplPtr p = this;

	while ( p && ! p->isDocument() ) {
		if ( ! p->hasParent )
			return (DocumentImplPtr)p->ownr;
		p = p->getParent();
	}

	return (DocumentImplPtr)p;

}

// ============================================================================

DOM::NodeImplPtr DOM::NodeImpl::insertBefore(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	refChild ) {

	if ( ! newChild ) {
		return 0;
	}

	if (newChild == refChild)
		return 0;

	if ( refChild && refChild->getParent() != this )
		throw InvalidArgument( "Node::insertBefore(): Invalid ref child!" );

	// "mark lists as dirty"
	nodeListTime++;

	// Special handling for inserting a fragment. We just insert
	// all elements of the fragment instead of the fragment itself.
	if (newChild->isDocumentFragment()) {
		// Fragment is empty ?
		if ( ! newChild->first )
			return newChild;

		// New parent
		NodeImplPtr n = newChild->first;
		while (n)  {
			n->setParent(this);
			n = n->next;
		}

		// Insert at the beginning ?
		if (!refChild || ! refChild->prev ) {
			if (first)
				first->prev = newChild->last;
			newChild->last->next = first;
			if (!last)
				last = newChild->last;
			first = newChild->first;
		} else {
			// Insert in the middle
			newChild->last->next = refChild;
			newChild->first->prev = refChild->prev;
			refChild->prev->next = newChild->first;
			refChild->prev = newChild->last;
		}

		// No need to increase the reference since DocumentFragment
		// does not decrease the reference.

		// Remove the nodes from the fragment
		newChild->first.kill();
		newChild->last.kill();
		return newChild;
	}

	if (newChild->getParent())
		newChild->getParent()->removeChild(newChild);

	newChild->setParent(this);

	if (!refChild) {
		if (first)
			first->prev = newChild;
		newChild->next = first;
		if (!last)
			last = newChild;
		first = newChild;
		return newChild;
	}

	if ( ! refChild->prev ) {
		if (first)
			first->prev = newChild;
		newChild->next = first;
		if (!last)
			last = newChild;
		first = newChild;
		return newChild;
	}

	newChild->next = refChild;
	newChild->prev = refChild->prev;
	refChild->prev->next = newChild;
	refChild->prev = newChild;

	return newChild;
}

DOM::NodeImplPtr DOM::NodeImpl::insertAfter(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	refChild ) {

	// Error check
	if (!newChild) {
		return 0;
	}

	// Error check
	if (newChild == refChild)
		return 0;

	// Error check
	if (refChild && refChild->getParent() != this)
		throw InvalidArgument( "Node::insertAfter(): Invalid ref child!" );

	// "mark lists as dirty"
	nodeListTime++;

	// Special handling for inserting a fragment. We just insert
	// all elements of the fragment instead of the fragment itself.
	if (newChild->isDocumentFragment()) {
		// Fragment is empty ?
		if ( ! newChild->first )
			return newChild;

		// New parent
		NodeImplPtr n = newChild->first;
		while (n) {
			n->setParent(this);
			n = n->next;
		}

		// Insert at the end
		if (!refChild || ! refChild->next ) {
			if (last)
				last->next = newChild->first;
			newChild->first->prev = last;
			if (!first)
				first = newChild->first;
			last = newChild->last;
		} else { // Insert in the middle
			newChild->first->prev = refChild;
			newChild->last->next = refChild->next;
			refChild->next->prev = newChild->last;
			refChild->next = newChild->first;
		}

		// No need to increase the reference since DocumentFragment
		// does not decrease the reference.

		// Remove the nodes from the fragment
		newChild->first.kill();
		newChild->last.kill();
		return newChild;
	}

	// Release new node from its current parent
	if (newChild->getParent())
		newChild->getParent()->removeChild(newChild);

	newChild->setParent(this);

	// Insert at the end
	if (!refChild) {
		if (last)
			last->next = newChild;
		newChild->prev = last;
		if (!first)
			first = newChild;
		last = newChild;
		return newChild;
	}

	if ( ! refChild->next ) {
		if (last)
			last->next = newChild;
		newChild->prev = last;
		if (!first)
			first = newChild;
		last = newChild;
		return newChild;
	}

	newChild->prev = refChild;
	newChild->next = refChild->next;
	refChild->next->prev = newChild;
	refChild->next = newChild;

	return newChild;

}

DOM::NodeImplPtr DOM::NodeImpl::replaceChild(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	oldChild ) {

	if (oldChild->getParent() != this)
		throw InvalidArgument();
	if (!newChild || !oldChild)
		throw InvalidArgument();
	if (newChild == oldChild)
		throw InvalidArgument();

	// mark lists as dirty
	nodeListTime++;

	// Special handling for inserting a fragment. We just insert
	// all elements of the fragment instead of the fragment itself.
	if (newChild->isDocumentFragment()) {
		// Fragment is empty ?
		if ( ! newChild->first )
			return newChild;

		// New parent
		NodeImplPtr n = newChild->first;
		while (n) {
			n->setParent(this);
			n = n->next;
		}


		if (oldChild->next)
			oldChild->next->prev = newChild->last;
		if (oldChild->prev)
			oldChild->prev->next = newChild->first;

		newChild->last->next = oldChild->next;
		newChild->first->prev = oldChild->prev;

		if (first == oldChild)
			first = newChild->first;
		if (last == oldChild)
			last = newChild->last;

		oldChild->setNoParent();
		oldChild->next = 0;
		oldChild->prev = 0;

		// No need to increase the reference since DocumentFragment
		// does not decrease the reference.

		// Remove the nodes from the fragment
		newChild->first = 0;
		newChild->last = 0;

		return oldChild;
	}

	// Release new node from its current parent
	if (newChild->getParent())
		newChild->getParent()->removeChild(newChild);

	newChild->setParent(this);

	if (oldChild->next)
		oldChild->next->prev = newChild;
	if (oldChild->prev)
		oldChild->prev->next = newChild;

	newChild->next = oldChild->next;
	newChild->prev = oldChild->prev;

	if (first == oldChild)
		first = newChild;
	if (last == oldChild)
		last = newChild;

	oldChild->setNoParent();
	oldChild->next = 0;
	oldChild->prev = 0;

	return oldChild;

}

DOM::NodeImplPtr DOM::NodeImpl::removeChild(
	const	NodeImplPtr &	oldChild ) {

	if ( oldChild->getParent() != this ) {
		throw InvalidArgument();
	}

	// "mark lists as dirty"
	nodeListTime++;

	// Perhaps oldChild was just created with "createElement" or that. In this case
	// its parent is Document but it is not part of the documents child list.
	if ( ! oldChild->next && ! oldChild->prev && first != oldChild)
		return 0;

	if (oldChild->next)
		oldChild->next->prev = oldChild->prev;
	if (oldChild->prev)
		oldChild->prev->next = oldChild->next;

	if (last == oldChild)
		last = oldChild->prev;
	if (first == oldChild)
		first = oldChild->next;

	oldChild->setNoParent();
	oldChild->next = 0;
	oldChild->prev = 0;

	return oldChild;

}

DOM::NodeImplPtr DOM::NodeImpl::appendChild(
	const	NodeImplPtr &	newChild ) {

	// No reference manipulation needed. Done in insertAfter.
	return insertAfter( newChild, 0 );

}

// ============================================================================

DOM::NodeImplPtr DOM::NodeImpl::getNamedItem(
	const	Buffer &	n ) const {

	NodeImplPtr p = first;

	while ( p ) {
		if ( p->getNodeName() == n ) {
			return p;
		}
		p = p->next;
	}

	return 0;

}

// ============================================================================

DOM::NodeImplPtr DOM::NodeImpl::cloneNode(
		Bool		deep ) {

	return new NodeImpl( nodeType, this, deep );

}

// ============================================================================

void DOM::NodeImpl::normalize() {

	NodeImplPtr p = first;
	TextImplPtr t;

	while ( p ) {
		if ( p->isText() ) {
			if ( t ) {
				NodeImplPtr tmp = p->next;
				t->appendData(p->getNodeValue());
				removeChild( p );
				p = tmp;
			}
			else {
				t = (TextImplPtr)p;
				p = p->next;
			}
		}
		else {
			p = p->next;
			t = 0;
		}
	}

}

// ============================================================================

void DOM::NodeImpl::clear() {

	first.kill();
	last.kill();

}

// ============================================================================

void DOM::NodeImpl::setNoParent() {

	ownr = ( hasParent ? getOwnerDocument() : DocumentImplPtr() );
	hasParent = false;

}

// ============================================================================

Bool DOM::NodeImpl::isAttr() const {

	return ( getNodeType() == AttributeNode );

}

Bool DOM::NodeImpl::isCDATASection() const {

	return ( getNodeType() == CDATASectionNode );

}

Bool DOM::NodeImpl::isDocumentFragment() const {

	return ( getNodeType() == DocumentFragmentNode );

}

Bool DOM::NodeImpl::isEntityReference() const {

	return ( getNodeType() == EntityReferenceNode );

}

Bool DOM::NodeImpl::isText() const {

	return ( getNodeType() == TextNode );

}

Bool DOM::NodeImpl::isEntity() const {

	return ( getNodeType() == EntityNode );

}

Bool DOM::NodeImpl::isNotation() const {

	return ( getNodeType() == NotationNode );

}

Bool DOM::NodeImpl::isProcessingInstruction() const {

	return ( getNodeType() == ProcessingInstructionNode );

}

Bool DOM::NodeImpl::isCharacterData() const {

	return ( getNodeType() == CharacterDataNode );

}

Bool DOM::NodeImpl::isComment() const {

	return ( getNodeType() == CommentNode );

}

// ============================================================================

void DOM::NodeImpl::save(
		IO::DevicePtr	s,
		int		depth,
		int		indent ) const {

	NodeImplPtr n = first;

	while ( n ) {
		n->save( s, depth, indent );
		n = n->next;
	}

}

// ============================================================================

