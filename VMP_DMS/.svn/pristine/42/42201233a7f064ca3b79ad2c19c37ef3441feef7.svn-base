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
//	BFC.DOM.XElement.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XElement_H_
#define _BFC_DOM_XElement_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XElementPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.XAttrPtr.h"
#include "BFC.DOM.XNode.h"
#include "BFC.DOM.XNodeListPtr.h"
#include "BFC.DOM.TypeInfoPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Element.
///
/// The Element interface represents an element in an HTML or XML document.
/// Elements may have attributes associated with them; since the Element
/// interface inherits from Node, the generic Node interface attribute
/// attributes may be used to retrieve the set of all attributes for an
/// element. There are methods on the Element interface to retrieve either an
/// XAttrPtr object by name or an attribute value by name. In XML, where an
/// attribute value may contain entity references, an XAttrPtr object should be
/// retrieved to examine the possibly fairly complex sub-tree representing the
/// attribute value. On the other hand, in HTML, where all attributes have
/// simple string values, methods to directly access an attribute value can
/// safely be used as a convenience.
///
/// \note
///	In DOM Level 2, the method normalize is inherited from the Node
///	interface where it was moved.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XElement : virtual public XNode {

public :

#if 0

///The name of the element. If Node.localName  is different from null, this attribute is a qualified name. For example, in:
///
///          <elementExample id="demo"> 
///          ... 
///          </elementExample> ,
///        
///
///tagName has the value "elementExample". Note that this is case-preserving in XML, as are all of the operations of the DOM. The HTML DOM returns the tagName of an HTML element in the canonical uppercase form, regardless of the case in the source HTML document.
	/// \since DOM Level 1

	virtual const Buffer & getTagName(
	) const = 0;

///Retrieves an attribute value by name.
///Parameters
///
///name of type DOMString
///    The name of the attribute to retrieve.
///
///Return Value
///
///DOMString
///	
///
///The Attr value as a string, or the empty string if that attribute does not have a specified or default value.
	/// \since DOM Level 1

	virtual const Buffer & getAttribute(
		const	Buffer &	name
	) const = 0;

///Adds a new attribute. If an attribute with that name is already present in the element, its value is changed to be that of the value parameter. This value is a simple string; it is not parsed as it is being set. So any markup (such as syntax to be recognized as an entity reference) is treated as literal text, and needs to be appropriately escaped by the implementation when it is written out. In order to assign an attribute value that contains entity references, the user must create an Attr  node plus any Text  and EntityReference  nodes, build the appropriate subtree, and use setAttributeNode to assign it as the value of an attribute.
///To set an attribute with a qualified name and namespace URI, use the setAttributeNS method.
///Parameters
///
///name of type DOMString
///    The name of the attribute to create or alter.
///value of type DOMString
///    Value to set in string form.
///
///Exceptions
///
///DOMException
///	
///
///INVALID_CHARACTER_ERR: Raised if the specified name is not an XML name according to the XML version in use specified in the Document.xmlVersion attribute.
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
	/// \since DOM Level 1

	virtual void setAttribute(
		const	Buffer &	name, 
		const	Buffer &	value
	) = 0;

///Removes an attribute by name. If a default value for the removed attribute is defined in the DTD, a new attribute immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable. The implementation may handle default values from other schemas similarly but applications should use Document.normalizeDocument()  to guarantee this information is up-to-date.
///If no attribute with this name is found, this method has no effect.
///To remove an attribute by local name and namespace URI, use the removeAttributeNS method.
///Parameters
///
///name of type DOMString
///    The name of the attribute to remove.
///
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
	/// \since DOM Level 1

	virtual void removeAttribute(
		const	Buffer &	name
	) = 0;

///Retrieves an attribute node by name.
///To retrieve an attribute node by qualified name and namespace URI, use the getAttributeNodeNS method.
///Parameters
///
///name of type DOMString
///    The name (nodeName) of the attribute to retrieve.
///
///Return Value
///
///Attr
///	
///
///The Attr node with the specified name (nodeName) or null if there is no such attribute.
	/// \since DOM Level 1

	virtual XAttrPtr getAttributeNode(
		const	Buffer &	name
	) const = 0;

