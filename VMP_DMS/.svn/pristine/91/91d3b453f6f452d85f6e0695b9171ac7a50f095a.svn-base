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
//	BFC.DOM.EntityReference.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_EntityReference_H_
#define _BFC_DOM_EntityReference_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.EntityReferenceImplPtr.h"
#include "BFC.DOM.Node.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/*!
	\brief The DOM::EntityReference class represents an XML entity reference.

	\ingroup BFC_DOM

	A DOM::EntityReference object may be inserted into the DOM tree
	when an entity reference is in the source document, or when the
	user wishes to insert an entity reference.

	Note that character references and references to predefined
	entities are expanded by the XML processor so that characters are
	represented by their Unicode equivalent rather than by an entity
	reference.

	Moreover, the XML processor may completely expand references to
	entities while building the DOM tree, instead of providing
	DOM::EntityReference objects.

	If it does provide such objects, then for a given entity reference
	node, it may be that there is no entity node representing the
	referenced entity; but if such an entity exists, then the child
	list of the entity reference node is the same as that of the
	entity	node. As with the entity node, all descendents of the
	entity reference are read-only.

	For further information about the Document Object Model see
	\l{http://www.w3.org/TR/REC-DOM-Level-1/}{Level 1} and
	\l{http://www.w3.org/TR/DOM-Level-2-Core/}{Level 2 Core}.
	For a more general introduction of the DOM implementation see the
	DOM::Document documentation.
*/

class BFC_DOM_DLL EntityReference : public Node {

public:

/*!
	Constructs an empty entity reference. Use
	DOM::Document::createEntityReference() to create a entity reference
	with content.
*/
	EntityReference(
	);

/*!
	Constructs a copy of \a x.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	EntityReference(
		const	EntityReference	& x
	);

/*!
	Assigns \a x to this entity reference.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	EntityReference & operator = (
		const	EntityReference	&
	);

	virtual NodeType getNodeType() const {
		return EntityReferenceNode;
	}

private:

	EntityReference(
			EntityReferenceImplPtr
	);

	friend class Document;
	friend class Node;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_EntityReference_H_

// ============================================================================

