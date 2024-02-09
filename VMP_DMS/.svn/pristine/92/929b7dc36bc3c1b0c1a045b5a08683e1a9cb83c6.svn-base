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
//	BFC.DOM.XNodeList.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XNodeList_H_
#define _BFC_DOM_XNodeList_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XNodeListPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.DOM.XNodePtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM NodeList.
///
/// The NodeList interface provides the abstraction of an ordered collection of
/// nodes, without defining or constraining how this collection is implemented.
/// NodeList objects in the DOM are live.
/// 
/// The items in the NodeList are accessible via an integral index, starting
/// from 0.
///
/// \since DOM Level 1
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XNodeList : virtual public SObject {

public :

#if 0

	/// \brief Returns the indexth item in the collection.
	///
	/// If index is greater than or equal to the number of nodes in the
	/// list, this returns null.
	///
	/// \param index
	///	Index into the collection.
	///
	/// \returns
	///	The node at the indexth position in the NodeList, or null if
	///	that is not a valid index.
	///
	/// \since DOM Level 1

	virtual XNodePtr getItem(
		const	Uint32		index
	) = 0;

	/// \brief Returns the number of nodes in the list.
	///
	/// The range of valid child node indices is 0 to length-1 inclusive.
	///
	/// \since DOM Level 1

	virtual Uint32 getLength(
	) const = 0;

#endif // 0

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XNodeList_H_

// ============================================================================

