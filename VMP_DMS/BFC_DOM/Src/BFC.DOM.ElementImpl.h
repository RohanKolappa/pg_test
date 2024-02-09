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
//	BFC.DOM.ElementImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_ElementImpl_H_
#define _BFC_DOM_ElementImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.ElementImplPtr.h"

// ============================================================================

#include "BFC.DOM.AttrImplPtr.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.XElement.h"
#include "BFC.DOM.NamedNodeMapImpl.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class ElementImpl :

	virtual public XElement,
	public NodeImpl {

public:

	ElementImpl(
			DocumentImplPtr,
			NodeImplPtr	parent,
		const	Buffer &	name
	);

	ElementImpl(
			DocumentImplPtr,
			NodeImplPtr	parent,
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	ElementImpl(
			ElementImplPtr	n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	Buffer getAttribute(
		const	Buffer &	name,
		const	Buffer &	defValue
	) const;

	Buffer getAttributeNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName,
		const	Buffer &	defValue
	) const;

	void setAttribute(
		const	Buffer &	name,
		const	Buffer &	value
	);

	void setAttributeNS(
		const	Buffer &	nsURI,
		const	Buffer &	qName,
		const	Buffer &	newValue
	);

	void removeAttribute(
		const	Buffer &	name
	);

	AttrImplPtr getAttributeNode(
		const	Buffer &	name
	) const;

	AttrImplPtr getAttributeNodeNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName
	) const;

	AttrImplPtr setAttributeNode(
			AttrImplPtr	newAttr
	);

	AttrImplPtr setAttributeNodeNS(
			AttrImplPtr	newAttr
	);

	AttrImplPtr removeAttributeNode(
			AttrImplPtr	oldAttr
	);

	Bool hasAttribute(
		const	Buffer &	name
	) const;

	Bool hasAttributeNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName
	) const;

	Buffer text(
	) const;

	virtual NamedNodeMapImplPtr getAttributes(
	) const {
		return m_attr;
	}

	virtual Bool hasAttributes(
	) {
		return (m_attr->length() > 0);
	}

	virtual Bool isElement(
	) const {
		return true;
	}

	virtual void save(
			IO::DevicePtr	s,
			int,
			int
	) const;

	NamedNodeMapImplPtr	m_attr;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ElementImpl_H_

// ============================================================================

