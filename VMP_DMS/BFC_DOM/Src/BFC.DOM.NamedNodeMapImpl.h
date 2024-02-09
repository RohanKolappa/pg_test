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
//	BFC.DOM.NamedNodeMapImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NamedNodeMapImpl_H_
#define _BFC_DOM_NamedNodeMapImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.NamedNodeMapImplPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.TL.Map.h"

#include "BFC.DOM.XNamedNodeMap.h"
#include "BFC.DOM.NodeImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class NamedNodeMapImpl : virtual public XNamedNodeMap {

public:

	NamedNodeMapImpl(
			NodeImplPtr
	);

	NamedNodeMapImplPtr clone(
			NodeImplPtr	parent
	);

	void clearMap(
	);

	Uint32 length(
	) const;

	NodeImplPtr getNamedItem(
		const	Buffer &	name
	) const;

	NodeImplPtr getNamedItemNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName
	) const;

	NodeImplPtr setNamedItem(
			NodeImplPtr	arg
	);

	NodeImplPtr setNamedItemNS(
			NodeImplPtr	arg
	);

	NodeImplPtr removeNamedItem(
		const	Buffer &	name
	);

	NodeImplPtr getItem(
		const	Uint32		index
	) const;

	Bool contains(
		const	Buffer &	name
	) const;

	Bool containsNS(
		const	Buffer &	nsURI,
		const	Buffer &	localName
	) const;

	Bool isReadOnly(
	) const {
		return readonly;
	}

	void setReadOnly(
		const	Bool		r
	) {
		readonly = r;
	}

	Bool isAppendToParent(
	) const {
		return appendToParent;
	}

	/**
	 * If true, then the node will redirect insert/remove calls
	 * to its parent by calling DOM::NodeImpl::appendChild or removeChild.
	 * In addition the map wont increase or decrease the reference count
	 * of the nodes it contains.
	 *
	 * By default this value is false and the map will handle reference counting
	 * by itself.
	 */

	void setAppendToParent(
		const	Bool		b
	) {
		appendToParent = b;
	}

	Map< Buffer, NodeImplPtr >	map;
	NodeImplPtr			parent;
	Bool				readonly;
	Bool				appendToParent;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NamedNodeMapImpl_H_

// ============================================================================