///Adds a new attribute node. If an attribute with that name (nodeName) is already present in the element, it is replaced by the new one. Replacing an attribute node by itself has no effect.
///To add a new attribute node with a qualified name and namespace URI, use the setAttributeNodeNS method.
///Parameters
///
///newAttr of type Attr
///    The Attr node to add to the attribute list.
///
///Return Value
///
///Attr
///	
///
///If the newAttr attribute replaces an existing attribute, the replaced Attr node is returned, otherwise null is returned.
///Exceptions
///
///DOMException
///	
///
///WRONG_DOCUMENT_ERR: Raised if newAttr was created from a different document than the one that created the element.
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///INUSE_ATTRIBUTE_ERR: Raised if newAttr is already an attribute of another Element object. The DOM user must explicitly clone Attr nodes to re-use them in other elements.
	/// \since DOM Level 1

	virtual XAttrPtr setAttributeNode(
			XAttrPtr	newAttr
	) = 0;

///Removes the specified attribute node. If a default value for the removed Attr  node is defined in the DTD, a new node immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable. The implementation may handle default values from other schemas similarly but applications should use Document.normalizeDocument()  to guarantee this information is up-to-date.
///Parameters
///
///oldAttr of type Attr
///    The Attr node to remove from the attribute list.
///
///Return Value
///
///Attr
///	
///
///The Attr node that was removed.
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NOT_FOUND_ERR: Raised if oldAttr is not an attribute of the element.
	/// \since DOM Level 1

	virtual XAttrPtr removeAttributeNode(
			XAttrPtr	oldAttr
	) = 0;

///Returns a NodeList  of all descendant  Elements  with a given tag name, in document order.
///Parameters
///
///name of type DOMString
///    The name of the tag to match on. The special value "*" matches all tags.
///
///Return Value
///
///NodeList
///	
///
///A list of matching Element nodes.
	/// \since DOM Level 1

	virtual XNodeListPtr getElementsByTagName(
		const	Buffer &	name
	) = 0;

///Retrieves an attribute value by local name and namespace URI.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute to retrieve.
///localName of type DOMString
///    The local name of the attribute to retrieve.
///
///Return Value
///
///DOMString
///	
///
///The Attr value as a string, or the empty string if that attribute does not have a specified or default value.
///Exceptions
///
///DOMException
///	
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual const Buffer & getAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

///Adds a new attribute. If an attribute with the same local name and namespace URI is already present on the element, its prefix is changed to be the prefix part of the qualifiedName, and its value is changed to be the value parameter. This value is a simple string; it is not parsed as it is being set. So any markup (such as syntax to be recognized as an entity reference) is treated as literal text, and needs to be appropriately escaped by the implementation when it is written out. In order to assign an attribute value that contains entity references, the user must create an Attr  node plus any Text  and EntityReference  nodes, build the appropriate subtree, and use setAttributeNodeNS or setAttributeNode to assign it as the value of an attribute.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute to create or alter.
///qualifiedName of type DOMString
///    The qualified name of the attribute to create or alter.
///value of type DOMString
///    The value to set in string form.
///
///Exceptions
///
///DOMException
///	
///
///INVALID_CHARACTER_ERR: Raised if the specified qualified name is not an XML name according to the XML version in use specified in the Document.xmlVersion attribute.
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NAMESPACE_ERR: Raised if the qualifiedName is malformed per the Namespaces in XML specification, if the qualifiedName has a prefix and the namespaceURI is null, if the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace", if the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/", or if the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the qualifiedName nor its prefix is "xmlns".
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]).
	/// \since DOM Level 2

	virtual void setAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	qualifiedName, 
		const	Buffer &	value
	) = 0;

///Removes an attribute by local name and namespace URI. If a default value for the removed attribute is defined in the DTD, a new attribute immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable. The implementation may handle default values from other schemas similarly but applications should use Document.normalizeDocument()  to guarantee this information is up-to-date.
///If no attribute with this local name and namespace URI is found, this method has no effect.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute to remove.
///localName of type DOMString
///    The local name of the attribute to remove.
///
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual void removeAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

///Retrieves an Attr  node by local name and namespace URI.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute to retrieve.
///localName of type DOMString
///    The local name of the attribute to retrieve.
///
///Return Value
///
///Attr
///	
///
///The Attr node with the specified attribute local name and namespace URI or null if there is no such attribute.
///Exceptions
///
///DOMException
///	
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual XAttrPtr getAttributeNodeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

