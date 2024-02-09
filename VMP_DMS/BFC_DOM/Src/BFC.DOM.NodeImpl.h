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
//	BFC.DOM.NodeImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NodeImpl_H_
#define _BFC_DOM_NodeImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.NodeImplPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.IO.Device.h"

#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.XNode.h"
#include "BFC.DOM.NodeType.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class BFC_DOM_DLL NodeImpl : virtual public XNode {

protected :

	NodeImpl(
		const	NodeType	nodeType,
		const	DocumentImplPtr	&
					ownerDoc,
		const	NodeImplPtr &	parent = NodeImplPtr()
	);

	NodeImpl(
		const	NodeType	nodeType,
		const	NodeImplPtr &	n,
		const	Bool		deep
	);

public:

	virtual ~NodeImpl(
	);

	const Buffer & getNodeName(
	) const {
		return name;
	}

	void setNodeName(
		const	Buffer &	pNodeName
	) {
		name = pNodeName;
	}

	const Buffer & getNodeValue(
	) const {
		return value;
	}

	virtual void setNodeValue(
		const	Buffer &	v
	) {
		value = v;
	}

	DocumentImplPtr getOwnerDocument(
	);

	void setOwnerDocument(
		const	DocumentImplPtr	&
					doc
	) {
		ownr = doc;
		hasParent = false;
	}

	virtual NodeImplPtr insertBefore(
		const	NodeImplPtr &	newChild,
		const	NodeImplPtr &	refChild
	);

	virtual NodeImplPtr insertAfter(
		const	NodeImplPtr &	newChild,
		const	NodeImplPtr &	refChild
	);

	virtual NodeImplPtr replaceChild(
		const	NodeImplPtr &	newChild,
		const	NodeImplPtr &	oldChild
	);

	virtual NodeImplPtr removeChild(
		const	NodeImplPtr &	oldChild
	);

	virtual NodeImplPtr appendChild(
		const	NodeImplPtr &	newChild
	);

	NodeImplPtr getNamedItem(
		const	Buffer &	name
	) const;

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	virtual void normalize(
	);

	virtual void clear(
	);

	NodeImplCPtr getParent(
	) const {
		return ( hasParent ? NodeImplCPtr( ownr ) : NodeImplCPtr() );
	}

	NodeImplPtr getParent(
	) {
		return ( hasParent ? ownr : NodeImplPtr() );
	}

	void setParent(
		const	NodeImplPtr &	p
	) {
		ownr = NodeImplPtr( p, false );
		hasParent = true;
	}

	void setNoParent(
	);

	// Dynamic cast

	Bool isAttr(
	) const;

	Bool isCDATASection(
	) const;

	Bool isDocumentFragment(
	) const;

	virtual Bool isDocument(
	) const {
		return false;
	}

	virtual Bool isDocumentType(
	) const {
		return false;
	}

	virtual Bool isElement(
	) const {
		return false;
	}

	Bool isEntityReference(
	) const;

	Bool isText(
	) const;

	Bool isEntity(
	) const;

	Bool isNotation(
	) const;

	Bool isProcessingInstruction(
	) const;

	Bool isCharacterData(
	) const;

	Bool isComment(
	) const;

	NodeType getNodeType(
	) const {
		return nodeType;
	}

	virtual void save(
			IO::DevicePtr	oDevice,
			int,
			int
	) const;

	NodeImplPtr	prev;
	NodeImplPtr	next;
	NodeImplPtr	ownr;	///< Parent or owner document.
	NodeImplPtr	first;
	NodeImplPtr	last;

	Buffer		name;	// this is the local name if prefix != null
	Buffer		value;
	Buffer		prefix;	// set this only for ElementNode and AttributeNode
	Buffer		nsURI;	// set this only for ElementNode and AttributeNode
	Bool		createdWithDom1Interface;
	Bool		hasParent;

	NodeType	nodeType;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NodeImpl_H_

// ============================================================================

