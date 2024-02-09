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
//	BFC.DOM.DocumentType.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_DocumentType_H_
#define _BFC_DOM_DocumentType_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.DocumentTypeImplPtr.h"
#include "BFC.DOM.NamedNodeMap.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/*!
	\brief The DOM::DocumentType class is the representation of the DTD
	in the document tree.

	\ingroup BFC_DOM

	The DOM::DocumentType class allows read-only access to some of the
	data structures in the DTD: it can return a map of all entities()
	and notations(). In addition the function name() returns the name
	of the document type as specified in the &lt;!DOCTYPE name&gt;
	tag. This class also provides the publicId(), systemId() and
	internalSubset() functions.

	\sa DOM::Document
*/

class BFC_DOM_DLL DocumentType : public Node {

public:

/*!
	Creates an empty DOM::DocumentType object.
*/

	DocumentType();

/*!
	Constructs a copy of \a n.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/

	DocumentType(const DocumentType& x);

/*!
	Assigns \a n to this document type.

	The data of the copy is shared (shallow copy): modifying one node
	will also change the other. If you want to make a deep copy, use
	cloneNode().
*/

	DocumentType& operator= (const DocumentType&);

	// DOM read only attributes

/*!
	Returns the name of the document type as specified in the
	&lt;!DOCTYPE name&gt; tag.

	\sa getNodeName()
*/

	Buffer name() const;

	void setName(
		const	Buffer &	pName
	);

/*!
	Returns a map of all entities described in the DTD.
*/

	NamedNodeMap entities() const;

/*!
	Returns a map of all notations described in the DTD.
*/

	NamedNodeMap notations() const;

/*!
	Returns the public identifier of the external DTD subset or
	an empty string if there is no public identifier.

	\sa systemId() internalSubset() DOM::Implementation::createDocumentType()
*/

	Buffer publicId() const;

	void setPublicId(
		const	Buffer &	pPublicId
	);

/*!
	Returns the system identifier of the external DTD subset or
	an empty string if there is no system identifier.

	\sa publicId() internalSubset() DOM::Implementation::createDocumentType()
*/

	Buffer systemId() const;

	void setSystemId(
		const	Buffer &	pSystemId
	);

/*!
	Returns the internal subset of the document type or an empty
	string if there is no internal subset.

	\sa publicId() systemId()
*/

	Buffer internalSubset() const;

	virtual NodeType getNodeType() const { return DocumentTypeNode; }

private :

	DocumentType(DocumentTypeImplPtr);

	friend class ImplementationImpl;
	friend class Document;
	friend class Node;
};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_DocumentType_H_

// ============================================================================

