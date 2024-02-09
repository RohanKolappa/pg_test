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
//	BFC.DOM.Notation.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.Notation.h"
#include "BFC.DOM.NotationImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::Notation::Notation() {

}

DOM::Notation::Notation(
	const	Notation &	x ) :

	SObject	(),
	Node	( x ) {

}

DOM::Notation::Notation(
	const	NotationImplPtr &	n ) :

	Node	( n ) {

}

// ============================================================================

DOM::Notation & DOM::Notation::operator = (
	const	Notation &	x ) {

	Node::operator = ( x );

	return *this;

}

// ============================================================================

Buffer DOM::Notation::publicId() const {

	if ( ! impl ) {
		return Buffer();
	}

	return ( NotationImplPtr( impl )->getPublicId() );

}

Buffer DOM::Notation::systemId() const {

	if ( ! impl ) {
		return Buffer();
	}

	return ( NotationImplPtr( impl )->getSystemId() );

}

// ============================================================================

DOM::NodeType DOM::Notation::getNodeType() const {

	return NotationNode;

}

// ============================================================================

