// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Image".
// 
// "BFC::Image" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Image" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Image"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.PlaneFormatManifest.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_PlaneFormatManifest_H_
#define _BFC_Image_PlaneFormatManifest_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.UUId.h"

#include "BFC.Dyn.VarUser.h"

#include "BFC.Image.ColorChannel.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Groups all information about a specific PlaneFormat into
///	a convenient structure.
///
/// A Plane is a matrix of units of data, each unit containing information
/// about one or more pixel(s), and/or about one or more color channels.
///
/// The properties of a Plane are:
/// -	color channel: represents the color channel(s) this plane provides.
/// -	bits per pixel: gives the number of bits each elementary unit of
///	data uses in a plane.
/// -	pixels per unit: number of pixels per elementary unit of data.
/// -	X downsampling: optional downsampling in the X direction (a minimum
///	and a maximum).
/// -	Y downsampling: optional downsampling in the Y direction (a minimum
///	and a maximum).
///
/// The minimum downsampling factors give the resolution of this Plane,
/// compared to an original Picture, while the maximum downsampling factors
/// indicate the possible loss of precision.
///
/// Example: unique plane of the PictureFormat::YUYV predefined format.
/// -	color channel: all YUV channels
/// -	bits per pixel: 16
/// -	pixels per unit: 2
/// -	minimum downsampling: (1,1)
/// -	maximum downsampling: (2,1)
///
/// Example: unique plane of the PictureFormat::RGB24 predefined format.
/// -	color channel: all RGB channels
/// -	bits per pixel: 24
/// -	pixels per unit: 1
/// -	minimum downsampling: (1,1)
/// -	maximum downsampling: (1,1)
///
/// Example: second and third planes in the planar YUV420P predefined format.
/// -	color channel: U or V channel
/// -	bits per pixel: 8
/// -	pixels per unit: 1
/// -	minimum downsampling: (2,2)
/// -	maximum downsampling: (2,2)
///
/// \ingroup BFC_Image

class BFC_Image_DLL PlaneFormatManifest : public Dyn::VarUser {

public :

	/// \brief Creates a new PlaneFormatManifest object.

	PlaneFormatManifest(
	);

	/// \brief Creates a new PlaneFormatManifest object.

	PlaneFormatManifest(
		const	UUId &			univUniqueId
	);

	/// \brief Creates a new PlaneFormatManifest object.

	PlaneFormatManifest(
		const	UUId &			univUniqueId,
		const	ColorChannel		colorChannel,
		const	Uint32			bitsPerPixel,
		const	Uint32			pixelsPerElt,
		const	Uint32			minXDownsampling,
		const	Uint32			minYDownsampling,
		const	Uint32			maxXDownsampling,
		const	Uint32			maxYDownsampling
	);

	/// \brief Copy constructor.

	PlaneFormatManifest(
		const	PlaneFormatManifest &	other
	);

	/// \brief Copy operator.

	PlaneFormatManifest & operator = (
		const	PlaneFormatManifest &	other
	);

	/// \brief Returns true iff this object is equal to \a other.

	Bool operator == (
		const	PlaneFormatManifest	& other
	) const {
		return ( univUniqueId == other.univUniqueId );
	}

	/// \brief Returns true iff this object is not equal to \a other.

	Bool operator != (
		const	PlaneFormatManifest	& other
	) const {
		return ( univUniqueId != other.univUniqueId );
	}

	const UUId & getUUId(
	) const {
		return univUniqueId;
	}

	Buffer toBuffer(
	) const;

private :

	UUId		univUniqueId;	///< UUId for this specific object.

public :

	ColorChannel	colorChannel;	///< Color channel(s).
	Uint32		bitsPerPixel;	///< Number of bits per pixel (ex. 7).
	Uint32		pixelsPerElt;	///< Number of pixels per elementary unit of data.
	Uint32		minXDownsmpl;	///< Minimum downsampling in the X direction.
	Uint32		minYDownsmpl;	///< Minimum downsampling in the Y direction.
	Uint32		maxXDownsmpl;	///< Maximum downsampling in the X direction.
	Uint32		maxYDownsmpl;	///< Maximum downsampling in the Y direction.

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_PlaneFormatManifest_H_

// ============================================================================

