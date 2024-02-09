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
//	BFC.DOM.XDocument.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XDocument_H_
#define _BFC_DOM_XDocument_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XDocumentPtr.h"

// ============================================================================

#include "BFC.DOM.XAttrPtr.h"
#include "BFC.DOM.XCDATASectionPtr.h"
#include "BFC.DOM.XCommentPtr.h"
#include "BFC.DOM.ConfigPtr.h"
#include "BFC.DOM.XDocFragmentPtr.h"
#include "BFC.DOM.XDocTypePtr.h"
#include "BFC.DOM.XElementPtr.h"
#include "BFC.DOM.XEntityRefPtr.h"
#include "BFC.DOM.ImplementationPtr.h"
#include "BFC.DOM.XNode.h"
#include "BFC.DOM.XNodeListPtr.h"
#include "BFC.DOM.XProcInstrPtr.h"
#include "BFC.DOM.XTextPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Document.
///
/// The Document interface represents the entire HTML or XML document.
/// Conceptually, it is the root of the document tree, and provides the primary
/// access to the document's data.
///
/// Since elements, text nodes, comments, processing instructions, etc. cannot
/// exist outside the context of a Document, the Document interface also
/// contains the factory methods needed to create these objects. The Node
/// objects created have a ownerDocument attribute which associates them with
/// the Document within whose context they were created.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XDocument : virtual public XNode {

public :

