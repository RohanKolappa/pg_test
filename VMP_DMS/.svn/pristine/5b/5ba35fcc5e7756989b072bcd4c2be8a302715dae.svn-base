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
//	BFC.DOM.XNode.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XNode_H_
#define _BFC_DOM_XNode_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XNodePtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.XDocumentPtr.h"
#include "BFC.DOM.XNamedNodeMapPtr.h"
#include "BFC.DOM.XNodeListPtr.h"
#include "BFC.DOM.UserDataHandlerPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Node.
///
/// The Node interface is the primary datatype for the entire Document Object
/// Model. It represents a single node in the document tree. While all objects
/// implementing the Node interface expose methods for dealing with children,
/// not all objects implementing the Node interface may have children. For
/// example, Text nodes may not have children, and adding children to such
/// nodes results in a DOMException being raised.
///
/// The attributes nodeName, nodeValue and attributes are included as a
/// mechanism to get at node information without casting down to the specific
/// derived interface. In cases where there is no obvious mapping of these
/// attributes for a specific nodeType (e.g., nodeValue for an Element or
/// attributes for a Comment), this returns null. Note that the specialized
/// interfaces may contain additional and more convenient mechanisms to get and
/// set the relevant information.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XNode : virtual public SObject {

public :

#if 0

	/// \brief An integer indicating which type of node this is.
	///
	/// Numeric codes up to 200 are reserved to W3C for possible future
	/// use.
	///
	/// The values of nodeName, nodeValue, and attributes vary according to
	/// the node type as follows:
	/// <table>
	/// <tr><td>Interface</td><td>nodeName</td><td>nodeValue</td><td>attributes</td></tr>
	/// <tr><td>Attr</td><td>same as Attr.name</td><td>same as Attr.value</td><td>null</td></tr>
	/// <tr><td>CDATASection</td><td>"#cdata-section"</td><td>same as CharacterData.data, the content of the CDATA Section</td><td>null</td></tr>
	/// <tr><td>Comment</td><td>"#comment"</td><td>same as CharacterData.data, the content of the comment</td><td>null</td></tr>
	/// <tr><td>Document</td><td>"#document"</td><td>null</td><td>null</td></tr>
	/// <tr><td>DocumentFragment</td><td>"#document-fragment"</td><td>null</td><td>null</td></tr>
	/// <tr><td>DocumentType</td><td>same as DocumentType.name</td><td>null</td><td>null</td></tr>
	/// <tr><td>Element</td><td>same as Element.tagName</td><td>null</td><td>NamedNodeMap</td></tr>
	/// <tr><td>Entity</td><td>entity name</td><td>null</td><td>null</td></tr>
	/// <tr><td>EntityReference</td><td>name of entity referenced</td><td>null</td><td>null</td></tr>
	/// <tr><td>Notation</td><td>notation name</td><td>null</td><td>null</td></tr>
	/// <tr><td>ProcessingInstruction</td><td>same as ProcessingInstruction.target</td><td>same as ProcessingInstruction.data</td><td>null</td></tr>
	/// <tr><td>Text</td><td>"#text"</td><td>same as CharacterData.data, the content of the text node</td><td>null</td></tr>
	/// </table>
	///
	/// \since DOM Level 1

	enum NodeType {
		ELEMENT_NODE                   = 1,	///< The node is an Element.
		ATTRIBUTE_NODE                 = 2,	///< The node is an Attr.
		TEXT_NODE                      = 3,	///< The node is a Text  node.
		CDATA_SECTION_NODE             = 4,	///< The node is a CDATASection.
		ENTITY_REFERENCE_NODE          = 5,	///< The node is an EntityReference.
		ENTITY_NODE                    = 6,	///< The node is an Entity.
		PROCESSING_INSTRUCTION_NODE    = 7,	///< The node is a ProcessingInstruction.
		COMMENT_NODE                   = 8,	///< The node is a Comment.
		DOCUMENT_NODE                  = 9,	///< The node is a Document.
		DOCUMENT_TYPE_NODE             = 10,	///< The node is a DocumentType.
		DOCUMENT_FRAGMENT_NODE         = 11,	///< The node is a DocumentFragment.
		NOTATION_NODE                  = 12	///< The node is a Notation.
	};

	/// \brief Returns the name of this node, depending on its type; see
	///	the table above.
	///
	/// \since DOM Level 1

	virtual const Buffer & getNodeName(
	) const = 0;

	/// \brief Returns the value of this node, depending on its type; see
	///	the table above.
	///
	/// \since DOM Level 1

	virtual const Buffer & getNodeValue(
	) const = 0;

	/// \brief Sets the value of this node, depending on its type: see the
	///	table above.
	///
	/// When it is defined to be null, setting it has no effect, including
	/// if the node is read-only.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised when the node is readonly and if it is not defined to be
	///	null.
	///
	/// \since DOM Level 1

	virtual void setNodeValue(
		const	Buffer &	pNodeValue
	) = 0;

	/// \brief Returns a code representing the type of the underlying
	///	object, as defined above.
	///
	/// \since DOM Level 1

	virtual NodeType getNodeType(
	) const = 0;

	/// \brief Returns the parent of this node.
	///
	/// All nodes, except Attr, Document, DocumentFragment, Entity, and
	/// Notation may have a parent. However, if a node has just been
	/// created and not yet added to the tree, or if it has been removed
	/// from the tree, this is null.
	///
	/// \since DOM Level 1

	virtual XNodePtr getParentNode(
	) = 0;

	/// \brief Returns a NodeList that contains all children of this node.
	///
	// If there are no children, this is a NodeList containing no nodes.
	///
	/// \since DOM Level 1

	virtual XNodeListPtr getChildNodes(
	) = 0;

	/// \brief Returns the first child of this node.
	///
	/// If there is no such node, this returns null.
	///
	/// \since DOM Level 1

	virtual XNodePtr getFirstChild(
	) = 0;

	/// \brief Returns the last child of this node.
	///
	/// If there is no such node, this returns null.
	///
	/// \since DOM Level 1

	virtual XNodePtr getLastChild(
	) = 0;

	/// \brief Returns the node immediately preceding this node.
	///
	/// If there is no such node, this returns null.
	///
	/// \since DOM Level 1

	virtual XNodePtr getPreviousSibling(
	) = 0;

	/// \brief Returns the node immediately following this node.
	///
	/// If there is no such node, this returns null.
	///
	/// \since DOM Level 1

	virtual XNodePtr getNextSibling(
	) = 0;

	/// \brief Returns a NamedNodeMap containing the attributes of this
	///	node (if it is an Element) or null otherwise.
	///
	/// \since DOM Level 1

	virtual XNamedNodeMapPtr getAttributes(
	) = 0;

	/// \brief Returns the Document object associated with this node.
	///
	/// This is also the Document object used to create new nodes. When
	/// this node is a Document or a DocumentType which is not used with
	/// any Document yet, this is null.
	///
	/// \since DOM Level 1

	virtual XDocumentPtr getOwnerDocument(
	) = 0;

	/// \brief Inserts the node newChild before the existing child node
	///	refChild.
	///
	/// If refChild is null, insert newChild at the end of the list of
	/// children.
	///
	/// If newChild is a DocumentFragment object, all of its children are
	/// inserted, in the same order, before refChild. If the newChild is
	/// already in the tree, it is first removed.
	/// 
	/// \note
	///	Inserting a node before itself is implementation dependent.
	///
	/// \param newChild
	///	The node to insert.
	///
	/// \param refChild
	///	The reference node, i.e., the node before which the new node
	///	must be inserted.
	/// 
	/// \returns
	///	The node being inserted.
	///
	/// \throws Exception( HIERARCHY_REQUEST_ERR )
	///	Raised if this node is of a type that does not allow children
	///	of the type of the newChild node, or if the node to insert is
	///	one of this node's ancestors or this node itself, or if this
	///	node is of type Document and the DOM application attempts to
	///	insert a second DocumentType or Element node.
	/// 
	/// \throws Exception( WRONG_DOCUMENT_ERR )
	///	Raised if newChild was created from a different document than the one that created this node.
	/// 
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly or if the parent of the node being inserted is readonly.
	/// 
	/// \throws Exception( NOT_FOUND_ERR )
	///	Raised if refChild is not a child of this node.
	/// 
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	if this node is of type Document, this exception might be raised if the DOM implementation doesn't support the insertion of a DocumentType or Element node.
	///
	/// \since DOM Level 3

	virtual XNodePtr insertBefore(
			XNodePtr	newChild, 
			XNodePtr	refChild
	) = 0;

