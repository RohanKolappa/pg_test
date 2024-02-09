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
//	BFC.Image.Converter_RGBA_RGB24.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGBA_RGB24.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_RGBA_RGB24::Converter_RGBA_RGB24() :

	RawConverter(
		PictureFormat::RGBA32,
		PictureFormat::RGB24 ) {

}

// ============================================================================

void Converter_RGBA_RGB24::engineForward(
		ReaderArray &	pIPics,		// RGBA
		WriterArray &	pOPics ) {	// RGB24

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::Uint24Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( ( v >> 16 ) & 0x0000FF )
		     | ( ( v       ) & 0x00FF00 )
		     | ( ( v << 16 ) & 0xFF0000 ) );

		r++;
		w++;

	}

}

void Converter_RGBA_RGB24::engineBackward(
		ReaderArray &	pIPics,		// RGB24
		WriterArray &	pOPics ) {	// RGBA

	Mem::Uint24Reader	r = pIPics[ 0 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = 0xFF000000
		   | ( ( v >> 16 ) & 0x0000FF )
		   | ( ( v       ) & 0x00FF00 )
		   | ( ( v << 16 ) & 0xFF0000 );

		r++;
		w++;

	}

}

// ============================================================================

static ConverterPtr dummy_RGBA_RGB24 = new Converter_RGBA_RGB24;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

