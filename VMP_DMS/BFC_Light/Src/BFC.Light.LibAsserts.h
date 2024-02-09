// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Light.LibAsserts.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_LibAsserts_H_
#define _BFC_Light_LibAsserts_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Light {

BFC_PTR_DECL( BFC_Light_DLL, LibAsserts )

} // namespace Light
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace Light {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Light

class BFC_Light_DLL LibAsserts : virtual public SObject {

protected :

	/// \brief Creates a new LibAsserts.

	LibAsserts(
	);

public :

	static LibAssertsPtr instance(
	);

};

// ============================================================================

} // namespace Light
} // namespace BFC

// ============================================================================

#endif // _BFC_Light_LibAsserts_H_

// ============================================================================

