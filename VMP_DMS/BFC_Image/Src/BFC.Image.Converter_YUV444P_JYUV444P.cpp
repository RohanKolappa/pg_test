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
//	BFC.Image.Converter_YUV444P_JYUV444P.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV444P_JYUV444P.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV444P_JYUV444P::Converter_YUV444P_JYUV444P()
	: RawConverter(
		PictureFormat::YUV444P,
		PictureFormat::JYUV444P) {



}

Converter_YUV444P_JYUV444P::~Converter_YUV444P_JYUV444P() {



}

// ============================================================================

void Converter_YUV444P_JYUV444P::engineForward(
		ReaderArray	& pIPics,	// YUV
		WriterArray	& pOPics ) {	// JYUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 Y1 = ( Int32 )( Uint32 )*r0;
		Int32 U = ( Int32 )( Uint32 )*r1;
		Int32 V = ( Int32 )( Uint32 )*r2;

		Int32 Y2 = ( 298 * Y1 - 4640 ) >> 8;
		Int32 Cb = ( 291 * U - 4352 ) >> 8;
		Int32 Cr = ( 291 * V - 4352 ) >> 8;

		if ( Y2 < 0 ) Y2 = 0; else if ( Y2 > 255 ) Y2 = 255;
		if ( Cb < 0 ) Cb = 0; else if ( Cb > 255 ) Cb = 255;
		if ( Cr < 0 ) Cr = 0; else if ( Cr > 255 ) Cr = 255;

		*w0 = ( Uchar )( Uint32 )Y2;
		*w1 = ( Uchar )( Uint32 )Cb;
		*w2 = ( Uchar )( Uint32 )Cr;

		r0++;
		r1++;
		r2++;
		w0++;
		w1++;
		w2++;

	}

}

void Converter_YUV444P_JYUV444P::engineBackward(
		ReaderArray	& pIPics,	// JYUV
		WriterArray	& pOPics ) {	// YUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 Y1 = ( Int32 )( Uint32 )*r0;
		Int32 Cb = ( Int32 )( Uint32 )*r1;
		Int32 Cr = ( Int32 )( Uint32 )*r2;

		Int32 Y2 = ( 220 * Y1 + 4224 ) >> 8;
		Int32 U = ( 225 * Cb + 4096 ) >> 8;
		Int32 V = ( 225 * Cr + 4096 ) >> 8;

		*w0 = ( Uchar )( Uint32 )Y2;
		*w1 = ( Uchar )( Uint32 )U;
		*w2 = ( Uchar )( Uint32 )V;

		r0++;
		r1++;
		r2++;
		w0++;
		w1++;
		w2++;

	}

}

// ============================================================================

static ConverterPtr dummy_YUV444P_JYUV444P = new Converter_YUV444P_JYUV444P;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

