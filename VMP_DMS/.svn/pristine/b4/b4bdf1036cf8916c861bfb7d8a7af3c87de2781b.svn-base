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
//	BFC.Image.Converter_YUVxxxP_YUVxxxP.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUVxxxP_YUVxxxP.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter_YUVxxxP_YUVxxxP::Converter_YUVxxxP_YUVxxxP(
	const	PictureFormat	pIFormat,
	const	PictureFormat	pOFormat,
	const	Uint32		pIXDown,
	const	Uint32		pIYDown,
	const	Uint32		pOXDown,
	const	Uint32		pOYDown ) :

	Converter(pIFormat, pOFormat),

	ixdown(pIXDown),
	iydown(pIYDown),
	oxdown(pOXDown),
	oydown(pOYDown) {

}

// ============================================================================

Image::Picture Image::Converter_YUVxxxP_YUVxxxP::convertForward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::Converter_YUVxxxP_YUVxxxP::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	if ( iPic.getFormat() != getIFormat() ) {
		throw InternalError("Invalid input format!");
	}

	Uint32		picW = iPic.getWidth();
	Uint32		picH = iPic.getHeight();

//	if ( ! PictureFormat(getOFormat()).canRepresent(picW, picH) ) {
	if ( ! getOFormat().canRepresent(picW, picH) ) {
		throw InternalError("Can't represent picture in oformat!");
	}

	// Create output picture...

	PlaneArray	oPlanes;

	oPlanes = iPic.getPlanes();

	Int32		xdiff = ( Int32 )( ixdown ) - ( Int32 )( oxdown );
	Int32		ydiff = ( Int32 )( iydown ) - ( Int32 )( oydown );

	if ( xdiff == -1 && ydiff == -1 ) {
		oPlanes[1].downsampleXY2();
		oPlanes[2].downsampleXY2();
	}
	else if ( xdiff == -2 && ydiff == -2 ) {
		oPlanes[1].downsampleXY4();
		oPlanes[2].downsampleXY4();
	}
	else if ( xdiff == 1 && ydiff == 1 ) {
		oPlanes[1].upsampleXY2();
		oPlanes[2].upsampleXY2();
	}
	else if ( xdiff == 2 && ydiff == 2 ) {
		oPlanes[1].upsampleXY4();
		oPlanes[2].upsampleXY4();
	}
	else {
		if ( xdiff == -1 ) {
			oPlanes[1].downsampleX2();
			oPlanes[2].downsampleX2();
		}
		else if ( xdiff == -2 ) {
			oPlanes[1].downsampleX4();
			oPlanes[2].downsampleX4();
		}
		else if ( xdiff == 1 ) {
			oPlanes[1].upsampleX2();
			oPlanes[2].upsampleX2();
		}
		else if ( xdiff == 2 ) {
			oPlanes[1].upsampleX4();
			oPlanes[2].upsampleX4();
		}
		if ( iydown == oydown + 1 ) {
			oPlanes[1].downsampleY2();
			oPlanes[2].downsampleY2();
		}
		else if ( iydown == oydown + 2 ) {
			oPlanes[1].downsampleY4();
			oPlanes[2].downsampleY4();
		}
		else if ( iydown + 1 == oydown ) {
			oPlanes[1].upsampleY2();
			oPlanes[2].upsampleY2();
		}
		else if ( iydown + 2 == oydown ) {
			oPlanes[1].upsampleY4();
			oPlanes[2].upsampleY4();
		}
	}

	oPic = Picture(
			picW,
			picH,
			getOFormat(),
			oPlanes );

}

