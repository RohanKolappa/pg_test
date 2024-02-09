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
//	BFC.Image.Converter_RGB_JYUV444P.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_JYUV444P.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_RGB_JYUV444P::Converter_RGB_JYUV444P()
	: RawConverter(
		PictureFormat::RGB,
		PictureFormat::JYUV444P) {



}

// ============================================================================

void Converter_RGB_JYUV444P::engineForward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// JYUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 R = ( Int32 )( Uint32 )*r0;
		Int32 G = ( Int32 )( Uint32 )*r1;
		Int32 B = ( Int32 )( Uint32 )*r2;
/*
		Int32 Y = ( (  77 * R + 150 * G +  29 * B + 127 ) >> 8 );
		Int32 U = ( ( -43 * R -  85 * G + 128 * B + 127 ) >> 8 ) + 128;
		Int32 V = ( ( 128 * R - 107 * G -  21 * B + 127 ) >> 8 ) + 128;
*/
		Int32 Y = ( (  19595 * R + 38470 * G +  7471 * B + 32767 ) >> 16 );
		Int32 Cb = ( ( -11052 * R -  21697 * G + 32749 * B + 8421375 ) >> 16 );
		Int32 Cr = ( ( 32756 * R - 27429 * G -  5327 * B + 8421375 ) >> 16 );

		*w0 = ( Uchar )( Uint32 )Y;
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

void Converter_RGB_JYUV444P::engineBackward(
		ReaderArray	& pIPics,	// JYUV
		WriterArray	& pOPics ) {	// RGB

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		Int32 Y = ( Int32 )( Uint32 )*r0;
		Int32 U = ( Int32 )( Uint32 )*r1;
		Int32 V = ( Int32 )( Uint32 )*r2;
/*
		U -= 128;
		V -= 128;

		Int32 R = Y + ((           + 359 * V + 128 ) >> 8);
		Int32 G = Y + (( -  88 * U - 183 * V + 128 ) >> 8);
		Int32 B = Y + (( + 454 * U           + 128 ) >> 8);
*/

		Int32 R = Y + ((           + 91881 * V - 11728000 ) >> 16);
		Int32 G = Y + (( - 22554 * U - 46802 * V + 8910336 ) >> 16);
		Int32 B = Y + (( 116130 * U           - 14831872) >> 16);

		if ( R < 0 ) R = 0; else if ( R > 255 ) R = 255;
		if ( G < 0 ) G = 0; else if ( G > 255 ) G = 255;
		if ( B < 0 ) B = 0; else if ( B > 255 ) B = 255;

		*w0 = ( Uchar )( Uint32 )R;
		*w1 = ( Uchar )( Uint32 )G;
		*w2 = ( Uchar )( Uint32 )B;

		r0++;
		r1++;
		r2++;
		w0++;
		w1++;
		w2++;

	}

}

// ============================================================================

static ConverterPtr dummy_RGB_JYUV444P = new Converter_RGB_JYUV444P;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

