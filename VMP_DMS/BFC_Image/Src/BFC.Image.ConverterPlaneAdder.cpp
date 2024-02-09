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
//	BFC.Image.ConverterPlaneAdder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.ConverterPlaneAdder.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::ConverterPlaneAdder::ConverterPlaneAdder(
	const	PictureFormat &	pIFormat,
	const	PictureFormat &	pOFormat ) :

	Converter( pIFormat, pOFormat ) {

}

// ============================================================================

Image::Picture Image::ConverterPlaneAdder::convertForward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::ConverterPlaneAdder::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	PlaneArray	oPlanes;

	oPlanes.resize( 4 );

	oPlanes[ 0 ] = iPic.getPlane( 0 );
	oPlanes[ 1 ] = iPic.getPlane( 1 );
	oPlanes[ 2 ] = iPic.getPlane( 2 );
	oPlanes[ 3 ] = Plane(
		iPic.getWidth(),
		iPic.getHeight(),
		getOFormat().getPlane( 3 ) );

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getOFormat(),
			oPlanes );

}

Image::Picture Image::ConverterPlaneAdder::convertBackward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::ConverterPlaneAdder::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	PlaneArray	oPlanes;

	oPlanes.resize( 3 );

	oPlanes[ 0 ] = iPic.getPlane( 0 );
	oPlanes[ 1 ] = iPic.getPlane( 1 );
	oPlanes[ 2 ] = iPic.getPlane( 2 );

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getIFormat(),
			oPlanes );

}

// ============================================================================

static Image::ConverterPtr dummy_RGB_RGBA = new Image::ConverterPlaneAdder(
		Image::PictureFormat::RGB,
		Image::PictureFormat::RGBA );

static Image::ConverterPtr dummy_YUV444_YUV444A = new Image::ConverterPlaneAdder(
		Image::PictureFormat::YUV444P,
		Image::PictureFormat::YUVA444P );

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

