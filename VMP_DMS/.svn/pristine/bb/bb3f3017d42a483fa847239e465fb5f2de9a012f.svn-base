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
//	BFC.DOM.NamedNodeMap.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NamedNodeMap_H_
#define _BFC_DOM_NamedNodeMap_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.Node.h"
#include "BFC.DOM.NamedNodeMapImplPtr.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief The DOM::NamedNodeMap class contains a collection of nodes
///	that can be accessed by name.
/// 
/// Note that DOM::NamedNodeMap does not inherit from DOM::NodeList.
/// DOM::NamedNodeMaps do not provide any specific node ordering.
/// Although nodes in a DOM::NamedNodeMap may be accessed by an ordinal
/// index, this is simply to allow a convenient enumeration of the
/// contents of a DOM::NamedNodeMap, and does not imply that the DOM
/// specifies an ordering of the nodes.
/// 
/// The DOM::NamedNodeMap is used in three places:
/// -	DOM::DocumentType::entities() returns a map of all entities
/// 	described in the DTD.
/// -	DOM::DocumentType::notations() returns a map of all notations
/// 	described in the DTD.
/// -	DOM::Node::getAttributes() returns a map of all attributes of an
/// 	element.
/// 
/// Items in the map are identified by the name which DOM::Node::name()
/// returns. Nodes are retrieved using namedItem(), namedItemNS() or
/// item(). New nodes are inserted with setNamedItem() or
/// setNamedItemNS() and removed with removeNamedItem() or
/// removeNamedItemNS(). Use contains() to see if an item with the
/// given name is in the named node map. The number of items is
/// returned by length().
/// 
/// Terminology: in this class we use "item" and "node"
/// interchangeably.
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL NamedNodeMap : virtual public SObject {

public:

/*!
	Constructs an empty named node map.
*/
	NamedNodeMap();

/*!
	Constructs a copy of \a n.
*/
	NamedNodeMap(const NamedNodeMap&);

/*!
	Assigns \a n to this named node map.
*/
	NamedNodeMap& operator= (const NamedNodeMap&);

/*!
	Returns true if \a n and this named node map are equal; otherwise
	returns false.
*/
	Bool operator== (const NamedNodeMap&) const;

/*!
	Returns true if \a n and this named node map are not equal;
	otherwise returns false.
*/
	Bool operator!= (const NamedNodeMap&) const;

	// DOM functions

	Node getNamedItem( const Buffer& name ) const;

	Node setNamedItem(const Node& newNode);

	Node removeNamedItem(const Buffer& name);

	Node getItem( int index ) const;

	Node getNamedItemNS( const Buffer& nsURI, const Buffer& localName ) const;

	Node setNamedItemNS(const Node& newNode);

	Node removeNamedItemNS(const Buffer& nsURI, const Buffer& localName);

	// DOM read only attributes

	Uint32 length() const;

	int count() const { return length(); }

	int size() const { return length(); }

	Bool isEmpty() const { return length() == 0; }

	// Qt extension

	Bool contains(const Buffer& name) const;

private:

	NamedNodeMapImplPtr impl;

	NamedNodeMap(NamedNodeMapImplPtr);

	friend class Node;
	friend class DocumentType;
	friend class Element;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NamedNodeMap_H_

// ============================================================================

