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
//	BFC.Image.Converter_BGR24_RGB555.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_BGR24_RGB555.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_BGR24_RGB555::Converter_BGR24_RGB555()
	: RawConverter(
		PictureFormat::BGR24,
		PictureFormat::RGB555 ) {



}

Converter_BGR24_RGB555::~Converter_BGR24_RGB555() {



}

// ============================================================================

void Converter_BGR24_RGB555::engineForward(
		ReaderArray	& pIPics,	// BGR24
		WriterArray	& pOPics ) {	// RGB555

	Mem::Uint24Reader	r = pIPics[ 0 ];
	Mem::Uint16Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( v >> 9 ) & 0x7C00 )
		   | ( ( v >> 6 ) & 0x03E0 )
		   | ( ( v >> 3 ) & 0x001F );

		r++;
		w++;

	}

}

void Converter_BGR24_RGB555::engineBackward(
		ReaderArray	& pIPics,	// RGB555
		WriterArray	& pOPics ) {	// BGR24

	Mem::Uint16Reader	r = pIPics[ 0 ];
	Mem::Uint24Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( ( v & 0x001F ) << 3 )
		     | ( ( v & 0x03E0 ) << 6 )
		     | ( ( v & 0x7C00 ) << 9 ) );

		r++;
		w++;

	}

}

// ============================================================================

static ConverterPtr dummy_BGR24_RGB555 = new Converter_BGR24_RGB555;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

