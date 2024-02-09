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
//	BFC.Image.Converter_YUV420P_JYUV400.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV420P_JYUV400.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV420P_JYUV400::Converter_YUV420P_JYUV400()
	: RawConverter(
		PictureFormat::YUV420P,
		PictureFormat::JYUV400) {



}

Converter_YUV420P_JYUV400::~Converter_YUV420P_JYUV400() {



}

// ============================================================================

void Converter_YUV420P_JYUV400::engineForward(
		ReaderArray	& pIPics,	// YUV
		WriterArray	& pOPics ) {	// JYUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];

	while ( r0 ) {

		Int32 Y1 = ( Int32 )( Uint32 )*r0;
		Int32 Y2 = ( 298 * Y1 - 4640 ) >> 8;
		if ( Y2 < 0 ) Y2 = 0; else if ( Y2 > 255 ) Y2 = 255;

		*w0 = ( Uchar )( Uint32 )Y2;

		r0++;
		w0++;
	}

}

void Converter_YUV420P_JYUV400::engineBackward(
		ReaderArray	& pIPics,	// JYUV
		WriterArray	& pOPics ) {	// YUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 Y1 = ( Int32 )( Uint32 )*r0;

		Int32 Y2 = ( 220 * Y1 + 4224 ) >> 8;

		*w0 = ( Uchar )( Uint32 )Y2;

		r0++;
		w0++;
	}

	while(w1) {
		*w1=128;
		w1++;
	}

	while(w2) {
		*w2=128;
		w2++;
	}
}

// ============================================================================

static ConverterPtr dummy_YUV420P_JYUV400 = new Converter_YUV420P_JYUV400;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

