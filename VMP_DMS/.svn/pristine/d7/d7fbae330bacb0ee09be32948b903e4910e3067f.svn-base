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
//	BFC.DOM.Node.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Node_H_
#define _BFC_DOM_Node_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.IO.Device.h"

#include "BFC.DOM.NodeImplPtr.h"
#include "BFC.DOM.NodeType.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

class Attr;
class CDATASection;
class CharacterData;
class Comment;
class Document;
class DocumentFragment;
class DocumentType;
class Element;
class Entity;
class EntityReference; 
class NamedNodeMap;
class NodeList;
class Notation;
class ProcessingInstruction;
class Text;

// ============================================================================

/// \brief Base class for all the nodes in a DOM tree.
/// 
/// Many functions in the DOM return a DOM::Node.
/// 
/// You can find out the type of a node using isAttr(),
/// isCDATASection(), isDocumentFragment(), isDocument(),
/// isDocumentType(), isElement(), isEntityReference(), isText(),
/// isEntity(), isNotation(), isProcessingInstruction(),
/// isCharacterData() and isComment().
/// 
/// A DOM::Node can be converted into one of its subclasses using
/// toAttr(), toCDATASection(), toDocumentFragment(), toDocument(),
/// toDocumentType(), toElement(), toEntityReference(), toText(),
/// toEntity(), toNotation(), toProcessingInstruction(),
/// toCharacterData() or toComment(). You can convert a node to a null
/// node with clear().
/// 
/// Copies of the DOM::Node class share their data using explicit
/// sharing. This means that modifying one node will change all
/// copies. This is especially useful in combination with functions
/// which return a DOM::Node, e.g. firstChild(). You can make an
/// independent (deep) copy of the node with cloneNode().
/// 
/// Nodes are inserted with insertBefore(), insertAfter() or
/// appendChild(). You can replace one node with another using
/// replaceChild() and remove a node with removeChild().
/// 
/// To traverse nodes use firstChild() to get a node's first child (if
/// any), and nextSibling() to traverse. DOM::Node also provides
/// lastChild(), previousSibling() and parentNode(). To find the first
/// child node with a particular node name use namedItem().
/// 
/// To find out if a node has children use hasChildNodes() and to get
/// a list of all of a node's children use childNodes().
/// 
/// The node's name and value (the meaning of which varies depending
/// on its type) is returned by getNodeName() and getNodeValue()
/// respectively. The node's type is returned by getNodeType(). The
/// node's value can be set with setNodeValue().
/// 
/// The document to which the node belongs is returned by
/// getOwnerDocument().
/// 
/// Adjacent DOM::Text nodes can be merged into a single node with
/// normalize().
/// 
/// \l DOM::Element nodes have attributes which can be retrieved with
/// getAttributes().
/// 
/// DOM::Element and DOM::Attr nodes can have namespaces which can be
/// retrieved with namespaceURI(). Their local name is retrieved with
/// localName(), and their prefix with prefix(). The prefix can be set
/// with setPrefix().
/// 
/// You can write the XML representation of the node to a text stream
/// with save().
/// 
/// The following example looks for the first element in an XML document and
/// prints the names of all the elements that are its direct children.
/// \code
/// DOM::Document d;
/// d.setContent(someXML);
/// DOM::Node n = d.firstChild();
/// while (!n.isEmpty()) {
/// 	if (n.isElement()) {
/// 		DOM::Element e = n.toElement();
/// 		cout << "Element name: " << e.tagName() << endl;
/// 		break;
/// 	}
/// 	n = n.nextSibling();
/// }
/// \endcode
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Node : virtual public SObject {

public:

	/// \brief Constructs a new null DOM::Node.

	Node(
	);

	/// \brief Constructs a copy of \a n.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Node(
		const	Node &		n
	);

protected :

	/// \brief Constructs a new node for the data \a n.

	Node(
			NodeImplPtr	n
	);

