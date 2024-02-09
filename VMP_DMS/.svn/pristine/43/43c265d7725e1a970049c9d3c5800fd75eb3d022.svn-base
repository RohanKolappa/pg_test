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
//	BFC.Image.Converter_YVYU_YUYV.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_YVYU_YUYV.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_YVYU_YUYV::Converter_YVYU_YUYV()
	: RawConverter(
		PictureFormat::YVYU,
		PictureFormat::YUYV ) {



}

Converter_YVYU_YUYV::~Converter_YVYU_YUYV() {



}

// ============================================================================

void Converter_YVYU_YUYV::engineForward(
		ReaderArray	& pIPics,	// YVYU
		WriterArray	& pOPics ) {	// YUYV

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( v       ) & 0x00FF00FF )
		   | ( ( v >> 16 ) & 0x0000FF00 )
		   | ( ( v << 16 ) & 0xFF000000 );

		r++;
		w++;

	}

}

void Converter_YVYU_YUYV::engineBackward(
		ReaderArray	& pIPics,	// YUYV
		WriterArray	& pOPics ) {	// YVYU

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

	while ( r ) {

		Uint32 v = *r;

		*w = ( ( v       ) & 0x00FF00FF )
		   | ( ( v >> 16 ) & 0x0000FF00 )
		   | ( ( v << 16 ) & 0xFF000000 );

		r++;
		w++;

	}

}

// ============================================================================

static ConverterPtr dummy_YVYU_YUYV = new Converter_YVYU_YUYV;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

