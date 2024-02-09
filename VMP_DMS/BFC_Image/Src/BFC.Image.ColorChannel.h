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
//	BFC.Image.ColorChannel.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ColorChannel_H_
#define _BFC_Image_ColorChannel_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.ColorSpace.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Represents a single color channel of a specific color space.
///
/// \ingroup BFC_Image

class BFC_Image_DLL ColorChannel {

public :

	/// \brief Enumeration of various predefined types.

	enum Enum {

		Unknown		= 0 | ( ColorSpace::Unknown << 8 ),

		Gray		= ( 1 << 0 ) | ( ColorSpace::Gray << 8 ),

		RGB_R		= ( 1 << 0 ) | ( ColorSpace::RGB << 8 ),
		RGB_G		= ( 1 << 1 ) | ( ColorSpace::RGB << 8 ),
		RGB_B		= ( 1 << 2 ) | ( ColorSpace::RGB << 8 ),
		RGB_A		= ( 1 << 3 ) | ( ColorSpace::RGB << 8 ),

		RGB_All		= RGB_R | RGB_G | RGB_B,
		RGBA_All	= RGB_R | RGB_G | RGB_B | RGB_A,

		YUV_Y		= ( 1 << 0 ) | ( ColorSpace::YUV << 8 ),
		YUV_U		= ( 1 << 1 ) | ( ColorSpace::YUV << 8 ),
		YUV_V		= ( 1 << 2 ) | ( ColorSpace::YUV << 8 ),
		YUV_A		= ( 1 << 3 ) | ( ColorSpace::YUV << 8 ),

		YUV_All		= YUV_Y | YUV_U | YUV_V,
		YUVA_All	= YUV_Y | YUV_U | YUV_V | YUV_A,

		JYUV_Y		= ( 1 << 0 ) | ( ColorSpace::JYUV << 8 ),
		JYUV_U		= ( 1 << 1 ) | ( ColorSpace::JYUV << 8 ),
		JYUV_V		= ( 1 << 2 ) | ( ColorSpace::JYUV << 8 ),
		JYUV_A		= ( 1 << 3 ) | ( ColorSpace::JYUV << 8 ),

		JYUV_All	= JYUV_Y | JYUV_U | JYUV_V,
		JYUVA_All	= JYUV_Y | JYUV_U | JYUV_V | JYUV_A,

		RYUV_Y		= ( 1 << 0 ) | ( ColorSpace::RYUV << 8 ),
		RYUV_U		= ( 1 << 1 ) | ( ColorSpace::RYUV << 8 ),
		RYUV_V		= ( 1 << 2 ) | ( ColorSpace::RYUV << 8 ),
		RYUV_A		= ( 1 << 3 ) | ( ColorSpace::RYUV << 8 ),

		RYUV_All	= RYUV_Y | RYUV_U | RYUV_V,
		RYUVA_All	= RYUV_Y | RYUV_U | RYUV_V | RYUV_A

	};

	ColorChannel(
		const	Enum		pValue = Unknown
	) : value( pValue ) {
	}

	ColorChannel(
		const	ColorSpace &	pColorSpace
	) : value( ( Enum )( pColorSpace.getEnum() << 8 ) ) {
	}

	ColorChannel(
		const	ColorChannel &	pOther
	) : value( pOther.value ) {
	}

	/* virtual */ ~ColorChannel(
	) {
	}

	ColorChannel & operator = (
		const	ColorChannel &	pOther
	) {
		value = pOther.value;
		return *this;
	}

	Bool operator == (
		const	ColorChannel &	pOther
	) const {
		return ( value == pOther.value );
	}

	Bool operator != (
		const	ColorChannel &	pOther
	) const {
		return ( value != pOther.value );
	}

	ColorSpace getColorSpace(
	) const {
		return ColorSpace( ( ColorSpace::Enum )( value >> 8 ) );
	}

	Enum getPredefined(
	) const {
		return value;
	}

private :

	Enum	value;

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ColorChannel_H_

// ============================================================================

