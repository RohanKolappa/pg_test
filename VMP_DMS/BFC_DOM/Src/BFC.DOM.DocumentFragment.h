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
//	BFC.DOM.DocumentFragment.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_DocumentFragment_H_
#define _BFC_DOM_DocumentFragment_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.DocumentFragmentImplPtr.h"
#include "BFC.DOM.Node.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/*!
	\brief The DOM::DocumentFragment class is a tree of DOM::Nodes which is not usually a complete DOM::Document.

	\ingroup BFC_DOM

	If you want to do complex tree operations it is useful to have a
	lightweight class to store nodes and their relations.
	DOM::DocumentFragment stores a subtree of a document which does not
	necessarily represent a well-formed XML document.

	DOM::DocumentFragment is also useful if you want to group several
	nodes in a list and insert them all together as children of some
	node. In these cases DOM::DocumentFragment can be used as a
	temporary container for this list of children.

	The most important feature of DOM::DocumentFragment is that it is
	treated in a special way by DOM::Node::insertAfter(),
	DOM::Node::insertBefore(), DOM::Node::replaceChild() and
	DOM::Node::appendChild(): instead of inserting the fragment itself, all
	the fragment's children are inserted.
*/

class BFC_DOM_DLL DocumentFragment : public Node {

public:

/*!
	Constructs an empty document fragment.
*/
	DocumentFragment();

/*!
	Constructs a copy of \a x.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	DocumentFragment(const DocumentFragment& x);

/*!
	Assigns \a x to this DOM document fragment.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	DocumentFragment& operator= (const DocumentFragment&);

	virtual NodeType getNodeType() const { return DocumentFragmentNode; }

private:

	DocumentFragment(DocumentFragmentImplPtr);

	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_DocumentFragment_H_

// ============================================================================