Image::Picture Image::Converter_YUVxxxP_YUVxxxP::convertBackward(
	const	Picture &	iPic ) {

	Picture oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::Converter_YUVxxxP_YUVxxxP::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	if ( iPic.getFormat() != getOFormat() ) {
		throw InternalError("Invalid input format!");
	}

	Uint32		picW = iPic.getWidth();
	Uint32		picH = iPic.getHeight();

//	if ( ! PictureFormat(getIFormat()).canRepresent(picW, picH) ) {
	if ( ! getIFormat().canRepresent(picW, picH) ) {
		throw InternalError("Can't represent picture in oformat!");
	}

	// Create output picture...

	PlaneArray	oPlanes;

	oPlanes = iPic.getPlanes();

	Int32		xdiff = ( Int32 )( oxdown ) - ( Int32 )( ixdown );
	Int32		ydiff = ( Int32 )( oydown ) - ( Int32 )( iydown );

	if ( xdiff == -1 && ydiff == -1 ) {
		oPlanes[1].downsampleXY2();
		oPlanes[2].downsampleXY2();
	}
	else if ( xdiff == -2 && ydiff == -2 ) {
		oPlanes[1].downsampleXY4();
		oPlanes[2].downsampleXY4();
	}
	else if ( xdiff == 1 && ydiff == 1 ) {
		oPlanes[1].upsampleXY2();
		oPlanes[2].upsampleXY2();
	}
	else if ( xdiff == 2 && ydiff == 2 ) {
		oPlanes[1].upsampleXY4();
		oPlanes[2].upsampleXY4();
	}
	else {
		if ( xdiff == -1 ) {
			oPlanes[1].downsampleX2();
			oPlanes[2].downsampleX2();
		}
		else if ( xdiff == -2 ) {
			oPlanes[1].downsampleX4();
			oPlanes[2].downsampleX4();
		}
		else if ( xdiff == 1 ) {
			oPlanes[1].upsampleX2();
			oPlanes[2].upsampleX2();
		}
		else if ( xdiff == 2 ) {
			oPlanes[1].upsampleX4();
			oPlanes[2].upsampleX4();
		}
		if ( iydown == oydown + 1 ) {
			oPlanes[1].downsampleY2();
			oPlanes[2].downsampleY2();
		}
		else if ( iydown == oydown + 2 ) {
			oPlanes[1].downsampleY4();
			oPlanes[2].downsampleY4();
		}
		else if ( iydown + 1 == oydown ) {
			oPlanes[1].upsampleY2();
			oPlanes[2].upsampleY2();
		}
		else if ( iydown + 2 == oydown ) {
			oPlanes[1].upsampleY4();
			oPlanes[2].upsampleY4();
		}
	}

	oPic = Picture(
			picW,
			picH,
			getIFormat(),
			oPlanes );

}

// ============================================================================

static Image::ConverterPtr dummy_444_422 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV444P,	Image::PictureFormat::YUV422P, 0, 0, 1, 0);

static Image::ConverterPtr dummy_444_420 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV444P,	Image::PictureFormat::YUV420P, 0, 0, 1, 1);

static Image::ConverterPtr dummy_444_411 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV444P,	Image::PictureFormat::YUV411P, 0, 0, 2, 0);

static Image::ConverterPtr dummy_444_410 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV444P,	Image::PictureFormat::YUV410P, 0, 0, 2, 2);

static Image::ConverterPtr dummy_422_420 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV422P,	Image::PictureFormat::YUV420P, 1, 0, 1, 1);

static Image::ConverterPtr dummy_422_411 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV422P,	Image::PictureFormat::YUV411P, 1, 0, 2, 0);

static Image::ConverterPtr dummy_422_410 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV422P,	Image::PictureFormat::YUV410P, 1, 0, 2, 2);

static Image::ConverterPtr dummy_420_411 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV420P,	Image::PictureFormat::YUV411P, 1, 1, 2, 0);

static Image::ConverterPtr dummy_420_410 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV420P,	Image::PictureFormat::YUV410P, 1, 1, 2, 2);

static Image::ConverterPtr dummy_411_410 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::YUV411P,	Image::PictureFormat::YUV410P, 2, 0, 2, 2);

// ============================================================================

static Image::ConverterPtr jdummy_444_422 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::JYUV444P,	Image::PictureFormat::JYUV422P, 0, 0, 1, 0);

static Image::ConverterPtr jdummy_444_420 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::JYUV444P,	Image::PictureFormat::JYUV420P, 0, 0, 1, 1);

static Image::ConverterPtr jdummy_422_420 = new Image::Converter_YUVxxxP_YUVxxxP(
	Image::PictureFormat::JYUV422P,	Image::PictureFormat::JYUV420P, 1, 0, 1, 1);

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

