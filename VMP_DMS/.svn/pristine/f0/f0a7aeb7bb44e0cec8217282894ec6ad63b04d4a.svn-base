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
//	BFC.DOM.Document.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Document_H_
#define _BFC_DOM_Document_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

namespace BFC {
namespace DOM {

class Document;

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.DOM.Attr.h"
#include "BFC.DOM.CDATASection.h"
#include "BFC.DOM.Comment.h"
#include "BFC.DOM.DocumentFragment.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.DocumentType.h"
#include "BFC.DOM.Element.h"
#include "BFC.DOM.EntityReference.h"
#include "BFC.DOM.ImplementationPtr.h"
#include "BFC.DOM.Node.h"
#include "BFC.DOM.NodeList.h"
#include "BFC.DOM.ProcessingInstruction.h"
#include "BFC.DOM.Text.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents an XML document.
/// 
/// The DOM::Document class represents the entire XML document.
/// Conceptually, it is the root of the document tree, and provides
/// the primary access to the document's data.
/// 
/// Since elements, text nodes, comments, processing instructions,
/// etc., cannot exist outside the context of a document, the document
/// class also contains the factory functions needed to create these
/// objects. The node objects created have an getOwnerDocument() function
/// which associates them with the document within whose context they
/// were created. The DOM classes that will be used most often are
/// DOM::Node, DOM::Document, DOM::Element and DOM::Text.
/// 
/// The parsed XML is represented internally by a tree of objects that
/// can be accessed using the various DOM:: classes. All DOM:: classes
/// only \e reference objects in the internal tree. The internal
/// objects in the DOM tree will get deleted once the last DOM::
/// object referencing them and the DOM::Document itself are deleted.
/// 
/// Creation of elements, text nodes, etc. is done using the various
/// factory functions provided in this class. Using the default
/// constructors of the DOM:: classes will only result in empty
/// objects that cannot be manipulated or inserted into the Document.
/// 
/// The DOM::Document class has several functions for creating document
/// data, for example, createElement(), createTextNode(),
/// createComment(), createCDATASection(),
/// createProcessingInstruction(), createAttribute() and
/// createEntityReference(). Some of these functions have versions
/// that support namespaces, i.e. createElementNS() and
/// createAttributeNS(). The createDocumentFragment() function is used
/// to hold parts of the document; this is useful for manipulating for
/// complex documents.
/// 
/// The entire content of the document is set with setContent(). This
/// function parses the string it is passed as an XML document and
/// creates the DOM tree that represents the document. The root
/// element is available using getDocumentElement(). The textual
/// representation of the document can be obtained using toString().
/// 
/// It is possible to insert a node from another document into the
/// document using importNode().
/// 
/// You can obtain a list of all the elements that have a particular
/// tag with elementsByTagName() or with elementsByTagNameNS().
/// 
/// The DOM:: classes are typically used as follows:
/// \code
///	DOM::Document doc("mydocument");
///	QFile file("mydocument.xml");
///	if (!file.open(QIODevice::ReadOnly))
///		return;
///	if (!doc.setContent(&file)) {
///		file.close();
///		return;
///	}
///	file.close();
///	
///	// print out the element names of all elements that are direct children
///	// of the outermost element.
///	DOM::Element docElem = doc.getDocumentElement();
///	
///	DOM::Node n = docElem.firstChild();
///	while(!n.isEmpty()) {
///		DOM::Element e = n.toElement(); // try to convert the node to an element.
///		if(!e.isEmpty()) {
///			cout << e.tagName() << Buffer( "\n" ); // the node really is an element.
///		}
///		n = n.nextSibling();
///	}
///	
///	// Here we append a new element to the end of the document
///	DOM::Element elem = doc.createElement("img");
///	elem.setAttribute("src", "myimage.png");
///	docElem.appendChild(elem);
/// \endcode
/// 
/// Once \c doc and \c elem go out of scope, the whole internal tree
/// representing the XML document is deleted.
/// 
/// To create a document using DOM use code like this:
/// \code
///	DOM::Document doc("MyML");
///	DOM::Element root = doc.createElement("MyML");
///	doc.appendChild(root);
///	
///	DOM::Element tag = doc.createElement("Greeting");
///	root.appendChild(tag);
///	
///	DOM::Text t = doc.createTextNode("Hello World");
///	tag.appendChild(t);
///	
///	Buffer xml = doc.toString();
/// \endcode
/// 
/// \ingroup BFC_DOM

class BFC_DOM_DLL Document : public Node {

public:

	/// \brief Constructs an empty document.

	Document(
	);

	/// \brief Creates a document and sets the name of the document type
	///	to \a name.

	explicit Document(
		const	Buffer &	name
	);

	/// \brief Creates a document with the document type \a doctype.

	explicit Document(
		const	DocumentType &	doctype
	);

	/// \brief Constructs a copy of \a x.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Document(
		const	Document &	x
	);

