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
//	BFC.DOM.Attr.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.Attr.h"
#include "BFC.DOM.AttrImpl.h"
#include "BFC.DOM.Element.h"
#include "BFC.DOM.ElementImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

Attr::Attr() {



}

Attr::Attr(
	const	Attr		& x) :

	SObject(),
	Node( x ) {

}

Attr::Attr(
		AttrImplPtr	n ) :

	Node( n ) {

}

Attr & Attr::operator = (
	const	Attr		& x ) {

	Node::operator = ( x );

	return * this;

}

// ============================================================================

const Buffer & Attr::name() const {

	return ( impl ? impl->getNodeName() : emptyBuffer );

}

Bool Attr::specified() const {

	return ( impl ? ( ( AttrImplPtr )impl )->isSpecified() : false );

}

Element Attr::ownerElement() const {

	if (!impl || !impl->getParent()->isElement())
		return Element();
	return Element( ( ElementImplPtr )( impl->getParent() ) );

}

const Buffer & Attr::value() const {

	return ( impl ? impl->getNodeValue() : emptyBuffer );

}

void Attr::setValue(
	const	Buffer		& v ) {

	if ( ! impl ) {
		return;
	}

	impl->setNodeValue( v );

}

NodeType Attr::getNodeType() const {

	return AttributeNode;

}

// ============================================================================

