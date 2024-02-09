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
//	BFC.DOM.XEntity.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XEntity_H_
#define _BFC_DOM_XEntity_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XEntityPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Entity.
///
/// This interface represents a known entity, either parsed or unparsed, in an
/// XML document. Note that this models the entity itself not the entity
/// declaration.
///
/// The nodeName attribute that is inherited from Node contains the name of the
/// entity.
///
/// An XML processor may choose to completely expand entities before the
/// structure model is passed to the DOM; in this case there will be no
/// EntityReference nodes in the document tree.
///
/// XML does not mandate that a non-validating XML processor read and process
/// entity declarations made in the external subset or declared in parameter
/// entities. This means that parsed entities declared in the external subset
/// need not be expanded by some classes of applications, and that the
/// replacement text of the entity may not be available. When the replacement
/// text is available, the corresponding Entity node's child list represents
/// the structure of that replacement value. Otherwise, the child list is
/// empty.
///
/// DOM Level 3 does not support editing Entity nodes; if a user wants to make
/// changes to the contents of an Entity, every related EntityReference node
/// has to be replaced in the structure model by a clone of the Entity's
/// contents, and then the desired changes must be made to each of those clones
/// instead. Entity nodes and all their descendants are readonly.
///
/// An Entity node does not have any parent.
///
/// \note
///	If the entity contains an unbound namespace prefix, the namespaceURI of
///	the corresponding node in the Entity node subtree is null. The same is
///	true for EntityReference nodes that refer to this entity, when they are
///	created using the createEntityReference method of the Document
///	interface.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XEntity : virtual public XNode {

public :

	/// \brief Returns the public identifier associated with the entity if
	///	specified, and null otherwise.
	///
	/// \since DOM Level 1

	virtual const Buffer & getPublicId(
	) const = 0;

	/// \brief Returns the system identifier associated with the entity if
	///	specified, and null otherwise.
	///
	/// This may be an absolute URI or not.
	///
	/// \since DOM Level 1

	virtual const Buffer & getSystemId(
	) const = 0;

	/// \brief For unparsed entities, returns the name of the notation for
	///	the entity. For parsed entities, returns null.
	///
	/// \since DOM Level 1

	virtual const Buffer & getNotationName(
	) const = 0;

	/// \brief Returns an attribute specifying the encoding used for this
	///	entity at the time of parsing, when it is an external parsed
	///	entity.
	///
	/// This is null if it an entity from the internal subset or if it is
	/// not known.
	///
	/// \since DOM Level 3

	virtual const Buffer & getInputEncoding(
	) const = 0;

	/// \brief Returns an attribute specifying, as part of the text
	///	declaration, the encoding of this entity, when it is an
	///	external parsed entity. This is null otherwise.
	///
	/// \since DOM Level 3

	virtual const Buffer & getXMLEncoding(
	) const = 0;

	/// \brief Returns an attribute specifying, as part of the text
	///	declaration, the version number of this entity, when it is an
	///	external parsed entity. This is null otherwise.
	///
	/// \since DOM Level 3

	virtual const Buffer & getXMLVersion(
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XEntity_H_

// ============================================================================

