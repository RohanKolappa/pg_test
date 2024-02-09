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
//	BFC.DOM.AttrImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_AttrImpl_H_
#define _BFC_DOM_AttrImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.AttrImplPtr.h"

// ============================================================================

#include "BFC.DOM.XAttr.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class AttrImpl :

	virtual public XAttr,
	public NodeImpl {

public:

	AttrImpl(
			DocumentImplPtr,
			NodeImplPtr,
		const	Buffer &	name
	);

	AttrImpl(
			DocumentImplPtr,
			NodeImplPtr,
		const	Buffer &	nsURI,
		const	Buffer &	qName
	);

	AttrImpl(
			AttrImplPtr	n,
			Bool		deep
	);

	virtual void setNodeValue(
		const	Buffer &	v
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	// --------------------------------------------------------------------
	// Implementation of the XAttr interface.

	virtual const Buffer & getName(
	) const;

	virtual Bool isSpecified(
	) const;

	virtual const Buffer & getValue(
	) const;

	virtual void setValue(
		const	Buffer &	pValue
	);

	virtual XElementPtr getOwnerElement(
	) const;

	virtual TypeInfoCPtr getSchemaTypeInfo(
	) const;

	virtual Bool isId(
	) const;

	virtual void save(
			IO::DevicePtr	s,
			int,
			int
	) const;

private :

	Bool	m_specified;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_AttrImpl_H_

// ============================================================================