public :

	/// \brief Assigns a copy of \a n to this DOM node.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Node & operator = (
		const	Node	& n
	);

	/// \brief Returns true iff \a n and this DOM node are equal.

	Bool operator == (
		const	Node	& n
	) const;

	/// \brief Returns true iff \a n and this DOM node are not equal.

	Bool operator != (
		const	Node	& n
	) const;

	/// \brief Inserts the node \a newChild before the child node
	///	\a refChild.
	///
	/// \a refChild must be a direct child of this node. If \a refChild is
	/// \link isEmpty() null\endlink then \a newChild is inserted as the
	/// node's first child.
	/// 
	/// If \a newChild is the child of another node, it is reparented to
	/// this node. If \a newChild is a child of this node, then its
	/// position in the list of children is changed.
	/// 
	/// If \a newChild is a DOM::DocumentFragment, then the children of the
	/// fragment are removed from the fragment and inserted before \a
	/// refChild.
	/// 
	/// Returns a new reference to \a newChild on success or a \link
	/// isEmpty() null node\endlink on failure.

	Node insertBefore(
		const	Node	& newChild,
		const	Node	& refChild
	);

	/// \brief Inserts the node \a newChild after the child node
	///	\a refChild.
	///
	/// \a refChild must be a direct child of this node. If \a refChild is
	/// \link isEmpty() null\endlink then \a newChild is appended as this
	/// node's last child.
	/// 
	/// If \a newChild is the child of another node, it is reparented to
	/// this node. If \a newChild is a child of this node, then its
	/// position in the list of children is changed.
	/// 
	/// If \a newChild is a DOM::DocumentFragment, then the children of the
	/// fragment are removed from the fragment and inserted after \a
	/// refChild.
	/// 
	/// Returns a new reference to \a newChild on success or a \link
	/// isEmpty() null node\endlink on failure.

	Node insertAfter(
		const	Node	& newChild,
		const	Node	& refChild
	);

	/// \brief Replaces \a oldChild with \a newChild.
	///
	/// \a oldChild must be a
	/// direct child of this node.
	/// 
	/// If \a newChild is the child of another node, it is reparented to
	/// this node. If \a newChild is a child of this node, then its
	/// position in the list of children is changed.
	/// 
	/// If \a newChild is a DOM::DocumentFragment, then \a oldChild is
	/// replaced by all of the children of the fragment.
	/// 
	/// Returns a new reference to \a oldChild on success or a \link
	/// isEmpty() null node\endlink an failure.

	Node replaceChild(
		const	Node	& newChild,
		const	Node	& oldChild
	);

	/// /brief Removes \a oldChild from the list of children.
	///
	/// \a oldChild must be
	/// a direct child of this node.
	/// 
	/// Returns a new reference to \a oldChild on success or a \link
	/// isEmpty() null node\endlink on failure.

	Node removeChild(
		const	Node	& oldChild
	);

	/// \brief Appends \a newChild as the node's last child.
	/// 
	/// If \a newChild is the child of another node, it is reparented to
	/// this node. If \a newChild is a child of this node, then its
	/// position in the list of children is changed.
	/// 
	/// If \a newChild is a DOM::DocumentFragment, then the children of the
	/// fragment are removed from the fragment and appended.
	/// 
	/// Returns a new reference to \a newChild.

	Node appendChild(
		const	Node	& newChild
	);

	/// \ brief Returns true iff the node has one or more children.

	Bool hasChildNodes(
	) const;

	/// \brief Creates a deep (not shallow) copy of the DOM::Node.
	/// 
	/// If \a deep is true, then the cloning is done recursively which
	/// means that all the node's children are deep copied too. If \a deep
	/// is false only the node itself is copied and the copy will have no
	/// child nodes.

	Node cloneNode(
			Bool		deep = true
	) const;

	/// Calling normalize() on an element converts all its children into a
	/// standard form. This means that adjacent DOM::Text objects will be
	/// merged into a single text object (DOM::CDATASection nodes are not
	/// merged).

	void normalize(
	);

	/// Returns true iff the DOM implementation implements the feature \a
	/// feature and this feature is supported by this node in the version
	/// \a version.

	Bool isSupported(
		const	Buffer		& feature,
		const	Buffer		& version
	) const;

	/// \brief Returns the name of the node.
	/// 
	/// The meaning of the name depends on the subclass:
	/// -	DOM::Attr: The name of the attribute
	/// -	DOM::CDATASection: The string "#cdata-section"
	/// -	DOM::Comment: The string "#comment"
	/// -	DOM::Document: The string "#document"
	/// -	DOM::DocumentFragment: The string "#document-fragment"
	/// -	DOM::DocumentType: The name of the document type
	/// -	DOM::Element: The tag name
	/// -	DOM::Entity: The name of the entity
	/// -	DOM::EntityReference: The name of the referenced entity
	/// -	DOM::Notation: The name of the notation
	/// -	DOM::ProcessingInstruction: The target of the processing instruction
	/// -	DOM::Text: The string "#text"

	Buffer getNodeName(
	) const;

	/// \brief Returns the type of the node.

	NodeType getNodeType() const;

	/// \brief Returns the parent node.
	///
	/// If this node has no parent, a null node
	/// is returned (i.e. a node for which isEmpty() returns true).

	Node parentNode() const;

	/// \brief Returns a list of all direct child nodes.
	/// 
	/// Most often you will call this function on a DOM::Element object.
	/// 
	/// For example, if the XML document looks like this:
	/// \code
	/// <body>
	/// <h1>Heading</h1>
	/// <p>Hello <b>you</b></p>
	/// </body>
	/// \endcode
	/// Then the list of child nodes for the "body"-element will contain
	/// the node created by the &lt;h1&gt; tag and the node created by the
	/// &lt;p&gt; tag.
	/// 
	/// The nodes in the list are not copied; so changing the nodes in the
	/// list will also change the children of this node.

	NodeList childNodes() const;

	/// \brief Returns the first child of the node.
	///
	/// If there is no child node, a
	/// \link isEmpty() null node\endlink is returned. Changing the
	/// returned node will also change the node in the document tree.

	Node firstChild() const;

	/// \brief Returns the last child of the node.
	///
	/// If there is no child node, a
	/// \link isEmpty() null node\endlink is returned. Changing the
	/// returned node will also change the node in the document tree.

	Node lastChild() const;

	/// \brief Returns the previous sibling in the document tree.
	///
	/// Changing the
	/// returned node will also change the node in the document tree.
	/// 
	/// For example, if you have XML like this:
	/// \code
	/// <h1>Heading</h1>
	/// <p>The text...</p>
	/// <h2>Next heading</h2>
	/// \endcode
	/// and this DOM::Node represents the &lt;p&gt; tag, previousSibling()
	/// will return the node representing the &lt;h1&gt; tag.

	Node previousSibling() const;

	/// \brief Returns the next sibling in the document tree.
	///
	/// Changing the
	/// returned node will also change the node in the document tree.
	/// 
	/// If you have XML like this:
	/// \code
	/// <h1>Heading</h1>
	/// <p>The text...</p>
	/// <h2>Next heading</h2>
	/// \endcode
	/// and this DOM::Node represents the <p> tag, nextSibling() will
	/// return the node representing the <h2> tag.

	Node nextSibling() const;

	/// \brief Returns a named node map of all attributes.
	///
	/// Attributes are only
	/// provided for \l{DOM::Element}s.
	/// 
	/// Changing the attributes in the map will also change the attributes
	/// of this DOM::Node.

	NamedNodeMap getAttributes() const;

	/// \brief Returns the document to which this node belongs.

	Document getOwnerDocument() const;

	/// \brief Returns the namespace URI of this node or an empty string if the
	///	node has no namespace URI.
	/// 
	/// Only nodes of type \link DOM::NodeType ElementNode\endlink or
	/// \link DOM::NodeType AttributeNode\endlink can have
	/// namespaces. A namespace URI must be specified at creation time and
	/// cannot be changed later.

	Buffer namespaceURI() const;

	/// \brief If the node uses namespaces, this function returns the local name
	///	of the node; otherwise it returns an empty string.
	/// 
	/// Only nodes of type \link DOM::NodeType ElementNode\endlink or
	/// \link DOM::NodeType AttributeNode\endlink can have
	/// namespaces. A namespace must have been specified at creation time;
	/// it is not possible to add a namespace afterwards.

	Buffer localName() const;

	/// \brief Returns true iff the node has attributes.

	Bool hasAttributes() const;

	/// \brief Returns the value of the node.
	/// 
	/// The meaning of the value depends on the subclass:
	/// \header: Name: Meaning
	/// -	DOM::Attr: The attribute value
	/// -	DOM::CDATASection: The content of the CDATA section
	/// -	DOM::Comment: The comment
	/// -	DOM::ProcessingInstruction: The data of the processing intruction
	/// -	DOM::Text: The text
	/// 
	/// All the other subclasses do not have a node value and will return
	/// an empty string.

	Buffer getNodeValue(
	) const;

	/// \brief Sets the node's value to \a v.

	void setNodeValue(const Buffer&);

	/// \brief Returns the namespace prefix of the node or an empty string if the
	///	node has no namespace prefix.
	/// 
	/// Only nodes of type \link DOM::NodeType ElementNode\endlink or
	/// \link DOM::NodeType AttributeNode\endlink can have
	/// namespaces. A namespace prefix must be specified at creation time.
	/// If a node was created with a namespace prefix, you can change it
	/// later with setPrefix().
	/// 
	/// If you create an element or attribute with
	/// DOM::Document::createElement() or DOM::Document::createAttribute(),
	/// the prefix will be an empty string. If you use
	/// DOM::Document::createElementNS() or
	/// DOM::Document::createAttributeNS() instead, the prefix will not be
	/// an empty string; but it might be an empty string if the name does
	/// not have a prefix.

	Buffer prefix() const;

	/// If the node has a namespace prefix, this function changes the
	/// namespace prefix of the node to \a pre. Otherwise this function
	/// does nothing.
	/// 
	/// Only nodes of type \link DOM::NodeType ElementNode\endlink or
	/// \link DOM::NodeType AttributeNode\endlink can have
	/// namespaces. A namespace prefix must have be specified at creation
	/// time; it is not possible to add a namespace prefix afterwards.

	void setPrefix(const Buffer& pre);

	/// \brief Returns true iff the node is an attribute.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Attribute; you can get the DOM::Attribute with
	/// toAttribute().

	Bool isAttr() const;

	/// \brief Returns true iff the node is a CDATA section.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::CDATASection; you can get the DOM::CDATASection with
	/// toCDATASection().

	Bool isCDATASection() const;

	/// \brief Returns true iff the node is a document fragment.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::DocumentFragment; you can get the DOM::DocumentFragment
	/// with toDocumentFragment().

	Bool isDocumentFragment() const;

	/// \brief Returns true iff the node is a document.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Document; you can get the DOM::Document with toDocument().

	Bool isDocument() const;

	/// \brief Returns true iff the node is a document type.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::DocumentType; you can get the DOM::DocumentType with
	/// toDocumentType().

	Bool isDocumentType() const;

	/// \brief Returns true iff the node is an element.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Element; you can get the DOM::Element with toElement().

	Bool isElement() const;

	/// \brief Returns true iff the node is an entity reference.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::EntityReference; you can get the DOM::EntityReference with
	/// toEntityReference().

	Bool isEntityReference() const;

	/// \brief Returns true iff the node is a text node.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Text; you can get the DOM::Text with toText().

	Bool isText() const;

	/// \brief Returns true iff the node is an entity.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Entity; you can get the DOM::Entity with toEntity().

	Bool isEntity() const;

	/// \brief Returns true iff the node is a notation.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Notation; you can get the DOM::Notation with toNotation().

	Bool isNotation() const;

	/// \brief Returns true iff the node is a processing instruction.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::ProcessingInstruction; you can get the
	/// QProcessingInstruction with toProcessingInstruction().

	Bool isProcessingInstruction() const;

	/// \brief Returns true iff the node is a character data node.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::CharacterData; you can get the DOM::CharacterData with
	/// toCharacterData().

	Bool isCharacterData() const;

	/// \brief Returns true if the node is a comment; otherwise returns false.
	/// 
	/// If this function returns true, it does not imply that this object
	/// is a DOM::Comment; you can get the DOM::Comment with toComment().

	Bool isComment() const;

	/// \brief Returns the first direct child node for which getNodeName() equals \a
	/// name.
	/// 
	/// If no such direct child exists, a \link isEmpty() null node\endlink
	/// is returned.

	Node getNamedItem( const Buffer& name ) const;

	/// \brief Returns true iff this node is null (i.e. if it has no type or
	///	contents).

	Bool isEmpty(
	) const;

	/// \brief Converts the node into a null node; if it was not a null node
	///	before, its type and contents are deleted.

	void clear(
	);

	/// \brief Converts a Node into a Attr.

	Attr toAttr(
	) const;

	/// \brief Converts a Node into a CDATASection.

	CDATASection toCDATASection(
	) const;

	/// \brief Converts a Node into a DocumentFragment.

	DocumentFragment toDocumentFragment(
	) const;

	/// \brief Converts a Node into a Document.

	Document toDocument(
	) const;

	/// \brief Converts a Node into a DocumentType.

	DocumentType toDocumentType(
	) const;

	/// \brief Converts a Node into a Element.

	Element toElement(
	) const;

	/// \brief Converts a Node into a EntityReference.

	EntityReference toEntityReference(
	) const;

	/// \brief Converts a Node into a Text.

	Text toText(
	) const;

	/// \brief Converts a Node into a Entity.

	Entity toEntity(
	) const;

	/// \brief Converts a Node into a Notation.

	Notation toNotation(
	) const;

	/// \brief Converts a Node into a ProcessingInstruction.

	ProcessingInstruction toProcessingInstruction(
	) const;

	/// \brief Converts a Node into a CharacterData.

	CharacterData toCharacterData(
	) const;

	/// \brief Converts a Node into a Comment.

	Comment toComment(
	) const;

	/// \brief Writes the XML representation of the node and all its children to
	///	the stream \a str. This function uses \a indent as the amount of
	///	space to indent the node.

	void save(
			IO::DevicePtr	oDevice,
			int
	) const;

	/// \brief Returns the first child element with tag name \a tagName
	///	if \a tagName is non-empty; otherwise returns the first child element.
	///
	/// Returns a null element if no such child exists.

	Element firstChildElement(const Buffer &tagName = Buffer()) const;

	/// \brief Returns the last child element with tag name \a tagName
	///	if \a tagName is non-empty; otherwise returns the first child element.
	///
	/// Returns a null element if no such child exists.

	Element lastChildElement(const Buffer &tagName = Buffer()) const;

	/// \brief Returns the previous sibilng element with tag name
	///	\a tagName if \a tagName is non-empty; otherwise returns any
	///	previous sibling element.
	///
	/// Returns a null element if no such sibling exists.

	Element previousSiblingElement(const Buffer &tagName = Buffer()) const;

	/// \brief Returns the next sibilng element with tag name 
	///	\a tagName if \a tagName is non-empty; otherwise returns any
	///	next sibling element.
	///
	/// Returns a null element if no such sibling exists.

	Element nextSiblingElement(const Buffer &taName = Buffer()) const;

	Node getParent(
	) const;

protected:

	NodeImplPtr	impl;

private:

	friend class Document;
	friend class DocumentType;
	friend class NodeList;
	friend class NamedNodeMap;

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Node_H_

// ============================================================================

