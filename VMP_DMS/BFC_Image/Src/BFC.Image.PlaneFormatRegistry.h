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
//	BFC.Image.PlaneFormatRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_PlaneFormatRegistry_H_
#define _BFC_Image_PlaneFormatRegistry_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Image {

BFC_PTR_DECL( BFC_Image_DLL, PlaneFormatRegistry )

} // namespace Image
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"
#include "BFC.Image.PlaneFormatManifest.h"
#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Database of PlaneFormatManifest objects.
///
/// \ingroup BFC_Image

class BFC_Image_DLL PlaneFormatRegistry : virtual public SObject {

protected :

	/// \brief Creates a new PlaneFormatRegistry.

	PlaneFormatRegistry(
	);

public :

	static PlaneFormatRegistryPtr instance(
	);

	Uint32 getNbrManifests(
	) const {
		return db.getSize();
	}

	const PlaneFormatManifest & getManifest(
		const	Uint32		index
	) const {
		return db[ index ];
	}

	Uint32 getIndex(
		const	UUId &		uuid
	) const;

	Uint32 getIndex(
		const	ColorChannel &	pColorChannel,
		const	Uint32		pBitsPerPixel,
		const	Uint32		pPixelsPerElt,
		const	Uint32		pMinXDownsampling,
		const	Uint32		pMinYDownsampling,
		const	Uint32		pMaxXDownsampling,
		const	Uint32		pMaxYDownsampling
	) const;

	const PlaneFormatManifest & getManifest(
		const	UUId &		uuid
	) const {
		return getManifest( getIndex( uuid ) );
	}

	Uint32 addManifest(
		const	PlaneFormatManifest &	params
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Image.PlaneFormatRegistry" );

	CLASS_EXCEPTION( Duplicate, "Duplicate" );
	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );
	CLASS_EXCEPTION( NotDefined, "Not defined" );

protected :

private :

	Array< PlaneFormatManifest >	db;

	/// \brief Forbidden copy constructor.

	PlaneFormatRegistry(
		const	PlaneFormatRegistry&
	);

	/// \brief Forbidden copy operator.

	PlaneFormatRegistry& operator = (
		const	PlaneFormatRegistry&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_PlaneFormatRegistry_H_

// ============================================================================

