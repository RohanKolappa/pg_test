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
//	BFC.Image.Converter_RGB_BGR24.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_BGR24.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Image;

// ============================================================================

Converter_RGB_BGR24::Converter_RGB_BGR24()
	: RawConverter(
		PictureFormat::RGB,
		PictureFormat::BGR24) {



}

Converter_RGB_BGR24::~Converter_RGB_BGR24() {



}

// ============================================================================

void Converter_RGB_BGR24::engineForward(
		ReaderArray &	pIPics,	// RGB
		WriterArray &	pOPics ) {	// BGR24

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint24Writer	w = pOPics[ 0 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uchar *		oPtr = ( Uchar * )w.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			*oPtr++ = *iPtr2++;
			*oPtr++ = *iPtr1++;
			*oPtr++ = *iPtr0++;
		}
	}

}

void Converter_RGB_BGR24::engineBackward(
		ReaderArray &	pIPics,	// BGR24
		WriterArray &	pOPics ) {	// RGB

	Mem::Uint24Reader	r = pIPics[ 0 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	Uint32	nbrCols = w0.getNbrCols();
	Uint32	nbrRows = w0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr = ( const Uchar * )r.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			*oPtr2++ = *iPtr++;
			*oPtr1++ = *iPtr++;
			*oPtr0++ = *iPtr++;
		}
	}

}

// ============================================================================

static ConverterPtr dummy_RGB_BGR24 = new Converter_RGB_BGR24;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

