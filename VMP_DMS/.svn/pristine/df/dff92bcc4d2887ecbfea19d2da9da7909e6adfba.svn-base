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
//	BFC.DOM.NameList.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_NameList_H_
#define _BFC_DOM_NameList_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

BFC_PTR_DECL( BFC_DOM_DLL, NameList )

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a NameList.
///
/// The NameList interface provides the abstraction of an ordered collection of
/// parallel pairs of name and namespace values (which could be null values),
/// without defining or constraining how this collection is implemented. The
/// items in the NameList are accessible via an integral index, starting from
/// 0.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL NameList : virtual public SObject {

public :

	/// \brief Returns the \a pIndex-th name item in the collection.
	///
	/// \param pIndex
	///	Index into the collection.
	///
	/// \returns
	///	The name at the \a pIndex-th position in the NameList, or null
	///	if there is no name for the specified \a pIndex or if the \a
	///	pIndex is out of range.

	virtual const Buffer & getName(
		const	Uint32		pIndex
	) const = 0;

	/// \brief Returns the \a pIndex-th namespaceURI item in the
	///	collection.
	///
	/// \param pIndex
	///	Index into the collection.
	///
	/// \returns
	///	The namespace URI at the \a pIndex-th position in the NameList,
	///	or null if there is no name for the specified \a pIndex or if
	///	the \a pIndex is out of range.

	virtual const Buffer & getNamespaceURI(
		const	Uint32		pIndex
	) const = 0;

	/// \brief Returns the number of pairs (name and namespaceURI) in the
	///	list.
	///
	/// The range of valid child node indices is 0 to getLength()-1,
	/// inclusive.

	virtual Uint32 getLength(
	) const = 0;

	/// \brief Test if a name is part of this NameList.
	///
	/// \param pName
	///	The name to look for.
	///
	/// \returns
	///	true if the name has been found, false otherwise.

	virtual Bool contains(
		const	Buffer &	pName
	) const = 0;

	/// \brief Test if the pair \a pNamespaceURI/pName is part of this
	///	NameList.
	///
	/// \param pNamespaceURI
	///	The namespace URI to look for.
	///
	/// \param pName
	///	The name to look for.
	///
	/// \returns
	///	true if the pair \a pNamespaceURI/pName has been found, false
	///	otherwise.

	virtual Bool containsNS(
		const	Buffer &	pNamespaceURI,
		const	Buffer &	pName
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_NameList_H_

// ============================================================================

