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
//	BFC.DOM.EntityImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_EntityImpl_H_
#define _BFC_DOM_EntityImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.EntityImplPtr.h"

// ============================================================================

#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.XEntity.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class EntityImpl :

	virtual public XEntity,
	public NodeImpl {

public :

	EntityImpl(
			DocumentImplPtr,
			NodeImplPtr	parent,
		const	Buffer &	name,
		const	Buffer &	pub,
		const	Buffer &	sys,
		const	Buffer &	notation
	);

	EntityImpl(
			EntityImplPtr	n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	// --------------------------------------------------------------------
	// Implementation of the XEntity interface.

	virtual const Buffer & getPublicId(
	) const;

	virtual const Buffer & getSystemId(
	) const;

	virtual const Buffer & getNotationName(
	) const;

	virtual const Buffer & getInputEncoding(
	) const;

	virtual const Buffer & getXMLEncoding(
	) const;

	virtual const Buffer & getXMLVersion(
	) const;

	virtual void save(
			IO::DevicePtr	s,
			int,
			int
	) const;

private :

	Buffer	m_sys;
	Buffer	m_pub;
	Buffer	m_notationName;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_EntityImpl_H_

// ============================================================================

