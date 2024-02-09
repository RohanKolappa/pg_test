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
//	BFC.DOM.NamedNodeMap.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.NamedNodeMap.h"
#include "BFC.DOM.NamedNodeMapImpl.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

using namespace BFC;
using namespace BFC::DOM;

// ============================================================================

NamedNodeMap::NamedNodeMap() {

}

NamedNodeMap::NamedNodeMap(
	const	NamedNodeMap &	n ) :

	SObject	(),
	impl	( n.impl ) {

}

NamedNodeMap::NamedNodeMap(
		NamedNodeMapImplPtr	n ) :

	impl	( n, true ) {

}

NamedNodeMap& NamedNodeMap::operator = (
	const	NamedNodeMap &	n ) {

	impl = n.impl;

	return *this;

}

Bool NamedNodeMap::operator== (const NamedNodeMap& n) const
{
	return (impl == n.impl);
}

Bool NamedNodeMap::operator!= (const NamedNodeMap& n) const
{
	return (impl != n.impl);
}

/*!
	Returns the node called \a name.

	If the named node map does not contain such a node, a \link
	Node::isEmpty() null node\endlink is returned. A node's name is
	the name returned by Node::getNodeName().

	\sa setNamedItem() namedItemNS()
*/
Node NamedNodeMap::getNamedItem( const Buffer& name ) const
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->getNamedItem( name ));
}

/*!
	Inserts the node \a newNode into the named node map. The name used
	by the map is the node name of \a newNode as returned by
	Node::getNodeName().

	If the new node replaces an existing node, i.e. the map contains a
	node with the same name, the replaced node is returned.

	\sa namedItem() removeNamedItem() setNamedItemNS()
*/
Node NamedNodeMap::setNamedItem(const Node& newNode)
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->setNamedItem((NodeImplPtr)newNode.impl));
}

/*!
	Removes the node called \a name from the map.

	The function returns the removed node or a \link
	Node::isEmpty() null node\endlink if the map did not contain a
	node called \a name.

	\sa setNamedItem() namedItem() removeNamedItemNS()
*/
Node NamedNodeMap::removeNamedItem(const Buffer& name)
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->removeNamedItem(name));
}

/*!
	Retrieves the node at position \a index.

	This can be used to iterate over the map. Note that the nodes in
	the map are ordered arbitrarily.

	\sa length()
*/
Node NamedNodeMap::getItem( int index ) const
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->getItem( index ));
}

/*!
	Returns the node associated with the local name \a localName and
	the namespace URI \a nsURI.

	If the map does not contain such a node, a \link
	Node::isEmpty() null node\endlink is returned.

	\sa setNamedItemNS() namedItem()
*/
Node NamedNodeMap::getNamedItemNS( const Buffer& nsURI, const Buffer& localName ) const
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->getNamedItemNS( nsURI, localName ));
}

/*!
	Inserts the node \a newNode in the map. If a node with the same
	namespace URI and the same local name already exists in the map,
	it is replaced by \a newNode. If the new node replaces an existing
	node, the replaced node is returned.

	\sa namedItemNS() removeNamedItemNS() setNamedItem()
*/
Node NamedNodeMap::setNamedItemNS(const Node& newNode)
{
	if (!impl)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->setNamedItemNS((NodeImplPtr)newNode.impl));
}

/*!
	Removes the node with the local name \a localName and the
	namespace URI \a nsURI from the map.

	The function returns the removed node or a \link
	Node::isEmpty() null node\endlink if the map did not contain a
	node with the local name \a localName and the namespace URI \a
	nsURI.

	\sa setNamedItemNS() namedItemNS() removeNamedItem()
*/
Node NamedNodeMap::removeNamedItemNS(const Buffer& nsURI, const Buffer& localName)
{
	if (!impl)
		return Node();
	NodeImplPtr n = ( ( NamedNodeMapImplPtr )impl )->getNamedItemNS( nsURI, localName );
	if (!n)
		return Node();
	return Node(( ( NamedNodeMapImplPtr )impl )->removeNamedItem(n->name));
}

/*!
	Returns the number of nodes in the map.

	\sa item()
*/
Uint32 NamedNodeMap::length() const
{
	if (!impl)
		return 0;
	return ( ( NamedNodeMapImplPtr )impl )->length();
}

/*!
	\fn Bool NamedNodeMap::isEmpty() const

	Returns true if the map is empty; otherwise returns false. This function is
	provided for Qt API consistency.
*/

/*!
	\fn int NamedNodeMap::count() const

	This function is provided for Qt API consistency. It is equivalent to length().
*/

/*!
	\fn int NamedNodeMap::size() const

	This function is provided for Qt API consistency. It is equivalent to length().
*/

/*!
	Returns true if the map contains a node called \a name; otherwise
	returns false.
*/
Bool NamedNodeMap::contains(const Buffer& name) const
{
	if (!impl)
		return false;
	return ( ( NamedNodeMapImplPtr )impl )->contains(name);
}

// ============================================================================

