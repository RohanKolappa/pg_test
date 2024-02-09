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
//	BFC.DOM.NodeListImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NodeListImpl_H_
#define _BFC_DOM_NodeListImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.NodeListImplPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.NodeImplPtr.h"
#include "BFC.DOM.XNodeList.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class BFC_DOM_DLL NodeListImpl : virtual public XNodeList {

public:

	NodeListImpl(
		const	NodeImplPtr &	pNode
	);

	NodeListImpl(
		const	NodeImplPtr &	pNode,
		const	Buffer &	pName
	);

	NodeListImpl(
		const	NodeImplPtr &	pNode,
		const	Buffer &	pNsUri,
		const	Buffer &	pLocalName
	);

	Bool operator == (
		const	NodeListImpl &	pOther
	) const;

	Bool operator != (
		const	NodeListImpl &	pOther
	) const;

	NodeImplPtr getItem(
		const	Uint32		index
	) const;

	Uint32 length(
	) const;

protected :

	void createList(
	) const;

private :

	NodeImplPtr		node;
	Buffer			tagname;
	Buffer			nsURI;

	mutable Array< NodeImplPtr >	list;
	mutable Int64			stamp;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NodeListImpl_H_

// ============================================================================

