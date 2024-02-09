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
//	BFC.DOM.DocumentImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_DocumentImpl_H_
#define _BFC_DOM_DocumentImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.DocumentImplPtr.h"

// ============================================================================

#include "BFC.XML.InputSource.h"
#include "BFC.XML.Reader.h"

#include "BFC.DOM.Attr.h"
#include "BFC.DOM.CDATASection.h"
#include "BFC.DOM.Comment.h"
#include "BFC.DOM.XDocument.h"
#include "BFC.DOM.DocumentFragment.h"
#include "BFC.DOM.DocumentTypeImplPtr.h"
#include "BFC.DOM.Element.h"
#include "BFC.DOM.EntityImplPtr.h"
#include "BFC.DOM.EntityReference.h"
#include "BFC.DOM.ImplementationPtr.h"
#include "BFC.DOM.NodeImpl.h"
#include "BFC.DOM.NotationImplPtr.h"
#include "BFC.DOM.ProcessingInstruction.h"
#include "BFC.DOM.Text.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class DocumentImpl :

	virtual public XDocument,
	public NodeImpl {

public :

	DocumentImpl(
	);

	DocumentImpl(
		const	Buffer &	name
	);

	DocumentImpl(
			DocumentTypeImplPtr	dt
	);

	DocumentImpl(
			DocumentImplPtr	n,
			Bool		deep
	);

	void setContent(
			XML::InputSourcePtr	source,
			Bool		namespaceProcessing
	);

	void setContent(
			XML::InputSourcePtr	source,
			XML::Reader *	reader
	);

	DocumentTypeImplPtr getDocType(
	) const {
		return type;
	}

	ImplementationPtr getImplementation(
	) const {
		return impl;
	}

	ElementImplPtr getDocumentElement(
	) const;

	ElementImplPtr createElement(
		const	Buffer &	tagName
	);

	ElementImplPtr createElementNS(
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	DocumentFragmentImplPtr createDocumentFragment(
	);

	TextImplPtr createTextNode(
		const	Buffer &	data
	);

	CommentImplPtr createComment(
		const	Buffer &	data
	);

	CDATASectionImplPtr createCDATASection(
		const	Buffer &	data
	);

	ProcessingInstructionImplPtr createProcessingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	);

	AttrImplPtr createAttribute(
		const	Buffer &	name
	);

	AttrImplPtr createAttributeNS(
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	EntityReferenceImplPtr createEntityReference(
		const	Buffer &	name
	);

	EntityImplPtr createEntity(
		const	Buffer &	pEntityName,
		const	Buffer &	pPublicId,
		const	Buffer &	pSystemId,
		const	Buffer &	pNotationName,
		const	Buffer &	pValue
	);

	NotationImplPtr createNotation(
		const	Buffer &	pNotationName,
		const	Buffer &	pPublicId,
		const	Buffer &	pSystemId
	);

	NodeImplPtr importNode(
		const	NodeImplPtr	importedNode,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	virtual Bool isDocument(
	) const {
		return true;
	}

	virtual void clear(
	);

	virtual void save(
			IO::DevicePtr,
			int,
			int
	) const;

	ImplementationPtr	impl;
	DocumentTypeImplPtr	type;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_DocumentImpl_H_

// ============================================================================