	/// \brief Assigns \a x to this DOM document.
	/// 
	/// The data of the copy is shared (shallow copy): modifying one node
	/// will also change the other. If you want to make a deep copy, use
	/// cloneNode().

	Document & operator = (
		const	Document &	x
	);

	/// \brief Creates a new element called \a tagName that can be inserted
	///	into the DOM tree, e.g. using Node::appendChild().
	/// 
	/// If \a tagName is not a valid XML name, the behavior of this
	/// function is governed by Implementation::InvalidDataPolicy.

	Element createElement(
		const	Buffer &	tagName
	);

	/// \brief Creates a new document fragment, that can be used to hold
	///	parts of the document, e.g. when doing complex manipulations of
	///	the document tree.

	DocumentFragment createDocumentFragment(
	);

	/// \brief Creates a text node for the string \a value that can be
	///	inserted into the document tree, e.g. using
	///	Node::appendChild().
	/// 
	/// If \a value contains characters which cannot be stored as character
	/// data of an XML document (even in the form of character references), the
	/// behavior of this function is governed by Implementation::InvalidDataPolicy.

	Text createTextNode(
		const	Buffer &	value
	);

	/// \brief Creates a new comment for the string \a value that can be inserted
	/// into the document, e.g. using Node::appendChild().
	/// 
	/// If \a value contains characters which cannot be stored in an XML comment,
	/// the behavior of this function is governed by Implementation::InvalidDataPolicy.

	Comment createComment(
		const	Buffer &	data
	);

	/// \brief Creates a new CDATA section for the string \a value that can be
	/// inserted into the document, e.g. using Node::appendChild().
	/// 
	/// If \a value contains characters which cannot be stored in a CDATA section,
	/// the behavior of this function is governed by
	/// Implementation::InvalidDataPolicy.

	CDATASection createCDATASection(
		const	Buffer &	data
	);

	/// Creates a new processing instruction that can be inserted into the
	/// document, e.g. using Node::appendChild(). This function sets
	/// the target for the processing instruction to \a target and the
	/// data to \a data.
	/// 
	/// If \a target is not a valid XML name, or data if contains characters which cannot
	/// appear in a processing instruction, the behavior of this function is governed by
	/// Implementation::InvalidDataPolicy.

	ProcessingInstruction createProcessingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	);

	/// Creates a new attribute called \a name that can be inserted into
	/// an element, e.g. using Element::setAttributeNode().
	/// 
	/// If \a name is not a valid XML name, the behavior of this function is governed by
	/// Implementation::InvalidDataPolicy.

	Attr createAttribute(
		const	Buffer &	name
	);

	/// Creates a new entity reference called \a name that can be inserted
	/// into the document, e.g. using Node::appendChild().
	/// 
	/// If \a name is not a valid XML name, the behavior of this function is governed by
	/// Implementation::InvalidDataPolicy.

	EntityReference createEntityReference(
		const	Buffer &	name
	);

	Entity createEntity(
		const	Buffer &	pEntityName,
		const	Buffer &	pPublicId,
		const	Buffer &	pSystemId,
		const	Buffer &	pNotationName,
		const	Buffer &	pValue
	);

	Notation createNotation(
		const	Buffer &	pNotationName,
		const	Buffer &	pPublicId,
		const	Buffer &	pSystemId
	);

	/// Returns a NodeList, that contains all the elements in the
	/// document with the name \a tagname. The order of the node list is
	/// the order they are encountered in a preorder traversal of the
	/// element tree.

	NodeList elementsByTagName(
		const	Buffer &	tagname
	) const;

	/// Imports the node \a importedNode from another document to this
	/// document. \a importedNode remains in the original document; this
	/// function creates a copy that can be used within this document.
	/// 
	/// This function returns the imported node that belongs to this
	/// document. The returned node has no parent. It is not possible to
	/// import Document and DocumentType nodes. In those cases
	/// this function returns a \link Node::isEmpty() null node\endlink.
	/// 
	/// If \a deep is true, this function imports not only the node \a
	/// importedNode but its whole subtree; if it is false, only the \a
	/// importedNode is imported. The argument \a deep has no effect on
	/// Attr and EntityReference nodes, since the descendents of
	/// Attr nodes are always imported and those of
	/// EntityReference nodes are never imported.
	/// 
	/// The behavior of this function is slightly different depending on
	/// the node types:
	/// . Attr: The owner element is set to 0 and the specified flag is
	/// 		set to true in the generated attribute. The whole subtree
	/// 		of \a importedNode is always imported for attribute nodes:
	/// 		\a deep has no effect.
	/// . Document: Document nodes cannot be imported.
	/// . DocumentFragment: If \a deep is true, this function imports the whole
	/// 		document fragment; otherwise it only generates an empty
	/// 		document fragment.
	/// . DocumentType: Document type nodes cannot be imported.
	/// . Element: Attributes for which Attr::specified() is true are
	/// 		also imported, other attributes are not imported. If \a
	/// 		deep is true, this function also imports the subtree of \a
	/// 		importedNode; otherwise it imports only the element node
	/// 		(and some attributes, see above).
	/// . Entity: Entity nodes can be imported, but at the moment there is
	/// 		no way to use them since the document type is read-only in
	/// 		DOM level 2.
	/// . EntityReference: Descendents of entity reference nodes are never imported:
	/// 		\a deep has no effect.
	/// . Notation: Notation nodes can be imported, but at the moment there is
	/// 		no way to use them since the document type is read-only in
	/// 		DOM level 2.
	/// . ProcessingInstruction: The target and value of the processing instruction is
	/// 		copied to the new node.
	/// . Text: The text is copied to the new node.
	/// . CDATASection: The text is copied to the new node.
	/// . Comment: The text is copied to the new node.

