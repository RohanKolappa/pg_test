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
//	BFC.DOM.DocumentTypeImpl.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.DocumentTypeImpl.h"
#include "BFC.DOM.NamedNodeMapImpl.h"
#include "BFC.DOM.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( DOM, DocumentTypeImpl )

// ============================================================================

DOM::DocumentTypeImpl::DocumentTypeImpl(
	const	DocumentImplPtr	&	doc,
	const	NodeImplPtr &	parent ) :

	NodeImpl( DocumentTypeNode, doc, parent ),
	entities( 0, true, true ),
	notations( 0, true, true ) {

	init();

}

DOM::DocumentTypeImpl::DocumentTypeImpl(
	const	DocumentTypeImplPtr &	n,
		Bool		deep ) :

	NodeImpl( DocumentTypeNode, n, deep ),
	entities( 0, true, true ),
	notations( 0, true, true ) {

	init();

	// Refill the maps with our new children

	NodeImplPtr p = first;

	while (p) {
		if (p->isEntity())
			// Dont use normal insert function since we would create infinite recursion
			entities->map.add(p->getNodeName(), p, true);
		if (p->isNotation())
			// Dont use normal insert function since we would create infinite recursion
			notations->map.add(p->getNodeName(), p, true);
	}

}

// ============================================================================

void DOM::DocumentTypeImpl::init() {

	entities = new NamedNodeMapImpl( DocumentTypeImplPtr( this, false ) );
	notations = new NamedNodeMapImpl( DocumentTypeImplPtr( this, false ) );

	publicId.kill();
	systemId.kill();
	internalSubset.kill();

	entities->setAppendToParent(true);
	notations->setAppendToParent(true);

}

// ============================================================================

DOM::NodeImplPtr DOM::DocumentTypeImpl::cloneNode(
		Bool		deep ) {

	return new DocumentTypeImpl(this, deep);

}

// ============================================================================

DOM::NodeImplPtr DOM::DocumentTypeImpl::insertBefore(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	refChild ) {

	// Call the origianl implementation
	NodeImplPtr p = NodeImpl::insertBefore(newChild, refChild);
	// Update the maps
	if (p && p->isEntity())
		entities->map.add(p->getNodeName(), p, true);
	else if (p && p->isNotation())
		notations->map.add(p->getNodeName(), p, true);

	return p;

}

DOM::NodeImplPtr DOM::DocumentTypeImpl::insertAfter(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	refChild ) {

	// Call the origianl implementation
	NodeImplPtr p = NodeImpl::insertAfter(newChild, refChild);
	// Update the maps
	if (p && p->isEntity())
		entities->map.add(p->getNodeName(), p, true);
	else if (p && p->isNotation())
		notations->map.add(p->getNodeName(), p, true);

	return p;

}

DOM::NodeImplPtr DOM::DocumentTypeImpl::replaceChild(
	const	NodeImplPtr &	newChild,
	const	NodeImplPtr &	oldChild ) {

	// Call the origianl implementation
	NodeImplPtr p = NodeImpl::replaceChild(newChild, oldChild);
	// Update the maps
	if (p) {
		if (oldChild && oldChild->isEntity())
			entities->map.del(oldChild->getNodeName());
		else if (oldChild && oldChild->isNotation())
			notations->map.del(oldChild->getNodeName());

		if (p->isEntity())
			entities->map.add(p->getNodeName(), p, true);
		else if (p->isNotation())
			notations->map.add(p->getNodeName(), p, true);
	}

	return p;

}

DOM::NodeImplPtr DOM::DocumentTypeImpl::removeChild(
	const	NodeImplPtr &	oldChild ) {

	// Call the origianl implementation
	NodeImplPtr p = NodeImpl::removeChild( oldChild);
	// Update the maps
	if (p && p->isEntity())
		entities->map.del(p->getNodeName());
	else if (p && p->isNotation())
		notations->map.del(p ->getNodeName());

	return p;

}

DOM::NodeImplPtr DOM::DocumentTypeImpl::appendChild(
	const	NodeImplPtr &	newChild ) {

	return insertAfter(newChild, 0);

}

// ============================================================================

void DOM::DocumentTypeImpl::save(
		IO::DevicePtr	s,
		int,
		int		indent ) const {

	if (name.isEmpty())
		return;

	s->putBytes( "<!DOCTYPE " );
	s->putBytes( name );

	if (!publicId.isEmpty()) {
		s->putBytes( " PUBLIC " );
		s->putBytes( quotedValue(publicId) );
		if (!systemId.isEmpty()) {
			s->putBytes( " " );
			s->putBytes( quotedValue(systemId) );
		}
	}
	else if (!systemId.isEmpty()) {
		s->putBytes( " SYSTEM " );
		s->putBytes( quotedValue(systemId) );
	}

	if ( entities->length() > 0 || notations->length() > 0 ) {
		s->putBytes( " [\n" );

		for ( Uint32 i = 0 ; i < notations->map.getSize() ; i++ ) {
			notations->map.getValue( i )->save(s, 0, indent);
		}

		for ( Uint32 i = 0 ; i < entities->map.getSize() ; i++ ) {
			entities->map.getValue( i )->save(s, 0, indent);
		}

		s->putBytes( "]" );
	}

	s->putBytes( ">\n" );

}

// ============================================================================

