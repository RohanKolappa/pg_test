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
//	BFC.DOM.Element.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Element_H_
#define _BFC_DOM_Element_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.Attr.h"
#include "BFC.DOM.ElementImplPtr.h"
#include "BFC.DOM.NamedNodeMap.h"
#include "BFC.DOM.Node.h"
#include "BFC.DOM.NodeList.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief The DOM::Element class represents one element in the DOM tree.
/// 
/// Elements have a tagName() and zero or more attributes associated
/// with them. The tag name can be changed with setTagName().
/// 
/// Element attributes are represented by DOM::Attr objects that can
/// be queried using the attribute() and attributeNode() functions.
/// You can set attributes with the setAttribute() and
/// setAttributeNode() functions. Attributes can be removed with
/// removeAttribute(). There are namespace-aware equivalents to these
/// functions, i.e. setAttributeNS(), setAttributeNodeNS() and
/// removeAttributeNS().
/// 
/// If you want to access the text of a node use text(), e.g.
/// \code
/// DOM::Element e = //...
/// //...
/// Buffer s = e.text()
/// \endcode
/// The text() function operates recursively to find the text (since
/// not all elements contain text). If you want to find all the text
/// in all of a node's children, iterate over the children looking for
/// DOM::Text nodes, e.g.
/// \code
/// Buffer text;
/// DOM::Element element = doc.getDocumentElement();
/// for(DOM::Node n = element.firstChild(); !n.isEmpty(); n = n.nextSibling())
/// {
/// 	DOM::Text t = n.toText();
/// 	if (!t.isEmpty())
/// 		text += t.getData();
/// }
/// \endcode
/// Note that we attempt to convert each node to a text node and use
/// text() rather than using firstChild().toText().getData() or
/// n.toText().getData() directly on the node, because the node may not
/// be a text element.
/// 
/// You can get a list of all the decendents of an element which have
/// a specified tag name with elementsByTagName() or
/// elementsByTagNameNS().
/// 
/// To browse the elements of a dom document use firstChildElement(), lastChildElement(),
/// nextSiblingElement() and previousSiblingElement(). For example, to iterate over all
/// child elements called "entry" in a root element called "database", you can use:
/// \code
/// DOM::Document doc = // ...
/// DOM::Element root = doc.firstChildElement("database");
/// DOM::Element elt = root.firstChildElement("entry");
/// for (; !elt.isEmpty(); elt = elt.nextSiblingElelement("entry")) {
/// 	// ...
/// }
/// \endcode
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Element : public Node {

public:

/*!
	Constructs an empty element. Use the DOM::Document::createElement()
	function to construct elements with content.
*/

	Element(
	);

/*!
	Constructs a copy of \a x.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/

	Element(
		const	Element		& x
	);

/*!
	Assigns \a x to this DOM element.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/

	Element & operator = (
		const	Element		&
	);

	// DOM functions

/*!
	Returns the attribute called \a name. If the attribute does not
	exist \a defValue is returned.

	\sa setAttribute() attributeNode() setAttributeNode() attributeNS()
*/

	Buffer getAttribute( const	Buffer		& name, const	Buffer		& defValue = Buffer() ) const;

/*!
	Adds an attribute called \a name with value \a value. If an
	attribute with the same name exists, its value is replaced by \a
	value.

	\sa attribute() setAttributeNode() setAttributeNS()
*/

	void setAttribute(
		const	Buffer		& name,
		const	Buffer		& value
	);

	void setAttribute(
		const	Buffer		& name,
			Int64		value
	);

	void setAttribute(
		const	Buffer		& name,
			Uint64		value
	);

	void setAttribute(
		const	Buffer		& name,
			int		value
	) {
		setAttribute( name, Int64( value ) );
	}

	void setAttribute(
		const	Buffer		& name,
			Uint32		value
	) {
		setAttribute( name, Uint64( value ) );
	}

	void setAttribute(
		const	Buffer		& name,
			Float		value
	);

	void setAttribute(
		const	Buffer		& name,
			Double		value
	);

/*!
	Removes the attribute called name \a name from this element.

	\sa setAttribute() attribute() removeAttributeNS()
	*/

	void removeAttribute(
		const	Buffer		& name
	);

/*!
	Returns the DOM::Attr object that corresponds to the attribute
	called \a name. If no such attribute exists a \link
	DOM::Node::isEmpty() null attribute\endlink is returned.

	\sa setAttributeNode() attribute() setAttribute() attributeNodeNS()
*/

	Attr getAttributeNode( const	Buffer		& name );

/*!
	Adds the attribute \a newAttr to this element.

	If the element has another attribute that has the same name as \a
	newAttr, this function replaces that attribute and returns it;
	otherwise the function returns a \link DOM::Node::isEmpty() null
	attribute\endlink.

	\sa attributeNode() setAttribute() setAttributeNodeNS()
*/

	Attr setAttributeNode(
		const	Attr		& newAttr
	);

/*!
	Removes the attribute \a oldAttr from the element and returns it.

	\sa attributeNode() setAttributeNode()
*/

	Attr removeAttributeNode(
		const	Attr		& oldAttr
	);

/*!
	Returns a DOM::NodeList containing all descendent elements of this
	element that are called \a tagname. The order they are in the node
	list is the order they are encountered in a preorder traversal of
	the element tree.

	\sa elementsByTagNameNS() DOM::Document::elementsByTagName()
*/

	NodeList elementsByTagName(
		const	Buffer		& tagname
	) const;

/*!
	Returns true if this element has an attribute called \a name;
	otherwise returns false.
*/

	Bool hasAttribute(
		const	Buffer		& name
	) const;

/*!
	Returns the attribute with the local name \a localName and the
	namespace URI \a nsURI. If the attribute does not exist \a
	defValue is returned.

	\sa setAttributeNS() attributeNodeNS() setAttributeNodeNS() attribute()
*/

	Buffer getAttributeNS( const	Buffer		& nsURI, const	Buffer		& localName, const	Buffer		& defValue = Buffer() ) const;

/*!
	Adds an attribute with the qualified name \a qName and the
	namespace URI \a nsURI with the value \a value. If an attribute
	with the same local name and namespace URI exists, its prefix is
	replaced by the prefix of \a qName and its value is repaced by \a
	value.

	Although \a qName is the qualified name, the local name is used to
	decide if an existing attribute's value should be replaced.

	\sa attributeNS() setAttributeNodeNS() setAttribute()
*/

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
		const	Buffer		& value
	);

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
			int		value
	) {
		setAttributeNS( nsURI, qName, Int64( value ) );
	}

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
			Uint32		value
	) {
		setAttributeNS( nsURI, qName, Uint64( value ) );
	}

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
			Int64		value
	);

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
			Uint64		value
	);

	void setAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
			Double		value
	);

