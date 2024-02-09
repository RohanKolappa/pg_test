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
//	BFC.DOM.ImplementationList.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_ImplementationList_H_
#define _BFC_DOM_ImplementationList_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.ImplementationListPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.ImplementationPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents an ordered collection of Implementation.
///
/// The ImplementationList interface provides the abstraction of an ordered
/// collection of DOM implementations, without defining or constraining how
/// this collection is implemented. The items in the ImplementationList are
/// accessible via an integral index, starting from 0.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ImplementationList : virtual public SObject {

public :

	/// \brief Returns the \a pIndex-th item in the collection.
	///
	/// If \a pIndex is greater than or equal to the number of
	/// Implementations in the list, this returns null.
	///
	/// \param pIndex
	///	Index into the collection.
	///
	/// \returns
	///	The Implementation at the \a pIndex-th position in the
	///	ImplementationList, or null if that is not a valid index.

	virtual ImplementationCPtr getItem(
		const	Uint32		pIndex
	) const = 0;

	/// \brief Returns the number of Implementations in the list.
	///
	/// The range of valid child node indices is 0 to getLength()-1,
	/// inclusive.

	virtual Uint32 getLength(
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ImplementationList_H_

// ============================================================================

