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
//	BFC.DOM.Locator.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_Locator_H_
#define _BFC_DOM_Locator_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.LocatorPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DOM.XNodePtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Locator.
///
/// Locator is an interface that describes a location (e.g. where an error
/// occurred).
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL Locator : virtual public SObject {

public :

	static const Uint32 NoOffset = ( Uint32 )-1;

	/// \brief Returns the line number this locator is pointing to, or
	///	NoOffset if there is no column number available.

	virtual Uint32 getLineNumber(
	) const = 0;

	/// \brief Returns the column number this locator is pointing to, or
	///	NoOffset if there is no column number available.

	virtual Uint32 getColumnNumber(
	) const = 0;

	/// \brief Returns the byte offset into the input source this locator
	///	is pointing to or NoOffset if there is no byte offset
	///	available.

	virtual Uint32 getByteOffset(
	) const = 0;

	/// \brief Returns the UTF-16, as defined in [Unicode] and Amendment 1
	///	of [ISO/IEC 10646], offset into the input source this locator
	///	is pointing to or NoOffset if there is no UTF-16 offset
	///	available.

	virtual Uint32 getUTF16Offset(
	) const = 0;

	/// \brief Returns the node this locator is pointing to, or null if no
	///	node is available.

	virtual XNodePtr getRelatedNode(
	) const = 0;

	/// \brief Returns the URI this locator is pointing to, or null if no
	///	URI is available.

	virtual const Buffer & getURI(
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Locator_H_

// ============================================================================

