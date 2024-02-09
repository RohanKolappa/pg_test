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
//	BFC.Image.Converter_RGB24_RGB565.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB24_RGB565.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_RGB24_RGB565::Converter_RGB24_RGB565()
	: RawConverter(
		PictureFormat::RGB24,
		PictureFormat::RGB565) {



}

Converter_RGB24_RGB565::~Converter_RGB24_RGB565() {



}

// ============================================================================

void Converter_RGB24_RGB565::engineForward(
		ReaderArray	& pIPics,	// RGB24
		WriterArray	& pOPics ) {	// RGB565

	Mem::Uint24Reader	r = pIPics[ 0 ];
	Mem::Uint16Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( v <<  8 ) & 0xF800 )
		   | ( ( v >>  5 ) & 0x07E0 )
		   | ( ( v >> 19 ) & 0x001F );

		r++;
		w++;

	}

}

void Converter_RGB24_RGB565::engineBackward(
		ReaderArray	& pIPics,	// RGB565
		WriterArray	& pOPics ) {	// RGB24

	Mem::Uint16Reader	r = pIPics[ 0 ];
	Mem::Uint24Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( ( v & 0x001F ) << 19 )
		     | ( ( v & 0x07E0 ) <<  5 )
		     | ( ( v & 0xF800 ) >>  8 ) );

		r++;
		w++;

	}

}

// ============================================================================

static ConverterPtr dummy_RGB24_RGB565 = new Converter_RGB24_RGB565;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

