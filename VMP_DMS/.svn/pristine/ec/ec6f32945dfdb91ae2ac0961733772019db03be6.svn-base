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
//	BFC.Image.Converter_YUV444P_UYV444.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV444P_UYV444.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV444P_UYV444::Converter_YUV444P_UYV444()
	: RawConverter(
		PictureFormat::YUV444P,
		PictureFormat::UYV444 ) {



}

Converter_YUV444P_UYV444::~Converter_YUV444P_UYV444() {



}

// ============================================================================

void Converter_YUV444P_UYV444::engineForward(
		ReaderArray	& pIPics,	// YUV444P
		WriterArray	& pOPics ) {	// UYV444

	Mem::UcharReader	r0 = pIPics[ 0 ];	// Y
	Mem::UcharReader	r1 = pIPics[ 1 ];	// U
	Mem::UcharReader	r2 = pIPics[ 2 ];	// V
	Mem::Uint24Writer	w = pOPics[ 0 ];

	while ( w ) {

		*w = ( ( ( Uint32 )*r2 << 16 )
		     | ( ( Uint32 )*r0 <<  8 )
		     | ( ( Uint32 )*r1       ) );

		r0++;
		r1++;
		r2++;
		w++;

	}

}

void Converter_YUV444P_UYV444::engineBackward(
		ReaderArray	& pIPics,	// UYV444
		WriterArray	& pOPics ) {	// YUV444P

	Mem::Uint24Reader	r = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r ) {

		Uint32 v = *r;

		*w2 = ( v >> 16 );
		*w0 = ( v >>  8 );
		*w1 = ( v       );

		r++;
		w0++;
		w1++;
		w2++;

	}

}

// ============================================================================

static ConverterPtr dummy_YUV444P_UYV444 = new Converter_YUV444P_UYV444;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

