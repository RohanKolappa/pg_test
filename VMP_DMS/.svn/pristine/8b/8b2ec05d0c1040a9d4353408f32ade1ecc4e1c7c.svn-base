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
//	BFC.DOM.DocumentTypeImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_DocumentTypeImpl_H_
#define _BFC_DOM_DocumentTypeImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.DocumentTypeImplPtr.h"

// ============================================================================

#include "BFC.DOM.XDocType.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.NamedNodeMapImplPtr.h"
#include "BFC.DOM.NodeImpl.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class DocumentTypeImpl :

	virtual public XDocType,
	public NodeImpl {

public :

	DocumentTypeImpl(
		const	DocumentImplPtr &
					pOwnerDoc,
		const	NodeImplPtr &	parent = 0
	);

	DocumentTypeImpl(
		const	DocumentTypeImplPtr &
					n,
			Bool		deep
	);

	void init(
	);

	virtual NodeImplPtr cloneNode(
			Bool		deep = true
	);

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

	virtual Bool isDocumentType(
	) const {
		return true;
	}

	virtual void save(
			IO::DevicePtr	oDevice,
			int,
			int
	) const;

	NamedNodeMapImplPtr entities;
	NamedNodeMapImplPtr notations;
	Buffer	publicId;
	Buffer	systemId;
	Buffer	internalSubset;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_DocumentTypeImpl_H_

// ============================================================================

