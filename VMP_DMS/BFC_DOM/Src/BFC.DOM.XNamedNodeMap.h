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
//	BFC.DOM.XNamedNodeMap.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XNamedNodeMap_H_
#define _BFC_DOM_XNamedNodeMap_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XNamedNodeMapPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.XNodePtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM NamedNodeMap.
///
/// Objects implementing the NamedNodeMap interface are used to represent
/// collections of nodes that can be accessed by name. Note that NamedNodeMap
/// does not inherit from NodeList; NamedNodeMaps are not maintained in any
/// particular order. Objects contained in an object implementing NamedNodeMap
/// may also be accessed by an ordinal index, but this is simply to allow
/// convenient enumeration of the contents of a NamedNodeMap, and does not imply
/// that the DOM specifies an order to these Nodes.
///
/// NamedNodeMap objects in the DOM are live.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XNamedNodeMap : virtual public SObject {

public :

#if 0

	/// \brief Retrieves a node specified by name.
	///
	/// \param name
	///	The nodeName of a node to retrieve.
	///
	/// \returns
	///	A Node (of any type) with the specified nodeName, or null if it
	///	does not identify any node in this map.
	///
	/// \since DOM Level 1

	virtual XNodePtr getNamedItem(
		const	Buffer &	name
	) = 0;

	/// \brief Adds a node using its nodeName attribute.
	///
	/// If a node with that name is already present in this map, it is
	/// replaced by the new one. Replacing a node by itself has no effect.
	///
	/// As the nodeName attribute is used to derive the name which the
	/// node must be stored under, multiple nodes of certain types (those
	/// that have a "special" string value) cannot be stored as the names
	/// would clash. This is seen as preferable to allowing nodes to be
	/// aliased.
	///
	/// \param arg
	///	A node to store in this map. The node will later be accessible
	///	using the value of its nodeName attribute.
	/// 
	/// \returns
	///	If the new Node replaces an existing node the replaced Node is
	///	returned, otherwise null is returned.
	///
	/// \throw Exception( WRONG_DOCUMENT_ERR )
	///	Raised if arg was created from a different document than the
	///	one that created this map.
	/// 
	/// \throw Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this map is readonly.
	/// 
	/// \throw Exception( INUSE_ATTRIBUTE_ERR )
	///	Raised if arg is an Attr that is already an attribute of
	///	another Element object. The DOM user must explicitly clone Attr
	///	nodes to re-use them in other elements.
	/// 
	/// \throw Exception( HIERARCHY_REQUEST_ERR )
	///	Raised if an attempt is made to add a node doesn't belong in
	///	this NamedNodeMap. Examples would include trying to insert
	///	something other than an Attr node into an Element's map of
	///	attributes, or a non-Entity node into the DocumentType's map of
	///	Entities.
	///
	/// \since DOM Level 1

	virtual XNodePtr setNamedItem(
			XNodePtr	arg
	) = 0;

	/// \brief Removes a node specified by name.
	///
	/// When this map contains the attributes attached to an element, if
	/// the removed attribute is known to have a default value, an
	/// attribute immediately appears containing the default value as well
	/// as the corresponding namespace URI, local name, and prefix when
	/// applicable.
	///
	/// \param name
	///	The nodeName of the node to remove.
	///
	/// \returns
	///	The node removed from this map if a node with such a name
	///	exists.
	///
	/// \throws Exception( NOT_FOUND_ERR )
	///	Raised if there is no node named name in this map.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this map is readonly.
	///
	/// \since DOM Level 1

	virtual XNodePtr removeNamedItem(
		const	Buffer &	name
	) = 0;

	/// \brief Returns the indexth item in the map.
	///
	/// If index is greater than or equal to the number of nodes in this
	/// map, this returns null.
	///
	/// \param index
	///	Index into this map.
	///
	/// \returns
	///	The node at the indexth position in the map, or null if that is
	///	not a valid index.
	///
	/// \since DOM Level 1

	virtual XNodePtr getItem(
		const	Uint32		index
	) = 0;

	/// \brief Returns the number of nodes in this map.
	///
	/// The range of valid child node indices is 0 to length-1 inclusive.
	///
	/// \since DOM Level 1

	virtual Uint32 getLength(
	) const = 0;

	/// \brief Retrieves a node specified by local name and namespace URI.
	///
	/// Per [XML Namespaces], applications must use the value null as the
	/// namespaceURI parameter for methods if they wish to have no
	/// namespace.
	///
	/// \param namespaceURI
	///	The namespace URI of the node to retrieve.
	///
	/// \param localName
	///	The local name of the node to retrieve.
	///
	/// \returns
	///	A Node (of any type) with the specified local name and
	///	namespace URI, or null if they do not identify any node in this
	///	map.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	May be raised if the implementation does not support the
	///	feature "XML" and the language exposed through the Document
	///	does not support XML Namespaces (such as [HTML 4.01]).
	///
	/// \since DOM Level 2

	virtual XNodePtr getNamedItemNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

	/// \brief Adds a node using its namespaceURI and localName.
	///
	/// If a node with that namespace URI and that local name is already
	/// present in this map, it is replaced by the new one. Replacing a
	/// node by itself has no effect.
	///
	/// Per [XML Namespaces], applications must use the value null as the
	/// namespaceURI parameter for methods if they wish to have no
	/// namespace.
	///
	/// \param arg
	///	A node to store in this map. The node will later be accessible
	///	using the value of its namespaceURI and localName attributes.
	/// 
	/// \returns
	///	If the new Node replaces an existing node the replaced Node is
	///	returned, otherwise null is returned.
	///
	/// \throws Exception( WRONG_DOCUMENT_ERR )
	///	Raised if arg was created from a different document than the
	///	one that created this map.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this map is readonly.
	/// 
	/// \throws Exception( INUSE_ATTRIBUTE_ERR )
	///	Raised if arg is an Attr that is already an attribute of
	///	another Element object. The DOM user must explicitly clone Attr
	///	nodes to re-use them in other elements.
	/// 
	/// \throws Exception( HIERARCHY_REQUEST_ERR )
	///	Raised if an attempt is made to add a node doesn't belong in
	///	this NamedNodeMap. Examples would include trying to insert
	///	something other than an Attr node into an Element's map of
	///	attributes, or a non-Entity node into the DocumentType's map of
	///	Entities.
	/// 
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	May be raised if the implementation does not support the
	///	feature "XML" and the language exposed through the Document
	///	does not support XML Namespaces (such as [HTML 4.01]).
	///
	/// \since DOM Level 2

	virtual XNodePtr setNamedItemNS(
			XNodePtr	arg
	) = 0;

	/// \brief Removes a node specified by local name and namespace URI.
	///
	/// A removed attribute may be known to have a default value when this
	/// map contains the attributes attached to an element, as returned by
	/// the attributes attribute of the Node interface. If so, an attribute
	/// immediately appears containing the default value as well as the
	/// corresponding namespace URI, local name, and prefix when
	/// applicable.
	///
	/// Per [XML Namespaces], applications must use the value null as the
	/// namespaceURI parameter for methods if they wish to have no
	/// namespace.
	///
	/// \param namespaceURI
	///	The namespace URI of the node to remove.
	///
	/// \param localName
	///	The local name of the node to remove.
	///
	/// \returns
	///	The node removed from this map if a node with such a local name
	///	and namespace URI exists.
	///
	/// \throws Exception( NOT_FOUND_ERR )
	///	Raised if there is no node with the specified namespaceURI and
	///	localName in this map.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this map is readonly.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	May be raised if the implementation does not support the
	///	feature "XML" and the language exposed through the Document
	///	does not support XML Namespaces (such as [HTML 4.01]).
	///
	/// \since DOM Level 2

	virtual XNodePtr removeNamedItemNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XNamedNodeMap_H_

// ============================================================================

