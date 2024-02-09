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
//	BFC.Image.ColorSpace.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_ColorSpace_H_
#define _BFC_Image_ColorSpace_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

/// \brief Enumerated type representing color spaces.
///
/// [src: Wikipedia]
///
/// A color model is an abstract mathematical model describing the way
/// colors can be represented as tuples of numbers, typically as three or
/// four values or color components (e.g. RGB and CMYK are color models).
/// However, a color model with no associated mapping function to an
/// absolute color space is a more or less arbitrary color system with
/// little connection to the requirements of any given application.
///
/// Adding a certain mapping function between the color model and a
/// certain reference color space results in a definite "footprint" within
/// the reference color space. This "footprint" is known as a gamut, and,
/// in combination with the color model, defines a new color space. For
/// example, Adobe RGB and sRGB are two different absolute color spaces,
/// both based on the RGB model.
///
/// When formally defining a color space, the usual reference standard is
/// the CIELAB or CIEXYZ color spaces, which were specifically designed to
/// encompass all colors the average human can see. This is the most
/// accurate color space but is too complex for everyday uses.
///
/// Since "color space" is a more specific term for a certain combination
/// of a color model plus a color mapping function, the term "color space"
/// tends to be used to also identify color models, since identifying a
/// color space automatically identifies the associated color model.
/// Informally, the two terms are often used interchangeably, though this
/// is strictly incorrect. For example, although several specific color
/// spaces are based on the RGB model, there is no such thing as "the" RGB
/// color space.
///
/// The RGB color model is implemented in different ways, depending on the
/// capabilities of the system used. By far the most common general-use
/// incarnation is the 24-bit implementation, with 8 bits, or 256 discrete
/// levels of color per channel. Any color space based on such a 24-bit
/// RGB model is thus limited to a gamut of 256×256×256 == 16.7 million
/// colors. Some implementations use 16 bits per component, resulting in
/// the same range with a greater density of distinct colors. This is
/// especially important when working with wide-gamut color spaces (where
/// most of the more common colors are located relatively close together),
/// or when a large number of digital filtering algorithms are used
/// consecutively. The same principle applies for any color spaces based
/// on the same color model, but implemented in different bit depths.
///
/// -	CIE 1931 XYZ color space is the first attempt to produce a color
///	space based on measurements of human color perception and it is
///	the basis for almost all other color spaces. Variants of the CIE
///	space include:
///	-	CIELUV color space - a modification to display color
///		differences more conveniently, replaced by:
///	-	CIELAB color space
///	-	CIE 1964 color space - measurements over a larger field of
///		view than the 1931 color space which produces slightly
///		different results.
///
/// -	RGB uses additive color mixing, because it describes what kind of
///	light needs to be emitted to produce a given color. Light is added
///	together to create form from out of the darkness. RGB stores
///	individual values for red, green and blue. RGBA is RGB with an
///	additional channel, alpha, to indicate transparency. Common color
///	spaces based on the RGB model include sRGB, Adobe RGB and Adobe Wide
///	Gamut RGB.
///
///	\image html AdditiveColorMixing.png "Additive color mixing"
///
/// -	CMYK (sometimes pronounced C-Mike)
///	uses subtractive color mixing used in the printing process,
///	because it describes what kind of inks need to be applied so the
///	light reflected from the substrate and through the inks produces a
///	given color. One starts with a white substrate (canvas, page, etc),
///	and uses ink to subtract color from white to create an image. CMYK
///	stores ink values for cyan, magenta, yellow and black. There are many
///	CMYK color spaces for different sets of inks, substrates, and press
///	characteristics (which change the dot gain or transfer function for
///	each ink and thus change the appearance).
///
///	\image html RGB_CMYK_4.jpg "Comparison of RGB and CMYK color spaces"
///
///	\image html CMYK_color_swatches.png "Color swatches showing CMYK"
///
/// -	YIQ is used in NTSC (North American) television broadcasts for
///	historical reasons. YIQ stores a luminance value with two chrominance
///	values, corresponding approximately to the amounts of blue and red in
///	the color. It corresponds closely to the YUV scheme used in PAL
///	television except that the YIQ color space is rotated 33° with respect
///	to the YUV color space. The YDbDr scheme used by SECAM television is
///	rotated in another way.
///
/// -	YPbPr is a scaled version of YUV. It is most commonly seen in its
///	digital form, YCbCr, used widely in video and image compression
///	schemes such as MPEG and JPEG.
///
/// \image html Yuv.png "Example of U-V color plane, Y value = 0.5"
///
/// -	HSV (hue, saturation, value), also known as HSB (hue, saturation,
///	brightness) is often used by artists because it is often more natural
///	to think about a color in terms of hue and saturation than in terms of
///	additive or subtractive color components. HSV is a transformation of
///	an RGB colorspace, and its components and colorimetry are relative to
///	the RGB colorspace from which it was derived.
///
/// -	HSL (hue, saturation, lightness/luminance), also known as HLS or HSI
///	(hue, saturation, intensity) is quite similar to HSV, with "lightness"
///	replacing "brightness". The difference is that the brightness of a
///	pure color is equal to the brightness of white, while the lightness of
///	a pure color is equal to the lightness of a medium gray.
///
/// -	RYB is a historical set of subtractive primary colors. It is primarily
///	used in art and art education, particularly painting. It predates
///	modern scientific color theory and does not correspond to the peak
///	responsivities of the trichromat cone cells.
///
///	\image html RYBColorMixture.gif "Mixture of RYB primary colors"
///
///	\image html RYBColorWheel.png "Standard RYB color wheel"
///
/// -	YUV, YCbCr, YCbCr-JPEG Color space<br>
///		In fact YUV refers to the analogue TV signals, while YCbCr refers to its digital equivalent.
///		In practice these terms are used interchangeably
///	-		digital formulas:<br>
///			Y  = 16  + 0.2568 * R + 0.5042 * G + 0.0979 * B<br>
///			Cb = 128 - 0.1482 * R - 0.2910 * G + 0.4392 * B<br>
///			Cr = 128 + 0.4392 * R - 0.3678 * G - 0.0714 * B<br>
///	-		JPEG allows YCbCr where Y, Cb and Cr have the full 256 values:<br>
///			Y =        0.299    * R + 0.587    * G + 0.114    * B<br>
///			Cb = 128 - 0.168736 * R - 0.331264 * G + 0.5      * B<br>
///			Cr = 128 + 0.5      * R - 0.418688 * G - 0.081312 * B<br>
///
///
/// The characteristics of a color space are:
/// - the name of the color space;
/// - the number of separate channels.
///
/// \ingroup BFC_Image

