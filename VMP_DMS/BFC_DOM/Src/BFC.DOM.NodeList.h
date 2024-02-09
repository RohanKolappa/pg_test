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
//	BFC.DOM.NodeList.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NodeList_H_
#define _BFC_DOM_NodeList_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.Node.h"
#include "BFC.DOM.NodeListImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief A list of DOM::Node objects.
/// 
/// Lists can be obtained by DOM::Document::elementsByTagName() and
/// DOM::Node::childNodes(). The Document Object Model (DOM) requires
/// these lists to be "live": whenever you change the underlying
/// document, the contents of the list will get updated.
/// 
/// You can get a particular node from the list with item(). The
/// number of items in the list is returned by length().
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL NodeList : virtual public SObject {

public:

	/// \brief Creates a new empty node list.

	NodeList(
	);

	/// \brief Constructs a copy of \a pOther.

	NodeList(
		const	NodeList &	pOther
	);

	/// \brief Assigns \a pOther to this node list.

	NodeList & operator = (
		const	NodeList &	pOther
	);

	/// \brief Returns true if the node list \a pOther and this node list
	///	are equal; otherwise returns false.

	Bool operator == (
		const	NodeList &	pOther
	) const;

	/// \brief Returns true the node list \a pOther and this node list are
	///	not equal; otherwise returns false.

	Bool operator != (
		const	NodeList &	pOther
	) const;

	/// \brief Returns the node at position \a index.
	/// 
	/// If \a index >= length() then a null
	/// node is returned (i.e. a node for which DOM::Node::isEmpty() returns
	/// true).

	Node getItem(
		const	Uint32		index
	) const;

	/// \brief Synonym of item().

	Node at(
		const	Uint32		index
	) const {
		return getItem( index );
	}

	/// \brief Returns the number of nodes in the list.

	Uint32 length(
	) const;

private:

	NodeListImplPtr	impl;

	NodeList(
		const	NodeListImplPtr &
	);

	friend class Node;
	friend class Element;
	friend class Document;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NodeList_H_

// ============================================================================

