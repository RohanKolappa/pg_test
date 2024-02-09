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
//	BFC.DOM.TextImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_TextImpl_H_
#define _BFC_DOM_TextImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.TextImplPtr.h"

// ============================================================================

#include "BFC.DOM.CharacterDataImpl.h"
#include "BFC.DOM.XText.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class TextImpl :

	virtual public XText,
	public CharacterDataImpl {

protected :

	TextImpl(
		const	NodeType	nodeType,
			DocumentImplPtr	,
			NodeImplPtr	parent,
		const	Buffer &	val
	);

	TextImpl(
		const	NodeType	nodeType,
			TextImplPtr	n,
			Bool		deep
	);

public:

	TextImpl(
			DocumentImplPtr	,
			NodeImplPtr	parent,
		const	Buffer &	val
	);

	TextImpl(
			TextImplPtr	n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

	// --------------------------------------------------------------------
	// Implementation of the XText interface.

	virtual XTextPtr splitText(
		const	Uint32		offset
	);

	virtual Bool isElementContentWhitespace(
	) const;

	virtual const Buffer & getWholeText(
	) const;

	virtual XTextPtr replaceWholeText(
		const	Buffer &	content
	);

	virtual void save(
			IO::DevicePtr	s,
			int,
			int
	) const;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_TextImpl_H_

// ============================================================================

