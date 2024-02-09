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
//	BFC.DOM.CharacterDataImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_CharacterDataImpl_H_
#define _BFC_DOM_CharacterDataImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CharacterDataImplPtr.h"

// ============================================================================

#include "BFC.DOM.XCharData.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class CharacterDataImpl :

	virtual public XCharData,
	public NodeImpl {

protected :

	CharacterDataImpl(
		const	NodeType	nodeType,
			DocumentImplPtr	d,
			NodeImplPtr	parent,
		const	Buffer &	data
	);

	CharacterDataImpl(
		const	NodeType	nodeType,
			CharacterDataImplPtr	n,
			Bool		deep
	);

public :

	CharacterDataImpl(
			DocumentImplPtr	d,
			NodeImplPtr	parent,
		const	Buffer &	data
	);

	CharacterDataImpl(
			CharacterDataImplPtr	n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	// --------------------------------------------------------------------
	// Implementation of XCharData interface.

	virtual const Buffer & getData(
	) const;

	virtual void setData(
		const	Buffer &	pData
	);

	virtual Uint32 getLength(
	) const;

	virtual Buffer substringData(
		const	Uint32		offset, 
		const	Uint32		count
	) const;

	virtual void appendData(
		const	Buffer &	arg
	);

	virtual void insertData(
		const	Uint32		offset, 
		const	Buffer &	arg
	);

	virtual void deleteData(
		const	Uint32		offset, 
		const	Uint32		count
	);

	virtual void replaceData(
		const	Uint32		offset, 
		const	Uint32		count, 
		const	Buffer &	arg
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_CharacterDataImpl_H_

// ============================================================================

