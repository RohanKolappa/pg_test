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
//	BFC.Image.Converter_GRAY12_GRAY16.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_GRAY12_GRAY16.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_GRAY12_GRAY16::Converter_GRAY12_GRAY16()
	: RawConverter(
		PictureFormat::GRAY12,
		PictureFormat::GRAY16) {



}

Converter_GRAY12_GRAY16::~Converter_GRAY12_GRAY16() {



}

// ============================================================================

void Converter_GRAY12_GRAY16::engineForward(
		ReaderArray	& pIPics,	// GRAY12
		WriterArray	& pOPics ) {	// GRAY16

	Mem::Uint16Reader	r = pIPics[ 0 ];
	Mem::Uint16Writer	w = pOPics[ 0 ];

	while ( r ) {

		*w = ( *r << 4 );

		r++;
		w++;

	}

}

void Converter_GRAY12_GRAY16::engineBackward(
		ReaderArray	& pIPics,	// GRAY16
		WriterArray	& pOPics ) {	// GRAY12

	Mem::Uint16Reader	r = pIPics[ 0 ];
	Mem::Uint16Writer	w = pOPics[ 0 ];

	while ( r ) {

		*w = ( *r >> 4 );

		r++;
		w++;

	}

}

// ============================================================================

static ConverterPtr dummy_GRAY12_GRAY16 = new Converter_GRAY12_GRAY16;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

