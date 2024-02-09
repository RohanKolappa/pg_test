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
//	BFC.Image.ResolutionRegistry.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ResolutionRegistry_H_
#define _BFC_Image_ResolutionRegistry_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Image {

BFC_PTR_DECL( BFC_Image_DLL, ResolutionRegistry )

} // namespace Image
} // namespace BFC

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.Image.ResolutionManifest.h"

#include "BFC.TL.Array.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Database of ResolutionManifest objects.
///
/// \ingroup BFC_Image

class BFC_Image_DLL ResolutionRegistry : virtual public SObject {

protected :

	/// \brief Creates a new ResolutionRegistry.

	ResolutionRegistry(
	);

public :

	/// \brief Destroys this object.

	virtual ~ResolutionRegistry(
	);

	static ResolutionRegistryPtr instance(
	);

	Uint32 getNbrManifests(
	) const {
		return db.getSize();
	}

	const ResolutionManifest & getManifest(
		const	Uint32		index
	) const {
		return db[ index ];
	}

	Uint32 getIndex(
		const	UUId &		uuid
	) const;

	const ResolutionManifest & getManifest(
		const	UUId &		uuid
	) const {
		return getManifest( getIndex( uuid ) );
	}

	Uint32 addManifest(
		const	ResolutionManifest &	params
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "BFC.Image.ResolutionRegistry" );

	CLASS_EXCEPTION( Duplicate, "Duplicate" );
	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );
	CLASS_EXCEPTION( NotDefined, "Not defined" );

protected :

private :

	Array< ResolutionManifest >	db;

	/// \brief Forbidden copy constructor.

	ResolutionRegistry(
		const	ResolutionRegistry &
	);

	/// \brief Forbidden copy operator.

	ResolutionRegistry & operator = (
		const	ResolutionRegistry &
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ResolutionRegistry_H_

// ============================================================================

