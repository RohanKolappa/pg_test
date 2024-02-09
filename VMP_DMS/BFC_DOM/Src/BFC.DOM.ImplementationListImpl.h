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
//	BFC.DOM.ImplementationListImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_ImplementationListImpl_H_
#define _BFC_DOM_ImplementationListImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

BFC_PTR_DECL( BFC_DOM_DLL, ImplementationListImpl )

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.DOM.ImplementationList.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Implements ImplementationList.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ImplementationListImpl : virtual public ImplementationList {

public :

	/// \brief Creates a new ImplementationListImpl.

	ImplementationListImpl(
	);

	virtual ImplementationCPtr getItem(
		const	Uint32		pIndex
	) const;

	virtual Uint32 getLength(
	) const;

protected :

private :

	Array< ImplementationCPtr >	lst;

	/// \brief Forbidden copy constructor.

	ImplementationListImpl(
		const	ImplementationListImpl &
	);

	/// \brief Forbidden copy operator.

	ImplementationListImpl & operator = (
		const	ImplementationListImpl &
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ImplementationListImpl_H_

// ============================================================================