#if 0

	/// \brief Returns the Document Type Declaration (see DocumentType)
	///	associated with this document.
	///
	/// For XML documents without a document type declaration this returns
	/// null. For HTML documents, a DocumentType object may be returned,
	/// independently of the presence or absence of document type
	/// declaration in the HTML document.
	///
	/// This provides direct access to the DocumentType node, child node of
	/// this Document. This node can be set at document creation time and
	/// later changed through the use of child nodes manipulation methods,
	/// such as Node::insertBefore(), or Node::replaceChild(). Note,
	/// however, that while some implementations may instantiate different
	/// types of Document objects supporting additional features than the
	/// "Core", such as "HTML" [DOM Level 2 HTML], based on the
	/// DocumentType specified at creation time, changing it afterwards is
	/// very unlikely to result in a change of the features supported.
	///
	/// \since DOM Level 1

	virtual XDocTypeCPtr getDocumentType(
	) const = 0;

	/// \brief Returns the Implementation object that handles this
	///	document.
	///
	/// A DOM application may use objects from multiple implementations.
	///
	/// \since DOM Level 1

	virtual ImplementationCPtr getImplementation(
	) const = 0;

	/// \brief Returns the child node that is the document element of the
	///	document.
	///
	/// \since DOM Level 1

	virtual XElementPtr getDocumentElement(
	) = 0;

	/// \brief Creates an element of the type specified.
	///
	/// Note that the instance returned implements the Element interface,
	/// so attributes can be specified directly on the returned object. In
	/// addition, if there are known attributes with default values, Attr
	/// nodes representing them are automatically created and attached to
	/// the element.
	///
	/// To create an element with a qualified name and namespace URI, use
	/// the createElementNS method.
	///
	/// \param tagName
	///	The name of the element type to instantiate. For XML, this is
	///	case-sensitive, otherwise it depends on the case-sensitivity of
	///	the markup language in use. In that case, the name is mapped to
	///	the canonical form of that markup by the DOM implementation.
	///
	/// \returns
	///	A new Element object with the nodeName attribute set to
	///	tagName, and localName, prefix, and namespaceURI set to null.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	 Raised if the specified name is not an XML name according to
	///	the XML version in use specified in the Document.xmlVersion
	///	attribute.
	///
	/// \since DOM Level 1

	virtual XElementPtr createElement(
		const	Buffer &	tagName
	) = 0;

	/// \brief Creates an empty DocumentFragment object.
	///
	/// \returns
	///	A new DocumentFragment.
	///
	/// \since DOM Level 1

	virtual XDocFragmentPtr createDocumentFragment(
	) = 0;

	/// \brief Creates a Text node given the specified string.
	///
	/// \param data
	///	The data for the node.
	///
	/// \returns
	///	The new Text object.
	///
	/// \since DOM Level 1

	virtual XTextPtr createTextNode(
		const	Buffer &	data
	) = 0;

	/// \brief Creates a Comment node given the specified string.
	///
	/// \param data
	///	The data for the node.
	///
	/// \returns
	///	The new Comment object.
	///
	/// \since DOM Level 1

	virtual XCommentPtr createComment(
		const	Buffer &	data
	) = 0;

	/// \brief Creates a CDATASection node whose value is the specified
	///	string.
	///
	/// \param data
	///	The data for the CDATASection contents.
	///
	/// \returns
	///	The new CDATASection object.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if this document is an HTML document.
	///
	/// \since DOM Level 1

	virtual XCDATASectionPtr createCDATASection(
		const	Buffer &	data
	) = 0;

	/// \brief Creates a ProcessingInstruction node given the specified
	///	name and data strings.
	///
	/// \param target
	///	The target part of the processing instruction. Unlike
	///	createElementNS() or createAttributeNS(), no namespace
	///	well-formed checking is done on the target name. Applications
	///	should invoke normalizeDocument() with the parameter
	///	"namespaces" set to true in order to ensure that the target
	///	name is namespace well-formed.
	///
	/// \param data
	///	The data for the node.
	///
	/// \returns
	///	The new ProcessingInstruction object.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified target is not an XML name according to
	///	the XML version in use specified in the xmlVersion attribute.
	/// 
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if this document is an HTML document.
	///
	/// \since DOM Level 1

	virtual XProcInstrPtr createProcessingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	) = 0;

	/// \brief Creates an Attr of the given name.
	///
	/// Note that the Attr instance can then be set on an Element using the
	/// setAttributeNode method.
	///
	/// To create an attribute with a qualified name and namespace URI, use
	/// the createAttributeNS method.
	///
	/// \param name
	///	The name of the attribute.
	///
	/// \returns
	///	A new Attr object with the nodeName attribute set to name, and
	///	localName, prefix, and namespaceURI set to null. The value of
	///	the attribute is the empty string.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified name is not an XML name according to
	///	the XML version in use specified in the xmlVersion attribute.
	///
	/// \since DOM Level 1

	virtual XAttrPtr createAttribute(
		const	Buffer &	name
	) = 0;

	/// \brief Creates an EntityReference object.
	///
	/// In addition, if the referenced entity is known, the child list of
	/// the EntityReference node is made the same as that of the
	/// corresponding Entity node.
	///
	/// \note
	///	If any descendant of the Entity node has an unbound namespace
	///	prefix, the corresponding descendant of the created
	///	EntityReference node is also unbound; (its namespaceURI is
	///	null). The DOM Level 2 and 3 do not support any mechanism to
	///	resolve namespace prefixes in this case.
	///
	/// \param name
	///	The name of the entity to reference. Unlike createElementNS()
	///	or createAttributeNS(), no namespace well-formed checking is
	///	done on the entity name. Applications should invoke
	///	normalizeDocument() with the parameter "namespaces" set to true
	///	in order to ensure that the entity name is namespace
	///	well-formed.
	///
	/// \returns
	///	The new EntityReference object.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified name is not an XML name according to
	///	the XML version in use specified in the xmlVersion attribute.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if this document is an HTML document.
	///
	/// \since DOM Level 1

	virtual XEntityRefPtr createEntityReference(
		const	Buffer &	name
	) = 0;

	/// \brief Returns a NodeList of all the Elements in document order
	///	with a given tag name and are contained in the document.
	///
	/// \param tagname
	///	The name of the tag to match on. The special value "*" matches
	///	all tags. For XML, the tagname parameter is case-sensitive,
	///	otherwise it depends on the case-sensitivity of the markup
	///	language in use.
	///
	/// \returns
	///	A new NodeList object containing all the matched Elements.
	///
	/// \since DOM Level 1

	virtual XNodeListPtr getElementsByTagName(
		const	Buffer &	tagname
	) = 0;

	/// \brief Imports a node from another document to this document,
	///	without altering or removing the source node from the original
	///	document.
	///
	/// This method creates a new copy of the source node. The returned
	/// node has no parent; (parentNode is null).
	///
	/// For all nodes, importing a node creates a node object owned by the
	/// importing document, with attribute values identical to the source
	/// node's nodeName and nodeType, plus the attributes related to
	/// namespaces (prefix, localName, and namespaceURI). As in the
	/// cloneNode operation, the source node is not altered. User data
	/// associated to the imported node is not carried over. However, if
	/// any UserDataHandlers has been specified along with the associated
	/// data these handlers will be called with the appropriate parameters
	/// before this method returns.
	///
	/// Additional information is copied as appropriate to the nodeType,
	/// attempting to mirror the behavior expected if a fragment of XML or
	/// HTML source was copied from one document to another, recognizing
	/// that the two documents may have different DTDs in the XML case. The
	/// following list describes the specifics for each type of node.
	///
	/// -	ATTRIBUTE_NODE: The ownerElement attribute is set to null and
	///	the specified flag is set to true on the generated Attr. The
	///	descendants of the source Attr are recursively imported and the
	///	resulting nodes reassembled to form the corresponding subtree.
	///	Note that the deep parameter has no effect on Attr nodes; they
	///	always carry their children with them when imported.
	/// -	DOCUMENT_FRAGMENT_NODE: If the deep option was set to true, the
	///	descendants of the source DocumentFragment are recursively
	///	imported and the resulting nodes reassembled under the imported
	///	DocumentFragment to form the corresponding subtree. Otherwise,
	///	this simply generates an empty DocumentFragment.
	/// -	DOCUMENT_NODE: Document nodes cannot be imported.
	/// -	DOCUMENT_TYPE_NODE: DocumentType nodes cannot be imported.
	/// -	ELEMENT_NODE: Specified attribute nodes of the source element
	///	are imported, and the generated Attr nodes are attached to the
	///	generated Element. Default attributes are not copied, though if
	///	the document being imported into defines default attributes for
	///	this element name, those are assigned. If the importNode deep
	///	parameter was set to true, the descendants of the source
	///	element are recursively imported and the resulting nodes
	///	reassembled to form the corresponding subtree.
	/// -	ENTITY_NODE: Entity nodes can be imported, however in the
	///	current release of the DOM the DocumentType is readonly.
	///	Ability to add these imported nodes to a DocumentType will be
	///	considered for addition to a future release of the DOM. On
	///	import, the publicId, systemId, and notationName attributes are
	///	copied. If a deep import is requested, the descendants of the
	///	the source Entity are recursively imported and the resulting
	///	nodes reassembled to form the corresponding subtree.
	/// -	ENTITY_REFERENCE_NODE: Only the EntityReference itself is
	///	copied, even if a deep import is requested, since the source
	///	and destination documents might have defined the entity
	///	differently. If the document being imported into provides a
	///	definition for this entity name, its value is assigned.
	/// -	NOTATION_NODE: Notation nodes can be imported, however in the
	///	current release of the DOM the DocumentType is readonly.
	///	Ability to add these imported nodes to a DocumentType will be
	///	considered for addition to a future release of the DOM. On
	///	import, the publicId and systemId attributes are copied. Note
	///	that the deep parameter has no effect on this type of nodes
	///	since they cannot have any children.
	/// -	PROCESSING_INSTRUCTION_NODE: The imported node copies its
	///	target and data values from those of the source node.
	///	Note that the deep parameter has no effect on this type of
	///	nodes since they cannot have any children.
	/// -	TEXT_NODE, CDATA_SECTION_NODE, COMMENT_NODE: These three types
	///	of nodes inheriting from CharacterData copy their data and
	///	length attributes from those of the source node.
	///	Note that the deep parameter has no effect on these types of
	///	nodes since they cannot have any children.
	///
	/// \param importedNode
	///	The node to import.
	///
	/// \param deep
	///	If true, recursively import the subtree under the specified
	///	node; if false, import only the node itself, as explained
	///	above. This has no effect on nodes that cannot have any
	///	children, and on Attr, and EntityReference nodes.
	///
	/// \returns
	///	The imported node that belongs to this Document.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if the type of node being imported is not supported.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if one of the imported names is not an XML name
	///	according to the XML version in use specified in the xmlVersion
	///	attribute. This may happen when importing an XML 1.1 [XML 1.1]
	///	element into an XML 1.0 document, for instance.
	///
	/// \since DOM Level 2

	virtual XNodePtr importNode(
			XNodePtr	importedNode,
		const	Bool		deep
	) = 0;

	/// \brief Creates an element of the given qualified name and namespace
	///	URI.
	///
	/// Per [XML Namespaces], applications must use the value null as the
	/// namespaceURI parameter for methods if they wish to have no
	/// namespace.
	///
	/// \param namespaceURI
	///	The namespace URI of the element to create.
	///
	/// \param qualifiedName
	///	The qualified name of the element type to instantiate.
	///
	/// \returns
	///	A new Element object with the following attributes:
	///	-	Node.nodeName: qualifiedName
	///	-	Node.namespaceURI: namespaceURI
	///	-	Node.prefix: prefix, extracted from qualifiedName, or
	///		null if there is no prefix
	///	-	Node.localName: local name, extracted from
	///		qualifiedName
	///	-	Element.tagName	qualifiedName
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified qualifiedName is not an XML name
	///	according to the XML version in use specified in the xmlVersion
	///	attribute.
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the qualifiedName is a malformed qualified name, if
	///	the qualifiedName has a prefix and the namespaceURI is null, or
	///	if the qualifiedName has a prefix that is "xml" and the
	///	namespaceURI is different from
	///	"http://www.w3.org/XML/1998/namespace" [XML Namespaces], or if
	///	the qualifiedName or its prefix is "xmlns" and the namespaceURI
	///	is different from "http://www.w3.org/2000/xmlns/", or if the
	///	namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the
	///	qualifiedName nor its prefix is "xmlns".
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Always thrown if the current document does not support the
	///	"XML" feature, since namespaces were defined by XML.
	///
	/// \since DOM Level 2

	virtual XElementPtr createElementNS(
		const	Buffer &	namespaceURI,
		const	Buffer &	qualifiedName
	) = 0;

	/// \brief Creates an attribute of the given qualified name and
	///	namespace URI.
	///
	/// Per [XML Namespaces], applications must use the value null as the
	/// namespaceURI parameter for methods if they wish to have no
	/// namespace.
	///
	/// \param namespaceURI
	///	The namespace URI of the attribute to create.
	///
	/// \param qualifiedName
	///	The qualified name of the attribute to instantiate.
	///
	/// \returns
	///	A new Attr object with the following attributes:
	///	-	Attribute: Value
	///	-	Node.nodeName: qualifiedName
	///	-	Node.namespaceURI: namespaceURI
	///	-	Node.prefix: prefix, extracted from qualifiedName, or null if there is no prefix
	///	-	Node.localName: local name, extracted from qualifiedName
	///	-	Attr.name: qualifiedName
	///	-	Node.nodeValue: the empty string
	///
	/// \throws Exception( INVALID_CHARACTER_ERR)
	///	Raised if the specified qualifiedName is not an XML name
	///	according to the XML version in use specified in the xmlVersion
	///	attribute.
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the qualifiedName is a malformed qualified name, if
	///	the qualifiedName has a prefix and the namespaceURI is null, if
	///	the qualifiedName has a prefix that is "xml" and the
	///	namespaceURI is different from
	///	"http://www.w3.org/XML/1998/namespace", if the qualifiedName or
	///	its prefix is "xmlns" and the namespaceURI is different from
	///	"http://www.w3.org/2000/xmlns/", or if the namespaceURI is
	///	"http://www.w3.org/2000/xmlns/" and neither the qualifiedName
	///	nor its prefix is "xmlns".
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Always thrown if the current document does not support the
	///	"XML" feature, since namespaces were defined by XML.
	///
	/// \since DOM Level 2

	virtual XAttrPtr createAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	qualifiedName
	) = 0;

	/// \brief Returns a NodeList of all the Elements with a given local
	///	name and namespace URI in document order.
	///
	/// \param namespaceURI
	///	The namespace URI of the elements to match on. The special
	///	value "*" matches all namespaces.
	///
	/// \param localName
	///	The local name of the elements to match on. The special value
	///	"*" matches all local names.
	///
	/// \returns
	///	A new NodeList object containing all the matched Elements.
	///
	/// \since DOM Level 2

	virtual XNodeListPtr getElementsByTagNameNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

	/// \brief Returns the Element that has an ID attribute with the given
	///	value.
	///
	/// If no such element exists, this returns null. If more than one
	/// element has an ID attribute with that value, what is returned is
	/// undefined.
	///
	/// The DOM implementation is expected to use the attribute Attr.isId
	/// to determine if an attribute is of type ID.
	///
	/// Note: Attributes with the name "ID" or "id" are not of type ID
	/// unless so defined.
	///
	/// \param elementId
	///	The unique id value for an element.
	///
	/// \returns
	///	The matching element or null if there is none.
	///
	/// \since DOM Level 2

	virtual XElementPtr getElementById(
		const	Buffer &	elementId
	) = 0;

	/// \brief Returns the InputEncoding, which specifies the encoding used
	///	for this document at the time of the parsing.
	///
	/// This is null when it is not known, such as when the Document was
	/// created in memory.
	///
	/// \since DOM Level 3

	virtual const Buffer & getInputEncoding(
	) const = 0;

	/// \brief Returns the XML Encoding, which specifies, as part of the
	///	XML declaration, the encoding of this document.
	///
	/// This is null when unspecified or when it is not known, such as when
	/// the Document was created in memory.
	///
	/// \since DOM Level 3

	virtual const Buffer & getXMLEncoding(
	) const = 0;

	/// \brief Returns the XML Standalone, which specifies, as part of the
	///	XML declaration, whether this document is standalone.
	///
	/// This is false when unspecified.
	///
	/// \since DOM Level 3

	virtual Bool getXMLStandalone(
	) const = 0;

	/// \brief Sets the XML Standalone.
	///
	/// No verification is done on the value when setting this attribute.
	/// Applications should use Document.normalizeDocument() with the
	/// "validate" parameter to verify if the value matches the validity
	/// constraint for standalone document declaration as defined in [XML
	/// 1.0].
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if this document does not support the "XML" feature.
	///
	/// \since DOM Level 3

	virtual void setXMLStandalone(
		const	Bool		pXMLStandalone
	) = 0;

	/// \brief Returns the XML Version, which specifies, as part of the XML
	///	declaration, the version number of this document.
	///
	/// If there is no declaration and if this document supports the "XML"
	/// feature, the value is "1.0". If this document does not support the
	/// "XML" feature, the value is always null.
	///
	/// \since DOM Level 3

	virtual const Buffer & getXMLVersion(
	) const = 0;

	/// \brief Sets the XML Version, which affects methods that check for
	///	invalid characters in XML names.
	///
	/// Application should invoke Document.normalizeDocument() in order to
	/// check for invalid characters in the Nodes that are already part of
	/// this Document.
	///
	/// DOM applications may use the DOMImplementation.hasFeature(feature,
	/// version) method with parameter values "XMLVersion" and "1.0"
	/// (respectively) to determine if an implementation supports [XML 1.0].
	/// DOM applications may use the same method with parameter values
	/// "XMLVersion" and "1.1" (respectively) to determine if an
	/// implementation supports [XML 1.1]. In both cases, in order to
	/// support XML, an implementation must also support the "XML" feature
	/// defined in this specification. Document objects supporting a
	/// version of the "XMLVersion" feature must not raise a
	/// NOT_SUPPORTED_ERR exception for the same version number when using
	/// Document.xmlVersion.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if the version is set to a value that is not supported
	///	by this Document or if this document does not support the "XML"
	///	feature.
	///
	/// \since DOM Level 3

	virtual void setXMLVersion(
		const	Buffer &	pVersion
	) = 0;

	/// \brief Returns the StrictErrorChecking, which specifies whether
	///	error checking is enforced or not.
	///
	/// This attribute is true by default.
	///
	/// \since DOM Level 3

	virtual Bool getStrictErrorChecking(
	) const = 0;

	/// \brief Sets the StrictErrorChecking.
	///
	/// When set to false, the implementation is free to not test every
	/// possible error case normally defined on DOM operations, and not
	/// raise any DOMException on DOM operations or report errors while
	/// using normalizeDocument(). In case of error, the behavior is
	/// undefined.
	///
	/// \since DOM Level 3

	virtual void setStrictErrorChecking(
		const	Bool		pStrictErrorChecking
	) = 0;

	/// \brief Returns the DocumentURI, which is the location of the
	///	document or null if undefined or if the Document was created
	///	using DOMImplementation.createDocument.
	///
	/// \since DOM Level 3

	virtual const Buffer & getDocumentURI(
	) const = 0;

	/// \brief Sets the DocumentURI.
	///
	/// No lexical checking is performed when setting this attribute; this
	/// could result in a null value returned when using Node.baseURI.
	///
	/// Beware that when the Document supports the feature "HTML" [DOM
	/// Level 2 HTML], the href attribute of the HTML BASE element takes
	/// precedence over this attribute when computing Node.baseURI.
	///
	/// \since DOM Level 3

	virtual void setDocumentURI(
		const	Buffer &	pDocumentURI
	) = 0;

	/// \brief Attempts to adopt a node from another document to this
	///	document.
	///
	/// If supported, it changes the ownerDocument of the source node, its
	/// children, as well as the attached attribute nodes if there are any.
	/// If the source node has a parent it is first removed from the child
	/// list of its parent. This effectively allows moving a subtree from
	/// one document to another (unlike importNode() which create a copy of
	/// the source node instead of moving it). When it fails, applications
	/// should use Document.importNode()  instead. Note that if the adopted
	/// node is already part of this document (i.e. the source and target
	/// document are the same), this method still has the effect of
	/// removing the source node from the child list of its parent, if any.
	///
	/// The following list describes the specifics for each type of node.
	/// -	ATTRIBUTE_NODE: The ownerElement attribute is set to null and the specified flag is set to true on the adopted Attr. The descendants of the source Attr are recursively adopted.
	/// -	DOCUMENT_FRAGMENT_NODE: The descendants of the source node are recursively adopted.
	/// -	DOCUMENT_NODE: Document nodes cannot be adopted.
	/// -	DOCUMENT_TYPE_NODE: DocumentType nodes cannot be adopted.
	/// -	ELEMENT_NODE: Specified attribute nodes of the source element are adopted. Default attributes are discarded, though if the document being adopted into defines default attributes for this element name, those are assigned. The descendants of the source element are recursively adopted.
	/// -	ENTITY_NODE: Entity nodes cannot be adopted.
	/// -	ENTITY_REFERENCE_NODE: Only the EntityReference node itself is adopted, the descendants are discarded, since the source and destination documents might have defined the entity differently. If the document being imported into provides a definition for this entity name, its value is assigned.
	/// -	NOTATION_NODE: Notation nodes cannot be adopted.
	/// -	PROCESSING_INSTRUCTION_NODE, TEXT_NODE, CDATA_SECTION_NODE, COMMENT_NODE: These nodes can all be adopted. No specifics.
	///
	/// \note
	///	Since it does not create new nodes unlike the importNode()
	///	method, this method does not raise an INVALID_CHARACTER_ERR
	///	exception, and applications should use the normalizeDocument()
	///	method to check if an imported name is not an XML name
	///	according to the XML version in use.
	///
	/// \param source
	///	The node to move into this document.
	///
	/// \returns
	///	The adopted node, or null if this operation fails, such as when
	///	the source node comes from a different implementation.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised if the source node is of type DOCUMENT, DOCUMENT_TYPE.
	///
	/// \throws Exception( NO_MODIFICATION_ALLOWED_ERR )
	///	Raised when the source node is readonly.
	///
	/// \since DOM Level 3

	virtual XNodePtr adoptNode(
			XNodePtr	source
	) = 0;

	/// \brief Returns the DOM Configuration, which is used when
	///	normalizeDocument() is invoked.
	///
	/// \since DOM Level 3

	virtual ConfigCPtr getDOMConfig(
	) const = 0;

	/// \brief Acts as if the document was going through a save and load
	///	cycle, putting the document in a "normal" form.
	///
	/// As a consequence, this method updates the replacement tree of
	/// EntityReference nodes and normalizes Text nodes, as defined in the
	/// method Node.normalize().
	///
	/// Otherwise, the actual result depends on the features being set on
	/// the Document.domConfig object and governing what operations
	/// actually take place. Noticeably this method could also make the
	/// document namespace well-formed according to the algorithm described
	/// in Namespace Normalization, check the character normalization,
	/// remove the CDATASection nodes, etc. See DOMConfiguration for
	/// details.
	///
	/// \code
	///	// Keep in the document the information defined
	///	// in the XML Information Set (Java example)
	///	DOMConfiguration docConfig = myDocument.getDomConfig();
	///	docConfig.setParameter("infoset", Boolean.TRUE);
	///	myDocument.normalizeDocument();
	/// \endcode
	///
	/// Mutation events, when supported, are generated to reflect the
	/// changes occurring on the document.
	///
	/// If errors occur during the invocation of this method, such as an
	/// attempt to update a read-only node or a Node.nodeName contains an
	/// invalid character according to the XML version in use, errors or
	/// warnings (DOMError.SEVERITY_ERROR or DOMError.SEVERITY_WARNING)
	/// will be reported using the DOMErrorHandler object associated with
	/// the "error-handler" parameter. Note this method might also report
	/// fatal errors (DOMError.SEVERITY_FATAL_ERROR) if an implementation
	/// cannot recover from an error.
	///
	/// \since DOM Level 3

	virtual void normalizeDocument(
	) = 0;

	/// \brief Rename an existing node of type ELEMENT_NODE or
	///	ATTRIBUTE_NODE.
	///
	/// When possible this simply changes the name of the given node,
	/// otherwise this creates a new node with the specified name and
	/// replaces the existing node with the new node as described below.
	///
	/// If simply changing the name of the given node is not possible, the
	/// following operations are performed: a new node is created, any
	/// registered event listener is registered on the new node, any user
	/// data attached to the old node is removed from that node, the old
	/// node is removed from its parent if it has one, the children are
	/// moved to the new node, if the renamed node is an Element its
	/// attributes are moved to the new node, the new node is inserted at
	/// the position the old node used to have in its parent's child nodes
	/// list if it has one, the user data that was attached to the old node
	/// is attached to the new node.
	///
	/// When the node being renamed is an Element only the specified
	/// attributes are moved, default attributes originated from the DTD
	/// are updated according to the new element name. In addition, the
	/// implementation may update default attributes from other schemas.
	/// Applications should use Document.normalizeDocument() to guarantee
	/// these attributes are up-to-date.
	///
	/// When the node being renamed is an Attr that is attached to an
	/// Element, the node is first removed from the Element attributes map.
	/// Then, once renamed, either by modifying the existing node or
	/// creating a new one as described above, it is put back.
	///
	/// In addition:
	/// -	a user data event NODE_RENAMED is fired,
	/// -	when the implementation supports the feature
	///	"MutationNameEvents", each mutation operation involved in this
	///	method fires the appropriate event, and in the end the event
	///	{http://www.w3.org/2001/xml-events, DOMElementNameChanged} or
	///	{http://www.w3.org/2001/xml-events, DOMAttributeNameChanged} is
	///	fired.
	///
	/// \param n
	///	The node to rename.
	///
	/// \param namespaceURI
	///	The new namespace URI.
	///
	/// \param qualifiedName
	///	The new qualified name.
	///
	/// \returns
	///	The renamed node. This is either the specified node or the new
	///	node that was created to replace the specified node.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	Raised when the type of the specified node is neither
	///	ELEMENT_NODE nor ATTRIBUTE_NODE, or if the implementation does
	///	not support the renaming of the document element.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the new qualified name is not an XML name according
	///	to the XML version in use specified in the Document.xmlVersion
	///	attribute.
	///
	/// \throws Exception( WRONG_DOCUMENT_ERR )
	///	Raised when the specified node was created from a different
	///	document than this document.
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the qualifiedName is a malformed qualified name, if
	///	the qualifiedName has a prefix and the namespaceURI is null, or
	///	if the qualifiedName has a prefix that is "xml" and the
	///	namespaceURI is different from
	///	"http://www.w3.org/XML/1998/namespace" [XML Namespaces]. Also
	///	raised, when the node being renamed is an attribute, if the
	///	qualifiedName, or its prefix, is "xmlns" and the namespaceURI
	///	is different from "http://www.w3.org/2000/xmlns/".
	///
	/// \since DOM Level 3

	virtual XNodePtr renameNode(
			XNodePtr	n,
		const	Buffer &	namespaceURI,
		const	Buffer &	qualifiedName
	) = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XDocument_H_

// ============================================================================

