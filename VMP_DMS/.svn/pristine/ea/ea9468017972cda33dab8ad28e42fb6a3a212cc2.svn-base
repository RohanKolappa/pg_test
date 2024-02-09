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
//	BFC.Image.Converter_RGB_RGB565.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_RGB565.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_RGB_RGB565::Converter_RGB_RGB565()
	: RawConverter(
		PictureFormat::RGB,
		PictureFormat::RGB565) {



}

Converter_RGB_RGB565::~Converter_RGB_RGB565() {



}

// ============================================================================

void Converter_RGB_RGB565::engineForward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// RGB565

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint16Writer	w = pOPics[ 0 ];

	while ( w ) {

		*w = ( ( ( Uint16 )*r0 << 8 ) & 0xF800 )
		   | ( ( ( Uint16 )*r1 << 3 ) & 0x07E0 )
		   | ( ( ( Uint16 )*r2 >> 3 ) & 0x001F );

		r0++;
		r1++;
		r2++;
		w++;

	}

}

void Converter_RGB_RGB565::engineBackward(
		ReaderArray	& pIPics,	// RGB565
		WriterArray	& pOPics ) {	// RGB

	Mem::Uint16Reader	r = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r ) {

		Uint16 v = *r;

		*w0 = ( ( v & 0xF800 ) >> 8 );
		*w1 = ( ( v & 0x07E0 ) >> 3 );
		*w2 = ( ( v & 0x001F ) << 3 );

		r++;
		w0++;
		w1++;
		w2++;

	}

}

// ============================================================================

static ConverterPtr dummy_RGB_RGB565 = new Converter_RGB_RGB565;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

