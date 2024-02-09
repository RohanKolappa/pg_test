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
//	BFC.DOM.InvalidDataPolicy.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_InvalidDataPolicy_H_
#define _BFC_DOM_InvalidDataPolicy_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

namespace DOM {

// ============================================================================

/// \brief Specifies what should be done when a factory function
///	in DOM::Document is called with invalid data.
///
/// \ingroup BFC_DOM

enum InvalidDataPolicy {

	/// \brief The data should be stored in the DOM object
	///	anyway. In this case the resulting XML document might
	///	not be well-formed.

	AcceptInvalidChars = 0,

	/// \brief The invalid characters should be removed from
	///	the data.

	DropInvalidChars,

	/// \brief The factory function should return a null node.

	ReturnNullNode

};

// ============================================================================

} // namespace DOM

} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_InvalidDataPolicy_H_

// ============================================================================

