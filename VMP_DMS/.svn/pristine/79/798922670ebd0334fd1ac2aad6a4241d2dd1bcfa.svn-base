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
//	BFC.Image.Converter_YUV400_YUV444P.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV400_YUV444P.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV400_YUV444P::Converter_YUV400_YUV444P()
	: RawConverter(
		PictureFormat::YUV400,
		PictureFormat::YUV444P ) {



}

Converter_YUV400_YUV444P::~Converter_YUV400_YUV444P() {



}

// ============================================================================

void Converter_YUV400_YUV444P::engineForward(
		ReaderArray	& pIPics,	// YUV400
		WriterArray	& pOPics ) {	// YUV444P

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r0 ) {

		*w0 = *r0;
		*w1 =
		*w2 = 128;

		r0++;
		w0++;
		w1++;
		w2++;

	}

}

void Converter_YUV400_YUV444P::engineBackward(
		ReaderArray	& pIPics,	// YUV444P
		WriterArray	& pOPics ) {	// YUV400

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];

	while ( r0 ) {

		*w0 = *r0;

		r0++;
		w0++;

	}

}

// ============================================================================

static ConverterPtr dummy_YUV400_YUV444P = new Converter_YUV400_YUV444P;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

