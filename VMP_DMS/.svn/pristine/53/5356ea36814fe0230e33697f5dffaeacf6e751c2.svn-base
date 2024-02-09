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
//	BFC.DOM.Implementation.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Implementation_H_
#define _BFC_DOM_Implementation_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.ImplementationPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DOM.Document.h"
#include "BFC.DOM.DocumentType.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Provides information about the features of the DOM Implementation.
///
/// The Implementation interface provides a number of methods for performing
/// operations that are independent of any particular instance of the document
/// object model.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL Implementation : virtual public SObject {

public:

	/// \brief Creates a clone (deep copy) of this Implementation.

	virtual ImplementationPtr clone(
	) const = 0;

	/// \brief Test if the DOM implementation implements a specific feature
	///	and version, as specified in DOM Features.
	///
	/// \param feature
	///	The name of the feature to test.
	///
	/// \param version
	///	This is the version number of the feature to test.
	///
	/// \returns
	///	true if the feature is implemented in the specified version,
	///	false otherwise.
	///
	/// \since DOM Level 1

	virtual Bool hasFeature(
		const	Buffer &	pFeature,
		const	Buffer &	pVersion
	) const = 0;

	/// \brief Creates an empty DocumentType node.
	///
	/// Entity declarations and notations are not made available. Entity
	/// reference expansions and default attribute additions do not occur.
	///
	/// \param qualifiedName
	///	The qualified name of the document type to be created.
	///
	/// \param publicId
	///	The external subset public identifier.
	///
	/// \param systemId
	///	The external subset system identifier.
	///
	/// \returns
	///	A new DocumentType node with Node::ownerDocument set to null.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified qualified name is not an XML name
	///	according to [XML 1.0].
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the qualifiedName is malformed.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	May be raised if the implementation does not support the
	///	feature "XML" and the language exposed through the Document
	///	does not support XML Namespaces (such as [HTML 4.01]).
	///
	/// \since DOM Level 2

	virtual DocumentType createDocumentType(
		const	Buffer &	qName,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	) const = 0;

	/// \brief Creates a DOM Document object of the specified type with its
	///	document element.
	///
	/// Note that based on the DocumentType given to create the document,
	/// the implementation may instantiate specialized Document objects
	/// that support additional features than the "Core", such as "HTML"
	/// [DOM Level 2 HTML]. On the other hand, setting the DocumentType
	/// after the document was created makes this very unlikely to happen.
	/// Alternatively, specialized Document creation methods, such as
	/// createHTMLDocument [DOM Level 2 HTML], can be used to obtain
	/// specific types of Document objects.
	///
	/// \param namespaceURI
	///	The namespace URI of the document element to create or null.
	///
	/// \param qualifiedName
	///	The qualified name of the document element to be created or
	///	null.
	///
	/// \param doctype
	///	The type of document to be created or null. When doctype is not
	///	null, its Node::ownerDocument attribute is set to the document
	///	being created.
	///
	/// \returns
	///	A new Document object with its document element. If the
	///	NamespaceURI, qualifiedName, and doctype are null, the returned
	///	Document is empty with no document element.
	///
	/// \throws Exception( INVALID_CHARACTER_ERR )
	///	Raised if the specified qualified name is not an XML name
	///	according to [XML 1.0].
	///
	/// \throws Exception( NAMESPACE_ERR )
	///	Raised if the qualifiedName is malformed, if the qualifiedName
	///	has a prefix and the namespaceURI is null, or if the
	///	qualifiedName is null and the namespaceURI is different from
	///	null, or if the qualifiedName has a prefix that is "xml" and
	///	the namespaceURI is different from
	///	"http://www.w3.org/XML/1998/namespace" [XML Namespaces], or if
	///	the DOM implementation does not support the "XML" feature but a
	///	non-null namespace URI was provided, since namespaces were
	///	defined by XML.
	///
	/// \throws Exception( WRONG_DOCUMENT_ERR )
	///	Raised if doctype has already been used with a different
	///	document or was created from a different implementation.
	///
	/// \throws Exception( NOT_SUPPORTED_ERR )
	///	May be raised if the implementation does not support the
	///	feature "XML" and the language exposed through the Document
	///	does not support XML Namespaces (such as [HTML 4.01]).
	///
	/// \since DOM Level 2

	virtual Document createDocument(
		const	Buffer &	nsURI,
		const	Buffer &	qName,
		const	DocumentType &	doctype
	) const = 0;

	/// \brief Returns a specialized object which implements the
	///	specialized APIs of the specified feature and version, as
	///	specified in DOM Features.
	///
	/// The specialized object may also be obtained by using
	/// binding-specific casting methods but is not necessarily expected
	/// to, as discussed in Mixed DOM Implementations. This method also
	/// allow the implementation to provide specialized objects which do
	/// not support the DOMImplementation interface.
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
	///	An SObject which implements the specialized APIs of the
	///	specified feature and version, if any, or null if there is no
	///	object which implements interfaces associated with that
	///	feature. If the SObject returned by this method implements the
	///	Implementation interface, it must delegate to the primary core
	///	Implementation and not return results inconsistent with the
	///	primary core Implementation such as hasFeature, getFeature,
	///	etc.
	///
	/// \since DOM Level 3

	virtual SCPtr getFeature(
		const	Buffer &	pFeature,
		const	Buffer &	pVersion
	) const = 0;

//	/// \brief Returns false iff the object was created by
//	///	DOM::Document::getImplementation().
//
//	Bool isEmpty(
//	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Implementation_H_

// ============================================================================

