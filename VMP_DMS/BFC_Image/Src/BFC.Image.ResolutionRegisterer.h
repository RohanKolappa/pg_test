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
//	BFC.Image.ResolutionRegisterer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ResolutionRegisterer_H_
#define _BFC_Image_ResolutionRegisterer_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Image {

BFC_PTR_DECL( BFC_Image_DLL, ResolutionRegisterer )

} // namespace Image
} // namespace BFC

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Image.ResolutionManifest.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Registerer for ResolutionManifest objects.
///
/// \ingroup BFC_Image

class BFC_Image_DLL ResolutionRegisterer : virtual public SObject {

public :

	/// \brief Creates a new ResolutionRegisterer.

	ResolutionRegisterer(
		const	ResolutionManifest &	manifest
	);

	Uint32 getIndex(
	) const {
		return idx;
	}

private :

	Uint32	idx;

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ResolutionRegisterer_H_

// ============================================================================