class BFC_Image_DLL ColorSpace {

public :

	/// \brief Enumeration of various predefined types.

	enum Enum {

		Unknown	= 0,	///< Unknown color space.

		RGB,
		YUV,		///< YUV (ITU 601 : Y:16-235, U:16-240, V:16-240)
		JYUV,		///< YUV (ITU 601 : Y:0-255, U:0-255, V:0-255) (JPEG)
		Gray,
		XYZ,
		LAB,
		HLS,
		HSV,
		RYB,
		CMYK,
		RYUV		///< YUV-like, but reversible.

	};

	ColorSpace(
		const	Enum		pValue = Unknown
	) : value( pValue ) {
	}

	ColorSpace(
		const	ColorSpace &	pOther
	) : value( pOther.value ) {
	}

	/* virtual */ ~ColorSpace(
	) {
	}

	const char * getName(
	) const {
		return dataTbl[ value ].name;
	}

	const char *name(
	) const {
		return getName();
	}

	Uint32 getNbrChannels(
	) const {
		return dataTbl[ value ].chans;
	}

	ColorSpace& operator = (
		const	ColorSpace &	pOther
	) {
		if ( this != &pOther ) {
			value = pOther.value;
		}
		return *this;
	}

	Bool operator == (
		const	ColorSpace &	pOther
	) const {
		return ( value == pOther.value );
	}

	Bool operator != (
		const	ColorSpace &	pOther
	) const {
		return ( value != pOther.value );
	}

	Enum getEnum(
	) const {
		return value;
	}

private :

	struct DataTblEntry {
		const	char*	name;
			Uint32	chans;
	};

	Enum				value;
	static const DataTblEntry	dataTbl[];

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_ColorSpace_H_

// ============================================================================

