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
//	BFC.DOM.Entity.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Entity_H_
#define _BFC_DOM_Entity_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.EntityImplPtr.h"
#include "BFC.DOM.Node.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/*!
	\brief The DOM::Entity class represents an XML entity.

	\ingroup BFC_DOM

	This class represents an entity in an XML document, either parsed
	or unparsed. Note that this models the entity itself not the
	entity declaration.

	DOM does not support editing entity nodes; if a user wants to make
	changes to the contents of an entity, every related
	DOM::EntityReference node must be replaced in the DOM tree by a
	clone of the entity's contents, and then the desired changes must
	be made to each of the clones instead. All the descendents of an
	entity node are read-only.

	An entity node does not have any parent.

	You can access the entity's publicId(), systemId() and
	notationName() when available.

	For further information about the Document Object Model see
	\l{http://www.w3.org/TR/REC-DOM-Level-1/}{Level 1} and
	\l{http://www.w3.org/TR/DOM-Level-2-Core/}{Level 2 Core}.
	For a more general introduction of the DOM implementation see the
	DOM::Document documentation.
*/

class BFC_DOM_DLL Entity : public Node
{
public:
/*!
	Constructs an empty entity.
*/
	Entity();
/*!
	Constructs a copy of \a x.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	Entity(const Entity& x);
/*!
	Assigns \a x to this DOM entity.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/
	Entity& operator= (const Entity&);

	// DOM read only attributes
/*!
	Returns the public identifier associated with this entity. If the
	public identifier was not specified an empty string is returned.
*/
	Buffer publicId() const;
/*!
	Returns the system identifier associated with this entity. If the
	system identifier was not specified an empty string is returned.
*/
	Buffer systemId() const;
/*!
	For unparsed entities this function returns the name of the
	notation for the entity. For parsed entities this function returns
	an empty string.
*/
	Buffer notationName() const;

	// Overridden from DOM::Node
	virtual NodeType getNodeType() const { return EntityNode; }

private:

	Entity(EntityImplPtr);

	friend class Node;
	friend class Document;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Entity_H_

// ============================================================================

