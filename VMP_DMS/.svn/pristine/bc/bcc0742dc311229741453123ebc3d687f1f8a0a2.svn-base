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
//	BFC.Image.Converter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Image_Converter_H_
#define _BFC_Image_Converter_H_

// ============================================================================

#include "BFC.Image.DLL.h"

// ============================================================================

#include "BFC.Image.ConverterPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Image.PictureFormat.h"

// ============================================================================

namespace BFC {
namespace Image {

// ============================================================================

class Picture;

// ============================================================================

/// Virtual base class of all picture format converters.
///
/// \ingroup BFC_Image

class BFC_Image_DLL Converter : virtual public SObject {

protected :

	Converter(
		const	PictureFormat &	pIFormat,
		const	PictureFormat &	pOFormat,
		const	Bool		pPreferred = false
	);

public :

	const PictureFormat & getIFormat(
	) const {
		return iformat;
	}

	const PictureFormat & getOFormat(
	) const {
		return oformat;
	}

	virtual Picture convertForward(
		const	Picture &	pIPicture
	) = 0;

	virtual void convertForward(
		const	Picture &	pIPicture,
			Picture &	pOPicture
	) = 0;

	virtual Picture convertBackward(
		const	Picture &	pIPicture
	) = 0;

	virtual void convertBackward(
		const	Picture &	pIPicture,
			Picture &	pOPicture
	) = 0;

protected :

	void transformRGB2YUV(
		const	Int32		R,
		const	Int32		G,
		const	Int32		B,
			Int32 &		Y,
			Int32 &		U,
			Int32 &		V
	) {
		Y = ( ( 16829 * R + 33039 * G +  6416 * B + 1081344 ) >> 16 );
		U = ( ( -9714 * R - 19070 * G + 28784 * B + 8421376 ) >> 16 );
		V = ( ( 28784 * R - 24103 * G -  4681 * B + 8421376 ) >> 16 );
	}

	void transformYUV2RGB(
		const	Int32		Y,
		const	Int32		U,
		const	Int32		V,
			Int32 &		R,
			Int32 &		G,
			Int32 &		B
	) {
		R = ( ( 76309 * Y              + 104597 * V - 14576592 ) >> 16 );
		G = ( ( 76309 * Y -  25674 * U -  53279 * V +  8917808 ) >> 16 );
		B = ( ( 76309 * Y + 132202 * U              - 18110032 ) >> 16 );
		if ( R < 0 ) R = 0; else if ( R > 255 ) R = 255;
		if ( G < 0 ) G = 0; else if ( G > 255 ) G = 255;
		if ( B < 0 ) B = 0; else if ( B > 255 ) B = 255;
	}

private :

	// internal state...

	PictureFormat	iformat;
	PictureFormat	oformat;

	// others...

	Converter(
	);

	Converter(
		const	Converter&
	);

	Converter& operator = (
		const	Converter&
	);

};

// ============================================================================

} // namespace Image
} // namespace BFC

// ============================================================================

#endif // _BFC_Image_Converter_H_

// ============================================================================

