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
//	BFC.DOM.Attr.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Attr_H_
#define _BFC_DOM_Attr_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.AttrImplPtr.h"
#include "BFC.DOM.Element.h"
#include "BFC.DOM.Node.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief The DOM::Attr class represents one attribute of a DOM::Element.
/// 
/// For example, the following piece of XML produces an element with
/// no children, but two attributes:
/// 
/// \code
/// <link href="http://www.trolltech.com" color="red" />
/// \endcode
/// 
/// You can access the attributes of an element with code like this:
/// 
/// \code
/// DOM::Element e = //...
/// //...
/// DOM::Attr a = e.getAttributeNode( "href" );
/// cout << a.value() << Buffer( "\n" );	// prints "http://www.trolltech.com"
/// a.setValue("http://doc.trolltech.com");	// change the node's attribute
/// DOM::Attr a2 = e.getAttributeNode( "href" );
/// cout << a2.value() << Buffer( "\n" );	// prints "http://doc.trolltech.com"
/// \endcode
/// 
/// This example also shows that changing an attribute received from
/// an element changes the attribute of the element. If you do not
/// want to change the value of the element's attribute you must
/// use cloneNode() to get an independent copy of the attribute.
/// 
/// DOM::Attr can return the name() and value() of an attribute. An
/// attribute's value is set with setValue(). If specified() returns
/// true the value was either set in the document or set with
/// setValue(); otherwise the value hasn't been set. The node this
/// attribute is attached to (if any) is returned by ownerElement().
/// 
/// For further information about the Document Object Model see
/// \l{http://www.w3.org/TR/REC-DOM-Level-1/} and
/// \l{http://www.w3.org/TR/DOM-Level-2-Core/}.
/// For a more general introduction of the DOM implementation see the
/// DOM::Document documentation.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL Attr : public Node {

public:

	/// \brief Constructs an empty attribute.

	Attr(
	);

	/// \brief Constructs a copy of \a x.
	///
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Attr(
		const	Attr		& x
	);

	/// \brief Assigns \a x to this DOM attribute.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Attr & operator = (
		const	Attr		& x
	);

	/// \brief Returns the attribute's name.

	const Buffer & name(
	) const;

	/// \brief Returns true iff the attribute has either been expicitly
	///	specified in the XML document or was set by the user with
	///	setValue().

	Bool specified(
	) const;

	/// \brief Returns the element node this attribute is attached to or a
	///	null node if this attribute is not attached to any element.

	Element ownerElement(
	) const;

	/// \brief Returns the value of the attribute or an empty string if the
	///	attribute has not been specified.

	const Buffer & value(
	) const;

	/// \brief Sets the attribute's value to \a v.

	void setValue(
		const	Buffer		& v
	);

	virtual NodeType getNodeType() const;

private:

	friend class Document;
	friend class Element;
	friend class Node;

	Buffer	emptyBuffer;

	Attr(
			AttrImplPtr
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Attr_H_

// ============================================================================

