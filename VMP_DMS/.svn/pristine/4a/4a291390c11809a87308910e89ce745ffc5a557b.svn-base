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
//	BFC.Image.Converter_YUV400_RGB.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV400_RGB.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV400_RGB::Converter_YUV400_RGB()
	: RawConverter(
		PictureFormat::YUV400 ,
		PictureFormat::RGB ) {



}

Converter_YUV400_RGB::~Converter_YUV400_RGB() {



}

// ============================================================================

void Converter_YUV400_RGB::engineForward(
		ReaderArray	& pIPics,	// YUV400
		WriterArray	& pOPics ) {	// RGB

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 Y = ( Int32 )( Uint32 )*r0;

//		if ( Y < 16 ) Y = 16; else if ( Y > 235 ) Y = 235;

		Int32 R = ( 298 * ( Y - 16 ) + 128 ) >> 8;

		if ( R < 0 ) R = 0; else if ( R > 255 ) R = 255;

		*w0 =
		*w1 =
		*w2 = ( Uchar )( Uint32 )R;

		r0++;
		w0++;
		w1++;
		w2++;

	}

}

void Converter_YUV400_RGB::engineBackward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// YUV400

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];

	while ( r0 ) {

		Int32 R = ( Int32 )( Uint32 )*r0;
		Int32 G = ( Int32 )( Uint32 )*r1;
		Int32 B = ( Int32 )( Uint32 )*r2;

		Int32 Y = ( (  66 * R + 129 * G +  25 * B + 128 ) >> 8 ) + 16;

		*w0 = ( Uchar )( Uint32 )Y;

		r0++;
		r1++;
		r2++;
		w0++;

	}

}

// ============================================================================

static ConverterPtr dummy_YUV400_RGB = new Converter_YUV400_RGB;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

