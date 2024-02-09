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
//	BFC.Image.ConverterPlaneSwitcher.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.ConverterPlaneSwitcher.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::ConverterPlaneSwitcher::ConverterPlaneSwitcher(
	const	PictureFormat	& pIFormat,
	const	PictureFormat	& pOFormat,
	const	Uint32		pSrcPlane0,
	const	Uint32		pSrcPlane1,
	const	Uint32		pSrcPlane2 ) :

	Converter( pIFormat, pOFormat ),

	srcPlane0( pSrcPlane0 ),
	srcPlane1( pSrcPlane1 ),
	srcPlane2( pSrcPlane2 ) {

}

// ============================================================================

Image::Picture Image::ConverterPlaneSwitcher::convertForward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::ConverterPlaneSwitcher::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	PlaneArray	oPlanes;

	oPlanes.resize( iPic.getNbrPlanes() );

	if ( oPlanes.size() == 4 ) {
		oPlanes[ 3 ] = iPic.getPlane( 3 );
	}

	oPlanes[ 0 ] = iPic.getPlane( srcPlane0 );
	oPlanes[ 1 ] = iPic.getPlane( srcPlane1 );
	oPlanes[ 2 ] = iPic.getPlane( srcPlane2 );

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getOFormat(),
			oPlanes );

}

Image::Picture Image::ConverterPlaneSwitcher::convertBackward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::ConverterPlaneSwitcher::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	PlaneArray	oPlanes;

	oPlanes.resize( iPic.getNbrPlanes() );

	if ( oPlanes.size() == 4 ) {
		oPlanes[ 3 ] = iPic.getPlane( 3 );
	}

	oPlanes[ srcPlane0 ] = iPic.getPlane( 0 );
	oPlanes[ srcPlane1 ] = iPic.getPlane( 1 );
	oPlanes[ srcPlane2 ] = iPic.getPlane( 2 );

	oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getIFormat(),
			oPlanes );

}

// ============================================================================

static Image::ConverterPtr dummy_RGBA_BGRA = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::RGBA,
		Image::PictureFormat::BGRA,
		0, 1, 2 );

static Image::ConverterPtr dummy_RGB_BGR = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::RGB,
		Image::PictureFormat::BGR,
		0, 1, 2 );

static Image::ConverterPtr dummy_444A_444A = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUVA444P,
		Image::PictureFormat::YVUA444P,
		0, 2, 1 );

static Image::ConverterPtr dummy_444_444 = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUV444P,
		Image::PictureFormat::YVU444P,
		0, 2, 1 );

static Image::ConverterPtr dummy_422_422 = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUV422P,
		Image::PictureFormat::YVU422P,
		0, 2, 1 );

static Image::ConverterPtr dummy_420_420 = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUV420P,
		Image::PictureFormat::YVU420P,
		0, 2, 1 );

static Image::ConverterPtr dummy_411_411 = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUV411P,
		Image::PictureFormat::YVU411P,
		0, 2, 1 );

static Image::ConverterPtr dummy_410_410 = new Image::ConverterPlaneSwitcher(
		Image::PictureFormat::YUV410P,
		Image::PictureFormat::YVU410P,
		0, 2, 1 );

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

