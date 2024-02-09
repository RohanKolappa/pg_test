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
//	BFC.Image.Converter_RGB_RGBA.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_RGBA.h"

// ============================================================================

using namespace BFC;
using namespace BFC;

// ============================================================================

Image::Converter_RGB_RGBA::Converter_RGB_RGBA() :

	RawConverter(
		PictureFormat::RGB,
		PictureFormat::RGBA32 ) {

}

// ============================================================================

void Image::Converter_RGB_RGBA::engineForward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// RGBA

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

#if 1

	Uint32		cols = w.getNbrCols();
	Uint32		rows = w.getNbrRows();

	for ( Uint32 y = 0 ; y < rows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uint32 *	oPtr = ( Uint32 * )w.getRowAddress( y );
		for ( Uint32 x = cols ; x ; x-- ) {
			*oPtr++	= 0xFF000000
				| ( ( Uint32 )*iPtr0++ << 16 )
				| ( ( Uint32 )*iPtr1++ <<  8 )
				| ( ( Uint32 )*iPtr2++       );
		}
	}

#else

	while ( w ) {

		*w = 0xFF000000
		   | ( ( Uint32 )*r0 << 16 )
		   | ( ( Uint32 )*r1 <<  8 )
		   | ( ( Uint32 )*r2       );

		r0++;
		r1++;
		r2++;
		w++;

	}

#endif

}

void Image::Converter_RGB_RGBA::engineBackward(
		ReaderArray &	pIPics,		// RGBA
		WriterArray &	pOPics ) {	// RGB

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

#if 1

	Uint32		cols = r.getNbrCols();
	Uint32		rows = r.getNbrRows();

	for ( Uint32 y = 0 ; y < rows ; y++ ) {
		const Uint32 *	iPtr = ( const Uint32 * )r.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		for ( Uint32 x = cols ; x ; x-- ) {
			Uint32 v = *iPtr++;
			*oPtr0++ = ( v >> 16 );
			*oPtr1++ = ( v >>  8 );
			*oPtr2++ = ( v       );
		}
	}

#else

	while ( r ) {

		Uint32 v = *r;

		*w0 = ( v >> 16 );
		*w1 = ( v >>  8 );
		*w2 = ( v       );

		r++;
		w0++;
		w1++;
		w2++;

	}

#endif

}

// ============================================================================

static ConverterPtr dummy_RGB_RGBA32 = new Converter_RGB_RGBA;

// ============================================================================

Image::Converter_RGB_BGRA::Converter_RGB_BGRA() :

	RawConverter(
		PictureFormat::RGB,
		PictureFormat::BGRA32 ) {

}

// ============================================================================

void Image::Converter_RGB_BGRA::engineForward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// RGBA

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint32Writer	w = pOPics[ 0 ];

	Uint32		cols = w.getNbrCols();
	Uint32		rows = w.getNbrRows();

	for ( Uint32 y = 0 ; y < rows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uint32 *	oPtr = ( Uint32 * )w.getRowAddress( y );
		for ( Uint32 x = cols ; x ; x-- ) {
			*oPtr++	= 0xFF000000
				| ( ( Uint32 )*iPtr2++ << 16 )
				| ( ( Uint32 )*iPtr1++ <<  8 )
				| ( ( Uint32 )*iPtr0++       );
		}
	}

}

void Image::Converter_RGB_BGRA::engineBackward(
		ReaderArray &	pIPics,		// RGBA
		WriterArray &	pOPics ) {	// RGB

	Mem::Uint32Reader	r = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	Uint32		cols = r.getNbrCols();
	Uint32		rows = r.getNbrRows();

	for ( Uint32 y = 0 ; y < rows ; y++ ) {
		const Uint32 *	iPtr = ( const Uint32 * )r.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		for ( Uint32 x = cols ; x ; x-- ) {
			Uint32 v = *iPtr++;
			*oPtr2++ = ( v >> 16 );
			*oPtr1++ = ( v >>  8 );
			*oPtr0++ = ( v       );
		}
	}

}

// ============================================================================

static ConverterPtr dummy_RGB_BGRA32 = new Converter_RGB_BGRA;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

