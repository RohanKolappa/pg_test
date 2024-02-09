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
//	BFC.DOM.NotationImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_NotationImpl_H_
#define _BFC_DOM_NotationImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.NotationImplPtr.h"

// ============================================================================

#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.NodeImpl.h"
#include "BFC.DOM.XNotation.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class NotationImpl :

	virtual public XNotation,
	public NodeImpl {

public :

	NotationImpl(
		const	DocumentImplPtr &
					owner,
		const	NodeImplPtr &	parent,
		const	Buffer &	name,
		const	Buffer &	pub,
		const	Buffer &	sys
	);

	NotationImpl(
		const	NotationImplPtr	&
					n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	virtual const Buffer & getPublicId(
	) const;

	virtual const Buffer & getSystemId(
	) const;

	virtual void save(
			IO::DevicePtr	s,
			int,
			int
	) const;

private :

	Buffer	m_sys;
	Buffer	m_pub;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NotationImpl_H_

// ============================================================================