///Adds a new attribute. If an attribute with that local name and that namespace URI is already present in the element, it is replaced by the new one. Replacing an attribute node by itself has no effect.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///newAttr of type Attr
///    The Attr node to add to the attribute list.
///
///Return Value
///
///Attr
///	
///
///If the newAttr attribute replaces an existing attribute with the same local name and namespace URI, the replaced Attr node is returned, otherwise null is returned.
///Exceptions
///
///DOMException
///	
///
///WRONG_DOCUMENT_ERR: Raised if newAttr was created from a different document than the one that created the element.
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///INUSE_ATTRIBUTE_ERR: Raised if newAttr is already an attribute of another Element object. The DOM user must explicitly clone Attr nodes to re-use them in other elements.
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual XAttrPtr setAttributeNodeNS(
			XAttrPtr	newAttr
	) = 0;

///Returns a NodeList  of all the descendant  Elements  with a given local name and namespace URI in document order.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the elements to match on. The special value "*" matches all namespaces.
///localName of type DOMString
///    The local name of the elements to match on. The special value "*" matches all local names.
///
///Return Value
///
///NodeList
///	
///
///A new NodeList object containing all the matched Elements.
///Exceptions
///
///DOMException
///	
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual XNodeListPtr getElementsByTagNameNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

///Returns true when an attribute with a given name is specified on this element or has a default value, false  otherwise.
///Parameters
///
///name of type DOMString
///    The name of the attribute to look for.
///
///Return Value
///
///boolean
///	
///
///true if an attribute with the given name is specified on this element or has a default value, false otherwise.
	/// \since DOM Level 2

	virtual Bool hasAttribute(
		const	Buffer &	name
	) = 0;

///Returns true when an attribute with a given local name and namespace URI is specified on this element or has a default value, false otherwise.
///Per [XML Namespaces], applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute to look for.
///localName of type DOMString
///    The local name of the attribute to look for.
///
///Return Value
///
///boolean
///	
///
///true if an attribute with the given local name and namespace URI is specified or has a default value on this element, false otherwise.
///Exceptions
///
///DOMException
///	
///
///NOT_SUPPORTED_ERR: May be raised if the implementation does not support the feature "XML" and the language exposed through the Document does not support XML Namespaces (such as [HTML 4.01]). 
	/// \since DOM Level 2

	virtual Bool hasAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName
	) = 0;

///The type information associated with this element. 
	/// \since DOM Level 3

	virtual TypeInfoCPtr getSchemaTypeInfo(
	) const = 0;

///If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute. This affects the value of Attr.isId  and the behavior of Document.getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr.schemaTypeInfo  of the specified Attr  node. Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.
///To specify an attribute by local name and namespace URI, use the setIdAttributeNS method.
///Parameters
///
///name of type DOMString
///    The name of the attribute.
///isId of type boolean
///    Whether the attribute is a of type ID.
///
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NOT_FOUND_ERR: Raised if the specified node is not an attribute of this element.
	/// \since DOM Level 3

	virtual void setIdAttribute(
		const	Buffer &	name, 
		const	Bool		isId
	) = 0;

///If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute. This affects the value of Attr.isId  and the behavior of Document.getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr.schemaTypeInfo  of the specified Attr  node. Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.
///Parameters
///
///namespaceURI of type DOMString
///    The namespace URI of the attribute.
///localName of type DOMString
///    The local name of the attribute.
///isId of type boolean
///    Whether the attribute is a of type ID.
///
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NOT_FOUND_ERR: Raised if the specified node is not an attribute of this element.
	/// \since DOM Level 3

	virtual void setIdAttributeNS(
		const	Buffer &	namespaceURI, 
		const	Buffer &	localName, 
		const	Bool		isId
	) = 0;

///If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute. This affects the value of Attr.isId  and the behavior of Document.getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr.schemaTypeInfo  of the specified Attr  node. Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.
///Parameters
///
///idAttr of type Attr
///    The attribute node.
///isId of type boolean
///    Whether the attribute is a of type ID.
///
///Exceptions
///
///DOMException
///	
///
///NO_MODIFICATION_ALLOWED_ERR: Raised if this node is readonly.
///
///NOT_FOUND_ERR: Raised if the specified node is not an attribute of this element.
	/// \since DOM Level 3

	virtual void setIdAttributeNode(
			XAttrPtr	idAttr,
		const	Bool		isId
	) = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XElement_H_

// ============================================================================

