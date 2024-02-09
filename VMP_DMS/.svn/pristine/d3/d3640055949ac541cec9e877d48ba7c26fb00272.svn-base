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
//	BFC.DOM.XNotation.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_XNotation_H_
#define _BFC_DOM_XNotation_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.DOM.XNotationPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.DOM.XNode.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Represents a DOM Notation.
///
/// This interface represents a notation declared in the DTD. A notation either
/// declares, by name, the format of an unparsed entity (see section 4.7 of the
/// XML 1.0 specification [XML 1.0]), or is used for formal declaration of
/// processing instruction targets (see section 2.6  of the XML 1.0
/// specification [XML 1.0]). The nodeName attribute inherited from Node is set
/// to the declared name of the notation.
///
/// The DOM Core does not support editing Notation nodes; they are therefore
/// readonly.
///
/// A Notation node does not have any parent.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL XNotation : virtual public XNode {

public :

	/// \brief Returns the public identifier of this notation.
	///
	/// If the public identifier was not specified, this is null.

	virtual const Buffer & getPublicId(
	) const = 0;

	/// \brief Returns the system identifier of this notation.
	///
	/// If the system identifier was not specified, this is null. This may
	/// be an absolute URI or not.

	virtual const Buffer & getSystemId(
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_XNotation_H_

// ============================================================================

