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
//	BFC.Image.Converter_YUV422P_YUV422.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YUV422P_YUV422.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YUV422P_YUV422::Converter_YUV422P_YUV422()
	: RawConverter(
		PictureFormat::YUV422P,
		PictureFormat::YUV422) {



}

Converter_YUV422P_YUV422::~Converter_YUV422P_YUV422() {



}

// ============================================================================

void Converter_YUV422P_YUV422::engineForward(
		ReaderArray	& pIPics,	// YUV422P
		WriterArray	& pOPics ) {	// YUV422

	// planar YUV ---> packed UYVY

	Mem::Uint16Reader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

	while ( w ) {

		Uint32 y = *r0;
		Uint32 u = *r1;
		Uint32 v = *r2;

		*w = ( ( u          )       )
		   | ( ( y & 0x00FF ) <<  8 )
		   | ( ( v          ) << 16 )
		   | ( ( y & 0xFF00 ) << 16 );

		r0++;
		r1++;
		r2++;
		w++;

	}

}

void Converter_YUV422P_YUV422::engineBackward(
		ReaderArray	& pIPics,	// YUV422
		WriterArray	& pOPics ) {	// YUV422P

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::Uint16Writer	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	while ( r ) {

		Uint32 uyvy = *r;

		*w0 = ( ( uyvy >>  8 ) & 0x00FF )
		    | ( ( uyvy >> 16 ) & 0xFF00 );
		*w1 = ( ( uyvy       )          );
		*w2 = ( ( uyvy >> 16 )          );

		r++;
		w0++;
		w1++;
		w2++;

	}

}

// ============================================================================

static ConverterPtr dummy_YUV422P_YUV422 = new Converter_YUV422P_YUV422;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

