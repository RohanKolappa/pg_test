// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.NameListImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_NameListImpl_H_
#define _BFC_DOM_NameListImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

BFC_PTR_DECL( BFC_DOM_DLL, NameListImpl )

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.DOM.NameList.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL NameListImpl : public NameList {

public :

	/// \brief Creates a new NameListImpl.

	NameListImpl(
	);

	virtual const Buffer & getName(
		const	Uint32		pIndex
	) const;

	virtual const Buffer & getNamespaceURI(
		const	Uint32		pIndex
	) const;

	virtual Uint32 getLength(
	) const;

	virtual Bool contains(
		const	Buffer &	str
	) const;

	virtual Bool containsNS(
		const	Buffer &	namespaceURI,
		const	Buffer &	name
	) const;

protected :

private :

	static const Buffer	empty;
	Map< Buffer, Buffer >	map;

	/// \brief Forbidden copy constructor.

	NameListImpl(
		const	NameListImpl &
	);

	/// \brief Forbidden copy operator.

	NameListImpl & operator = (
		const	NameListImpl &
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NameListImpl_H_

// ============================================================================

