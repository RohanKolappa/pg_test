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
//	BFC.DOM.XText.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XText_H_
#define _BFC_DOM_XText_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XTextPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.XCharData.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Text.
///
/// The Text interface inherits from CharacterData and represents the textual
/// content (termed character data in XML) of an Element or Attr. If there is
/// no markup inside an element's content, the text is contained in a single
/// object implementing the Text interface that is the only child of the
/// element. If there is markup, it is parsed into the information items
/// (elements, comments, etc.) and Text nodes that form the list of children
/// of the element.
///
/// When a document is first made available via the DOM, there is only one Text
/// node for each block of text. Users may create adjacent Text nodes that
/// represent the contents of a given element without any intervening markup,
/// but should be aware that there is no way to represent the separations
/// between these nodes in XML or HTML, so they will not (in general) persist
/// between DOM editing sessions. The Node.normalize() method merges any such
/// adjacent Text objects into a single node for each block of text.
///
/// No lexical check is done on the content of a Text node and, depending on
/// its position in the document, some characters must be escaped during
/// serialization using character references; e.g. the characters "<&" if the
/// textual content is part of an element or of an attribute, the character
/// sequence "]]>" when part of an element, the quotation mark character " or
/// the apostrophe character ' when part of an attribute.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XText : virtual public XCharData {

public :

	/// \brief Breaks this node into two nodes at the specified offset,
	///	keeping both in the tree as siblings.
	///
	/// After being split, this node will contain all the content up to the
	/// offset point. A new node of the same type, which contains all the
	/// content at and after the offset point, is returned. If the original
	/// node had a parent node, the new node is inserted as the next
	/// sibling of the original node. When the offset is equal to the
	/// length of this node, the new node has no data.
	///
	/// \param offset
	///	The 16-bit unit offset at which to split, starting from 0.
	///
	/// \returns
	///	The new node, of the same type as this node.
	///
	/// \throws Exception( INDEX_SIZE_ERR )
	///	Raised if the specified offset is negative or greater than the
	///	number of 16-bit units in data.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.
	///
	/// \since DOM Level 1

	virtual XTextPtr splitText(
		const	Uint32		offset
	) = 0;

	/// \brief Returns whether this text node contains element content
	///	 whitespace, often abusively called "ignorable whitespace".
	///
	/// The text node is determined to contain whitespace in element
	/// content during the load of the document or if validation occurs
	/// while using Document.normalizeDocument().
	///
	/// \since DOM Level 3

	virtual Bool isElementContentWhitespace(
	) const = 0;

	/// \brief Returns all text of Text nodes logically-adjacent text nodes
	///	to this node, concatenated in document order.
	///
	/// \since DOM Level 3

	virtual const Buffer & getWholeText(
	) const = 0;

	/// \brief Replaces the text of the current node and all
	///	logically-adjacent text nodes with the specified text.
	///
	/// All logically-adjacent text nodes are removed including the current
	/// node unless it was the recipient of the replacement text.
	///
	/// This method returns the node which received the replacement text.
	/// The returned node is:
	/// -	null, when the replacement text is the empty string;
	/// -	the current node, except when the current node is read-only;
	/// -	a new Text node of the same type (Text or CDATASection) as the
	///	current node inserted at the location of the replacement.
	///
	/// Where the nodes to be removed are read-only descendants of an
	/// EntityReference, the EntityReference must be removed instead of the
	/// read-only nodes. If any EntityReference to be removed has
	/// descendants that are not EntityReference, Text, or CDATASection
	/// nodes, the replaceWholeText method must fail before performing any
	/// modification of the document, raising a DOM::Exception with the
	/// code NO_MODIFICATION_ALLOWED_ERR.
	///
	/// \param content
	///	The content of the replacing Text node.
	///
	/// \returns
	///	The Text node created with the specified content.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if one of the Text nodes being replaced is readonly.
	///
	/// \since DOM Level 3

	virtual XTextPtr replaceWholeText(
		const	Buffer &	content
	) = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XText_H_

// ============================================================================