	Node importNode(
		const	Node &		importedNode,
			Bool		deep
	);

	/// Creates a new element with namespace support that can be inserted
	/// into the DOM tree. The name of the element is \a qName and the
	/// namespace URI is \a nsURI. This function also sets
	/// Node::prefix() and Node::localName() to appropriate values
	/// (depending on \a qName).
	/// 
	/// If \a qName is an empty string, returns a null element.

	Element createElementNS(
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	/// Creates a new attribute with namespace support that can be
	/// inserted into an element. The name of the attribute is \a qName
	/// and the namespace URI is \a nsURI. This function also sets
	/// Node::prefix() and Node::localName() to appropriate values
	/// (depending on \a qName).
	/// 
	/// If \a qName is not a valid XML name, the behavior of this function is governed by
	/// Implementation::InvalidDataPolicy.

	Attr createAttributeNS(
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	/// Returns a NodeList that contains all the elements in the
	/// document with the local name \a localName and a namespace URI of
	/// \a nsURI. The order of the node list is the order they are
	/// encountered in a preorder traversal of the element tree.

	NodeList elementsByTagNameNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName
	);

	/// Returns the element whose ID is equal to \a elementId. If no
	/// element with the ID was found, this function returns a \link
	/// Node::isEmpty() null element\endlink.
	/// 
	/// Since the Classes do not know which attributes are element
	/// IDs, this function returns always a \link Node::isEmpty() null
	/// element\endlink. This may change in a future version.

	Element elementById(
		const	Buffer &	elementId
	);

	/// \brief Returns the document type of this document.

	DocumentType getDocType(
	) const;

	/// \brief Returns an Implementation object.

	ImplementationCPtr getImplementation(
	) const;

	/// \brief Returns the root element of the document.

	Element getDocumentElement(
	) const;

	virtual NodeType getNodeType(
	) const;

	/// \brief Parses the XML document from the buffer \a text and sets it
	///	as the content of the document.
	/// 
	/// If \a namespaceProcessing is true, the parser recognizes
	/// namespaces in the XML file and sets the prefix name, local name
	/// and namespace URI to appropriate values. If \a namespaceProcessing
	/// is false, the parser does no namespace processing when it reads
	/// the XML file.
	/// 
	/// If \a namespaceProcessing is true, the function DOM::Node::prefix()
	/// returns a string for all elements and attributes. It returns an
	/// empty string if the element or attribute has no prefix.
	/// 
	/// If \a namespaceProcessing is false, the functions
	/// DOM::Node::prefix(), DOM::Node::localName() and
	/// DOM::Node::namespaceURI() return an empty string.
	/// 
	/// Entity references are handled as follows:
	/// -	References to internal general entities and character entities
	///	occuring in the content are included. The result is a DOM::Text
	///	node with the references replaced by their corresponding entity
	///	values.
	/// -	References to parameter entities occuring in the internal
	///	subset are included. The result is a DOM::DocumentType node
	///	which contains entity and notation declarations with the
	///	references replaced by their corresponding entity values.
	/// -	Any general parsed entity reference which is not defined in the
	///	internal subset and which occurs in the content is represented
	///	as a DOM::EntityReference node.
	/// -	Any parsed entity reference which is not defined in the
	///	internal subset and which occurs outside of the content is
	///	replaced with an empty string.
	/// -	Any unparsed entity reference is replaced with an empty string.

	void setContent(
		const	Buffer &	text,
			Bool		namespaceProcessing
	);

	/// \brief Converts the parsed document back to its textual
	///	representation.
	/// 
	/// This function uses \a indent as the amount of space to indent
	/// subelements.

	Buffer toString(
		const	Uint32		indent = 1
	) const;

protected :

	DocumentImplPtr getDocImpl(
	);

private :

	friend class Node;

	Document(
		const	DocumentImplPtr &
	);

	DocumentImplPtr	docImpl;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Document_H_

// ============================================================================

