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
//	BFC.Image.PictureFormatManifest.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_PictureFormatManifest_H_
#define _BFC_Image_PictureFormatManifest_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.UUId.h"

#include "BFC.Dyn.VarUser.h"

#include "BFC.Image.ColorSpace.h"
#include "BFC.Image.PlaneFormatManifest.h"

#include "BFC.TL.CompArray.h"
#include "BFC.TL.FourCCArray.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Groups all information about a specific PictureFormat into
///	a convenient structure.
///
/// \ingroup BFC_Image

class BFC_Image_DLL PictureFormatManifest : public Dyn::VarUser {

public :

	/// \brief Creates a new PictureFormatManifest.

	PictureFormatManifest(
	);

	/// \brief Creates a new PictureFormatManifest.

	PictureFormatManifest(
		const	UUId &		uuid
	);

	/// \brief Creates a new PictureFormatManifest.

	PictureFormatManifest(
		const	UUId &		uuid,
		const	Buffer &	fourCCs,
		const	Char *		shortName,
		const	Char *		longName,
		const	ColorSpace	colorSpace,
		const	Uint32		plane0
	);

	/// \brief Creates a new PictureFormatManifest.

	PictureFormatManifest(
		const	UUId &		uuid,
		const	Buffer &	fourCCs,
		const	Char *		shortName,
		const	Char *		longName,
		const	ColorSpace	colorSpace,
		const	Uint32		plane0,
		const	Uint32		plane1,
		const	Uint32		plane2
	);

	/// \brief Creates a new PictureFormatManifest.

	PictureFormatManifest(
		const	UUId &		uuid,
		const	Buffer &	fourCCs,
		const	Char *		shortName,
		const	Char *		longName,
		const	ColorSpace	colorSpace,
		const	Uint32		plane0,
		const	Uint32		plane1,
		const	Uint32		plane2,
		const	Uint32		plane3
	);

	/// \brief Copy constructor.

	PictureFormatManifest(
		const	PictureFormatManifest &	other
	);

	/// \brief Copy operator.

	PictureFormatManifest & operator = (
		const	PictureFormatManifest &	other
	);

	/// \brief Returns true iff this object is equal to \a __other.

	Bool operator == (
		const	PictureFormatManifest &	other
	) const {
		return ( uuid == other.uuid );
	}

	/// \brief Returns true iff this object is not equal to \a __other.

	Bool operator != (
		const	PictureFormatManifest &	other
	) const {
		return ( uuid != other.uuid );
	}

	const UUId & getUUId(
	) const {
		return uuid;
	}

	const FourCCArray & getFourCCs(
	) const {
		return fourCCs;
	}

private :

	UUId		uuid;		///< UUId for this specific object.

public :

	FourCCArray	fourCCs;	///< List or 4CC's.
	Buffer		shortName;	///< Short description.
	Buffer		longName;	///< Long description.
	ColorSpace	colorSpace;	///< Color space.
	Uint32		bitsPerPixel;	///< Mean taken over all planes.
	CompArray< Uint32 >
			planes;		///< Components indexes in PlaneFormatRegistry.
	Uint32		minXDownsmpl;	///< Minimum downsampling in the X direction.
	Uint32		minYDownsmpl;	///< Minimum downsampling in the Y direction.
	Uint32		maxXDownsmpl;	///< Maximum downsampling in the X direction.
	Uint32		maxYDownsmpl;	///< Maximum downsampling in the Y direction.

protected :

	/// \brief Parses the comma-separated list of 4CC's.

	void parseFourCCs(
		const	Buffer &	list
	);

	/// \brief Computes the number of bits per pixel and the
	///	downsampling factors.

	void computeCachedValues(
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_PictureFormatManifest_H_

// ============================================================================