/// Replaces the child node oldChild with newChild in the list of children, and returns the oldChild node.
/// If newChild is a DocumentFragment object, oldChild is replaced by all of the DocumentFragment children, which are inserted in the same order. If the newChild is already in the tree, it is first removed.
/// 
/// Note: Replacing a node with itself is implementation dependent.
/// Parameters
/// 
/// newChild of type Node
///     The new node to put in the child list.
/// oldChild of type Node
///     The node being replaced in the list.
/// 
/// Return Value
/// 
/// Node
/// 	
/// 
/// The node replaced.
/// Exceptions
/// 
/// DOMException
/// 	
/// 
/// HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not allow children of the type of the newChild node, or if the node to put in is one of this node's ancestors or this node itself, or if this node is of type Document and the result of the replacement operation would add a second DocumentType or Element on the Document node.
/// 
/// WRONG_DOCUMENT_ERR: Raised if newChild was created from a different document than the one that created this node.
/// 
/// NO_MODIFICATION_ALLOWED_ERR: Raised if this node or the parent of the new node is readonly.
/// 
/// NOT_FOUND_ERR: Raised if oldChild is not a child of this node.
/// 
/// NOT_SUPPORTED_ERR: if this node is of type Document, this exception might be raised if the DOM implementation doesn't support the replacement of the DocumentType child or Element child.
	///
	/// \since DOM Level 3

	virtual XNodePtr replaceChild(
			XNodePtr	newChild, 
			XNodePtr	oldChild
	) = 0;

	/// \brief Removes the child node indicated by oldChild from the list
	///	of children, and returns it.
	///
	/// \param oldChild
	///	The node being removed.
	/// 
	/// \returns
	///	The node removed.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.
	/// 
	/// \throws Exception( NOT_FOUND_ERR )
	///	Raised if oldChild is not a child of this node.
	/// 
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if this node is of type Document, this exception might
	///	be raised if the DOM implementation doesn't support the removal
	///	of the DocumentType child or the Element child.
	///
	/// \since DOM Level 3

	virtual XNodePtr removeChild(
			XNodePtr	oldChild
	) = 0;

	/// \brief Adds the node newChild to the end of the list of children of
	///	this node.
	///
	/// If the newChild is already in the tree, it is first removed.
	///
	/// \param newChild
	///	The node to add. If it is a DocumentFragment object, the entire
	///	contents of the document fragment are moved into the child list
	///	of this node.
	///
	/// \returns
	///	The node added.
	///
	/// \throws Exception( HIERARCHY_REQUEST_ERR )
	///	Raised if this node is of a type that does not allow children of the type of the newChild node, or if the node to append is one of this node's ancestors or this node itself, or if this node is of type Document and the DOM application attempts to append a second DocumentType or Element node.
	///
	/// \throws Exception( WRONG_DOCUMENT_ERR )
	///	Raised if newChild was created from a different document than the one that created this node.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly or if the previous parent of the node being inserted is readonly.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	If the newChild node is a child of the Document node, this exception might be raised if the DOM implementation doesn't support the removal of the DocumentType child or Element child.
	///
	/// \since DOM Level 1

	virtual XNodePtr appendChild(
			XNodePtr	newChild
	) = 0;

	/// \brief Returns whether this node has any children.
	///
	/// \returns
	///	Returns true if this node has any children, false otherwise.
	///
	/// \since DOM Level 1

	virtual Bool hasChildNodes(
	) const = 0;

	/// \brief Returns a duplicate of this node, i.e., serves as a generic
	///	copy constructor for nodes.
	///
	/// The duplicate node has no parent (parentNode is null) and no user
	/// data. User data associated to the imported node is not carried
	/// over. However, if any UserDataHandlers has been specified along
	/// with the associated data these handlers will be called with the
	/// appropriate parameters before this method returns.
	///
	/// Cloning an Element copies all attributes and their values,
	/// including those generated by the XML processor to represent
	/// defaulted attributes, but this method does not copy any children it
	/// contains unless it is a deep clone. This includes text contained in
	/// an the Element since the text is contained in a child Text node.
	/// Cloning an Attr directly, as opposed to be cloned as part of an
	/// Element cloning operation, returns a specified attribute (specified
	/// is true). Cloning an Attr always clones its children, since they
	/// represent its value, no matter whether this is a deep clone or not.
	/// Cloning an EntityReference automatically constructs its subtree if
	/// a corresponding Entity is available, no matter whether this is a
	/// deep clone or not. Cloning any other type of node simply returns a
	/// copy of this node.
	///
	/// Note that cloning an immutable subtree results in a mutable copy,
	/// but the children of an EntityReference clone are readonly. In
	/// addition, clones of unspecified Attr nodes are specified. And,
	/// cloning Document, DocumentType, Entity, and Notation nodes is
	/// implementation dependent.
	///
	/// \param deep
	///	If true, recursively clone the subtree under the specified
	///	node; if false, clone only the node itself (and its attributes,
	///	if it is an Element).
	///
	/// \returns
	///	The duplicate node.
	///
	/// \since DOM Level 1

	virtual XNodePtr cloneNode(
		const	Bool		deep
	) const = 0;

	/// \brief Puts all Text  nodes in the full depth of the sub-tree
	///	underneath this Node, including attribute nodes, into a
	///	"normal" form where only structure (e.g., elements, comments,
	///	processing instructions, CDATA sections, and entity references)
	///	separates Text nodes, i.e., there are neither adjacent Text
	///	nodes nor empty Text nodes.
	///
	/// This can be used to ensure that the DOM view of a document is the
	/// same as if it were saved and re-loaded, and is useful 
	/// when operations (such as XPointer [XPointer] lookups) that depend
	/// on a particular document tree structure are to be used. If the
	/// parameter "normalize-characters" of the DOMConfiguration object
	/// attached to the Node.ownerDocument is true, this method will also
	/// fully normalize the characters of the Text nodes.
	/// 
	/// \note
	///	In cases where the document contains CDATASections, the
	///	normalize operation alone may not be sufficient, since
	///	XPointers do not differentiate between Text nodes and
	///	CDATASection nodes.
	///
	/// \since DOM Level 3

	virtual void normalize(
	) = 0;

	/// \brief Tests whether the DOM implementation implements a specific
	///	feature and that feature is supported by this node, as
	///	specified in DOM Features.
	///
	/// \param feature
	///	The name of the feature to test.
	///
	/// \param version
	///	This is the version number of the feature to test.
	/// 
	/// \returns
	///	Returns true if the specified feature is supported on this
	///	node, false otherwise.
	///
	/// \since DOM Level 2

	virtual Bool isSupported(
		const	Buffer &	feature, 
		const	Buffer &	version
	) const = 0;

	/// \brief Returns the namespace URI of this node, or null if it is
	///	unspecified (see XML Namespaces).
	///
	/// This is not a computed value that is the result of a namespace
	/// lookup based on an examination of the namespace declarations in
	/// scope. It is merely the namespace URI given at creation time.
	///
	/// For nodes of any type other than ELEMENT_NODE and ATTRIBUTE_NODE
	/// and nodes created with a DOM Level 1 method, such as
	/// Document.createElement(), this is always null.
	///
	/// Note: Per the Namespaces in XML Specification [XML Namespaces] an
	/// attribute does not inherit its namespace from the element it is
	/// attached to. If an attribute is not explicitly given a namespace,
	/// it simply has no namespace.
	///
	/// \since DOM Level 2

	virtual const Buffer & getNamespaceURI(
	) const = 0;

	/// \brief Returns the namespace prefix of this node, or null if it is
	///	unspecified.
	///
	/// \since DOM Level 2

	virtual const Buffer & getPrefix(
	) const = 0;

	/// \brief Sets the namespace prefix of this node.
	///
	/// When it is defined to be null, setting it has no effect, including
	/// if the node is read-only.
	///
	/// Note that setting this attribute, when permitted, changes the
	/// nodeName attribute, which holds the qualified name, as well as the
	/// tagName and name attributes of the Element and Attr interfaces,
	/// when applicable.
	///
	/// Setting the prefix to null makes it unspecified, setting it to an
	/// empty string is implementation dependent.
	///
	/// Note also that changing the prefix of an attribute that is known to
	/// have a default value, does not make a new attribute with the
	/// default value and the original prefix appear, since the
	/// namespaceURI and localName do not change.
	///
	/// For nodes of any type other than ELEMENT_NODE and ATTRIBUTE_NODE
	/// and nodes created with a DOM Level 1 method, such as createElement
	/// from the Document interface, this is always null.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified prefix contains an illegal character
	///	according to the XML version in use specified in the
	///	Document.xmlVersion attribute.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised if this node is readonly.
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the specified prefix is malformed per the Namespaces
	///	in XML specification, if the namespaceURI of this node is null,
	///	if the specified prefix is "xml" and the namespaceURI of this
	///	node is different from "http://www.w3.org/XML/1998/namespace",
	///	if this node is an attribute and the specified prefix is
	///	"xmlns" and the namespaceURI of this node is different from
	///	"http://www.w3.org/2000/xmlns/", or if this node is an
	///	attribute and the qualifiedName of this node is "xmlns" [XML
	///	Namespaces].
	///
	/// \since DOM Level 2

	virtual void setPrefix(
		const	Buffer &	pPrefix
	) = 0;

	/// \brief Returns the local part of the qualified name of this node.
	///
	/// For nodes of any type other than ELEMENT_NODE and ATTRIBUTE_NODE
	/// and nodes created with a DOM Level 1 method, such as
	/// Document.createElement(), this is always null.
	///
	/// \since DOM Level 2

	virtual const Buffer & getLocalName(
	) const = 0;

	/// \brief Returns whether this node (if it is an element) has any
	///	attributes.
	///
	/// \returns
	///	Returns true if this node has any attributes, false otherwise.
	///
	/// \since DOM Level 2

	virtual Bool hasAttributes(
	) const = 0;

	/// \brief Returns the BaseURI, which is the absolute base URI of this
	///	node or null if the implementation wasn't able to obtain an
	///	absolute URI.
	///
	/// This value is computed as described in Base URIs. However, when the
	/// Document supports the feature "HTML" [DOM Level 2 HTML], the base
	/// URI is computed using first the value of the href attribute of the
	/// HTML BASE element if any, and the value of the documentURI
	/// attribute from the Document interface otherwise.
	///
	/// \since DOM Level 3

	virtual const Buffer & getBaseURI(
	) const = 0;

	/// \brief A bitmask indicating the relative document position of a node with respect to another node.
	/// 
	/// If the two nodes being compared are the same node, then no flags are set on the return.
	/// 
	/// Otherwise, the order of two nodes is determined by looking for common containers -- containers which contain both. A node directly contains any child nodes. A node also directly contains any other nodes attached to it such as attributes contained in an element or entities and notations contained in a document type. Nodes contained in contained nodes are also contained, but less-directly as the number of intervening containers increases.
	/// 
	/// If there is no common container node, then the order is based upon order between the root container of each node that is in no container. In this case, the result is disconnected and implementation-specific. This result is stable as long as these outer-most containing nodes remain in memory and are not inserted into some other containing node. This would be the case when the nodes belong to different documents or fragments, and cloning the document or inserting a fragment might change the order.
	/// 
	/// If one of the nodes being compared contains the other node, then the container precedes the contained node, and reversely the contained node follows the container. For example, when comparing an element against its own attribute or child, the element node precedes its attribute node and its child node, which both follow it.
	/// 
	/// If neither of the previous cases apply, then there exists a most-direct container common to both nodes being compared. In this case, the order is determined based upon the two determining nodes directly contained in this most-direct common container that either are or contain the corresponding nodes being compared.
	/// 
	/// If these two determining nodes are both child nodes, then the natural DOM order of these determining nodes within the containing node is returned as the order of the corresponding nodes. This would be the case, for example, when comparing two child elements of the same element.
	/// 
	/// If one of the two determining nodes is a child node and the other is not, then the corresponding node of the child node follows the corresponding node of the non-child node. This would be the case, for example, when comparing an attribute of an element with a child element of the same element.
	/// 
	/// If neither of the two determining node is a child node and one determining node has a greater value of nodeType than the other, then the corresponding node precedes the other. This would be the case, for example, when comparing an entity of a document type against a notation of the same document type.
	/// 
	/// If neither of the two determining node is a child node and nodeType is the same for both determining nodes, then an implementation-dependent order between the determining nodes is returned. This order is stable as long as no nodes of the same nodeType are inserted into or removed from the direct container. This would be the case, for example, when comparing two attributes of the same element, and inserting or removing additional attributes might change the order between existing attributes.
	///
	/// \since DOM Level 3

	enum DocumentPosition {
		/// The two nodes are disconnected. Order between disconnected nodes is always implementation-specific.
		DOCUMENT_POSITION_DISCONNECTED = 0x01,
		/// The second node precedes the reference node.
		DOCUMENT_POSITION_PRECEDING    = 0x02,
		/// The node follows the reference node.
		DOCUMENT_POSITION_FOLLOWING    = 0x04,
		/// The node contains the reference node. A node which contains is always preceding, too.
		DOCUMENT_POSITION_CONTAINS     = 0x08,
		/// The node is contained by the reference node. A node which is contained is always following, too.
		DOCUMENT_POSITION_CONTAINED_BY = 0x10,
		/// The determination of preceding versus following is implementation-specific.
		DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20
	};

	/// \brief Compares the reference node, i.e. the node on which this
	///	method is being called, with a node, i.e. the one passed as a
	///	parameter, with regard to their position in the document and
	///	according to the document order.
	///
	/// \param other
	///	The node to compare against the reference node.
	///
	/// \returns
	///	Value telling how the node is positioned relatively to the
	///	reference node.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	When the compared nodes are from different DOM implementations
	///	that do not coordinate to return consistent
	///	implementation-specific results.
	///
	/// \since DOM Level 3

	virtual DocumentPosition compareDocumentPosition(
			XNodeCPtr	other
	) const = 0;

	/// \brief Returns the text content of this node and its descendants.
	///
	/// No serialization is performed, the returned string does not contain
	/// any markup. No whitespace normalization is performed and the
	/// returned string does not contain the white spaces in element
	/// content (see the attribute Text.isElementContentWhitespace).
	///
	/// The string returned is made of the text content of this node
	/// depending on its type, as defined below:
	/// <table>
	/// <tr><td>Node type</td><td>Content</td></tr>
	/// <tr><td>ELEMENT_NODE, ATTRIBUTE_NODE, ENTITY_NODE, ENTITY_REFERENCE_NODE, DOCUMENT_FRAGMENT_NODE</td>
	///	<td>concatenation of the textContent attribute value of every child node, excluding COMMENT_NODE and PROCESSING_INSTRUCTION_NODE nodes. This is the empty string if the node has no children.</td></tr>
	/// <tr><td>TEXT_NODE, CDATA_SECTION_NODE, COMMENT_NODE, PROCESSING_INSTRUCTION_NODE</td>
	///	<td>nodeValue</td></tr>
	/// <tr><td>DOCUMENT_NODE, DOCUMENT_TYPE_NODE, NOTATION_NODE</td>
	///	<td>null</td></tr>
	/// </table>
	///
	/// \since DOM Level 3

	virtual const Buffer & getTextContent(
	) const = 0;

	/// \brief Sets the text content of this node.
	///
	/// When it is defined to be null, setting it has no effect.
	///
	/// Any possible children this node may have are removed and, if it the
	/// new string is not empty or null, replaced by a single Text node
	/// containing the string this attribute is set to.
	///
	/// No parsing is performed, the input string is taken as pure textual
	/// content.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised when the node is readonly.
	///
	/// \since DOM Level 3

	virtual void setTextContent(
		const	Buffer &	pTextContent
	) = 0;

	/// \brief Returns whether this node is the same node as the given one.
	///
	/// This method provides a way to determine whether two Node references
	/// returned by the implementation reference the same object. When two
	/// Node references are references to the same object, even if through
	/// a proxy, the references may be used completely interchangeably,
	/// such that all attributes have the same values and calling the same
	/// DOM method on either reference always has exactly the same effect.
	///
	/// \param other
	///	The node to test against.
	/// 
	/// \returns
	///	Returns true if the nodes are the same, false otherwise.
	///
	/// \since DOM Level 3

	virtual Bool isSameNode(
			XNodeCPtr	other
	) const = 0;

	/// \brief Look up the prefix associated to the given namespace URI,
	///	starting from this node.
	///
	/// The default namespace declarations are ignored by this method.
	///
	/// See Namespace Prefix Lookup for details on the algorithm used by
	/// this method.
	///
	/// \param namespaceURI
	///	The namespace URI to look for.
	/// 
	/// \returns
	///	Returns an associated namespace prefix if found or null if none
	///	is found. If more than one prefix are associated to the
	///	namespace prefix, the returned namespace prefix is
	///	implementation dependent.
	///
	/// \since DOM Level 3

	virtual const Buffer & lookupPrefix(
		const	Buffer &	namespaceURI
	) const = 0;

	/// \brief This method checks if the specified namespaceURI is the
	///	default namespace or not.
	///
	/// \param namespaceURI
	///	The namespace URI to look for.
	/// 
	/// \returns
	///	Returns true if the specified namespaceURI is the default
	///	namespace, false otherwise. 
	///
	/// \since DOM Level 3

	virtual Bool isDefaultNamespace(
		const	Buffer &	namespaceURI
	) const = 0;

	/// \brief Look up the namespace URI associated to the given prefix,
	///	starting from this node.
	///
	/// See Namespace URI Lookup for details on the algorithm used by this
	/// method.
	///
	/// \param prefix
	///	The prefix to look for. If this parameter is null, the method
	///	will return the default namespace URI if any.
	/// 
	/// \returns
	///	Returns the associated namespace URI or null if none is found.
	///
	/// \since DOM Level 3

	virtual const Buffer & lookupNamespaceURI(
		const	Buffer &	prefix
	) const = 0;

	/// \brief Tests whether two nodes are equal.
	///
	/// This method tests for equality of nodes, not sameness (i.e.,
	/// whether the two nodes are references to the same object) which can
	/// be tested with Node.isSameNode(). All nodes that are the same will
	/// also be equal, though the reverse may not be true.
	///
	/// Two nodes are equal if and only if the following conditions are
	/// satisfied:
	/// -	The two nodes are of the same type.
	/// -	The following string attributes are equal: nodeName, localName,
	///	namespaceURI, prefix, nodeValue. This is: they are both null,
	///	or they have the same length and are character for character
	///	identical.
	/// -	The attributes NamedNodeMaps are equal. This is: they are both
	///	null, or they have the same length and for each node that
	///	exists in one map there is a node that exists in the other map
	///	and is equal, although not necessarily at the same index.
	/// -	The childNodes NodeLists are equal. This is: they are both
	///	null, or they have the same length and contain equal nodes at
	///	the same index. Note that normalization can affect equality; to
	///	avoid this, nodes should be normalized before being compared.
	///
	/// For two DocumentType nodes to be equal, the following conditions
	/// must also be satisfied:
	/// -	The following string attributes are equal: publicId, systemId,
	///	internalSubset.
	/// -	The entities NamedNodeMaps are equal.
	/// -	The notations NamedNodeMaps are equal.
	/// 
	/// On the other hand, the following do not affect equality: the
	/// ownerDocument, baseURI, and parentNode attributes, the specified
	/// attribute for Attr nodes, the schemaTypeInfo attribute for Attr and
	/// Element nodes, the Text.isElementContentWhitespace attribute for
	/// Text nodes, as well as any user data or event listeners registered
	/// on the nodes.
	/// 
	/// \note
	///	As a general rule, anything not mentioned in the description
	///	above is not significant in consideration of equality checking.
	///	Note that future versions of this specification may take into
	///	account more attributes and implementations conform to this
	///	specification are expected to be updated accordingly.
	///
	/// \param arg
	///	The node to compare equality with.
	/// 
	/// \returns
	///	Returns true if the nodes are equal, false otherwise.
	///
	/// \since DOM Level 3

	virtual Bool isEqualNode(
			XNodeCPtr &	arg
	) const = 0;

	/// \brief This method returns a specialized object which implements
	///	the specialized APIs of the specified feature and version, as
	///	specified in DOM Features.
	///
	/// The specialized object may also be obtained by using
	/// binding-specific casting methods but is not necessarily expected
	/// to, as discussed in Mixed DOM Implementations. This method also
	/// allow the implementation to provide specialized objects which do
	/// not support the Node interface.
	///
	/// \param feature
	///	The name of the feature requested. Note that any plus sign "+"
	///	prepended to the name of the feature will be ignored since it
	///	is not significant in the context of this method.
	///
	/// \param version
	///	This is the version number of the feature to test.
	/// 
	/// \returns
	///	Returns an object which implements the specialized APIs of the
	///	specified feature and version, if any, or null if there is no
	///	object which implements interfaces associated with that
	///	feature. If the DOMObject returned by this method implements
	///	the Node interface, it must delegate to the primary core Node
	///	and not return results inconsistent with the primary core Node
	///	such as attributes, childNodes, etc.
	///
	/// \since DOM Level 3

	virtual SPtr getFeature(
		const	Buffer &	feature, 
		const	Buffer &	version
	) const = 0;

	/// \brief Associate an object to a key on this node.
	///
	/// The object can later be retrieved from this node by calling
	/// getUserData with the same key.
	///
	/// \param key
	///	The key to associate the object to.
	///
	/// \param data
	///	The object to associate to the given key, or null to remove any
	///	existing association to that key.
	///
	/// \param handler
	///	The handler to associate to that key, or null.
	/// 
	/// \returns
	///	Returns the DOMUserData previously associated to the given key
	///	on this node, or null if there was none.
	///
	/// \since DOM Level 3

	virtual SPtr setUserData(
		const	Buffer &	key,
			SPtr		data,
			UserDataHandlerPtr
					handler
	) = 0;

	/// \brief Retrieves the object associated to a key on a this node.
	///
	/// The object must first have been set to this node by calling
	/// setUserData with the same key.
	///
	/// \param key
	///	The key the object is associated to.
	/// 
	/// \returns
	///	Returns the DOMUserData associated to the given key on this
	///	node, or null if there was none.
	///
	/// \since DOM Level 3

	virtual SPtr getUserData(
		const	Buffer &	key
	) const = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XNode_H_

// ============================================================================

