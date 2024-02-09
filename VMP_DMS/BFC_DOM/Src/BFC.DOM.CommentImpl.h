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
//	BFC.DOM.CommentImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_CommentImpl_H_
#define _BFC_DOM_CommentImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.CommentImplPtr.h"

// ============================================================================

#include "BFC.DOM.CharacterDataImpl.h"
#include "BFC.DOM.XComment.h"
#include "BFC.DOM.DocumentImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class CommentImpl :

	virtual public XComment,
	public CharacterDataImpl {

public :

	CommentImpl(
			DocumentImplPtr	,
			NodeImplPtr	parent,
		const	Buffer &	val
	);

	CommentImpl(
			CommentImplPtr	n,
			Bool		deep
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
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

#endif // _BFC_DOM_CommentImpl_H_

// ============================================================================