/*!
	Removes the attribute with the local name \a localName and the
	namespace URI \a nsURI from this element.

	\sa setAttributeNS() attributeNS() removeAttribute()
*/

	void removeAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& localName
	);

/*!
	Returns the DOM::Attr object that corresponds to the attribute
	with the local name \a localName and the namespace URI \a nsURI.
	If no such attribute exists a \l{DOM::Node::isEmpty()}{null
	attribute} is returned.

	\sa setAttributeNode() attribute() setAttribute()
*/

	Attr getAttributeNodeNS( const	Buffer		& nsURI, const	Buffer		& localName );

/*!
	Adds the attribute \a newAttr to this element.

	If the element has another attribute that has the same local name
	and namespace URI as \a newAttr, this function replaces that
	attribute and returns it; otherwise the function returns a \link
	DOM::Node::isEmpty() null attribute\endlink.

	\sa attributeNodeNS() setAttributeNS() setAttributeNode()
*/

	Attr setAttributeNodeNS(
		const	Attr		& newAttr
	);

/*!
	Returns a DOM::NodeList containing all the descendent elements of
	this element with the local name \a localName and the namespace
	URI \a nsURI. The order they are in the node list is the order
	they are encountered in a preorder traversal of the element tree.

	\sa elementsByTagName() DOM::Document::elementsByTagNameNS()
*/

	NodeList elementsByTagNameNS(
		const	Buffer		& nsURI,
		const	Buffer		& localName
	) const;

/*!
	Returns true if this element has an attribute with the local name
	\a localName and the namespace URI \a nsURI; otherwise returns
	false.
*/

	Bool hasAttributeNS(
		const	Buffer		& nsURI,
		const	Buffer		& localName
	) const;

	// DOM read only attributes

/*!
	Returns the tag name of this element. For an XML element like this:

	\code
	<img src="myimg.png">
	\endcode

	the tagname would return "img".

	\sa setTagName()
*/

	Buffer tagName(
	) const;

/*!
	Sets this element's tag name to \a name.

	\sa tagName()
*/

	void setTagName(
		const	Buffer		& name
	); // Qt extension

	// Overridden from DOM::Node

/*!
	Returns a DOM::NamedNodeMap containing all this element's attributes.

	\sa attribute() setAttribute() attributeNode() setAttributeNode()
*/

	virtual NamedNodeMap getAttributes() const;

	virtual NodeType getNodeType() const {
		return ElementNode;
	}

/*!
	Returns the element's text or an empty string.

	Example:
	\code
	<h1>Hello <b>Qt</b> <![CDATA[<xml is cool>]]></h1>
	\endcode

	The function text() of the DOM::Element for the &lt;h1&gt; tag,
	will return "Hello Qt &lt;xml is cool&gt;".

	Comments are ignored by this function. It only evaluates DOM::Text
	and DOM::CDATASection objects.
*/

	Buffer text(
	) const;

private:

	Element(
			ElementImplPtr
	);

	friend class Document;
	friend class Node;
	friend class Attr;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Element_H_

// ============================================================================

