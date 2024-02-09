// ============================================================================

#if defined( BFC_Image_Converter_COMPILE )

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
//	BFC.Image.Converter_RGB_GRAY8.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_GRAY8.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter_RGB_GRAY8::Converter_RGB_GRAY8() :

	Converter(
		PictureFormat::RGB,
		PictureFormat::GRAY8 ) {

}

// ============================================================================

Image::Picture Image::Converter_RGB_GRAY8::convertForward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::Converter_RGB_GRAY8::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getOFormat(),
			Plane::getMean(
				iPic.getPlane( 0 ),
				iPic.getPlane( 1 ),
				iPic.getPlane( 2 ) ) );

}

Image::Picture Image::Converter_RGB_GRAY8::convertBackward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::Converter_RGB_GRAY8::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getIFormat(),
			iPic.getPlane( 0 ),
			iPic.getPlane( 0 ),
			iPic.getPlane( 0 )
		);

}

// ============================================================================

static Image::ConverterPtr dummy_RGB_GRAY8 = new Image::Converter_RGB_GRAY8;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

