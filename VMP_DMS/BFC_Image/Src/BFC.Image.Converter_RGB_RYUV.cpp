// ============================================================================

//#if defined( BFC_Image_Converter_COMPILE )

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
//	BFC.Image.Converter_RGB_RYUV.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_RYUV.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Image::Converter_RGB_RYUV444P
// ============================================================================

Image::Converter_RGB_RYUV444P::Converter_RGB_RYUV444P() :

	RawRYUVConverter(
		PictureFormat::RGB,
		PictureFormat::RYUV444P ) {

}

// ============================================================================

void Image::Converter_RGB_RYUV444P::engineForward(
		ReaderArray &	pIPics,		// RGB
		WriterArray &	pOPics ) {	// RYUV444P

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::Uint16Writer	w0 = pOPics[ 0 ];
	Mem::Uint16Writer	w1 = pOPics[ 1 ];
	Mem::Uint16Writer	w2 = pOPics[ 2 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uint16 *	oPtr0 = ( Uint16 * )w0.getRowAddress( y );
		Uint16 *	oPtr1 = ( Uint16 * )w1.getRowAddress( y );
		Uint16 *	oPtr2 = ( Uint16 * )w2.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			Uint16	Y;
			Uint16	U;
			Uint16	V;
			rgb2ryuv( *iPtr0++, *iPtr1++, *iPtr2++, Y, U, V );
			*oPtr0++ = Y;
			*oPtr1++ = U;
			*oPtr2++ = V;
		}
	}

}

void Image::Converter_RGB_RYUV444P::engineBackward(
		ReaderArray &	pIPics,		// RYUV444P
		WriterArray &	pOPics ) {	// RGB

	Mem::Uint16Reader	r0 = pIPics[ 0 ];
	Mem::Uint16Reader	r1 = pIPics[ 1 ];
	Mem::Uint16Reader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uint16 *	iPtr0 = ( const Uint16 * )r0.getRowAddress( y );
		const Uint16 *	iPtr1 = ( const Uint16 * )r1.getRowAddress( y );
		const Uint16 *	iPtr2 = ( const Uint16 * )r2.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			Uchar	R;
			Uchar	G;
			Uchar	B;
			ryuv2rgb( *iPtr0++, *iPtr1++, *iPtr2++, R, G, B );
			*oPtr0++ = R;
			*oPtr1++ = G;
			*oPtr2++ = B;
		}
	}

}

// ============================================================================

static Image::ConverterPtr dummy_RGB_RYUV = new Image::Converter_RGB_RYUV444P;

// ============================================================================
// BFC::Image::Converter_RGBA_RYUVA444P
// ============================================================================

Image::Converter_RGBA_RYUVA444P::Converter_RGBA_RYUVA444P() :

	RawRYUVConverter(
		PictureFormat::RGBA,
		PictureFormat::RYUVA444P ) {

}

// ============================================================================

void Image::Converter_RGBA_RYUVA444P::engineForward(
		ReaderArray &	pIPics,		// RGBA
		WriterArray &	pOPics ) {	// RYUVA444P

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharReader	r3 = pIPics[ 3 ];
	Mem::Uint16Writer	w0 = pOPics[ 0 ];
	Mem::Uint16Writer	w1 = pOPics[ 1 ];
	Mem::Uint16Writer	w2 = pOPics[ 2 ];
	Mem::Uint16Writer	w3 = pOPics[ 3 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		const Uchar *	iPtr3 = ( const Uchar * )r3.getRowAddress( y );
		Uint16 *	oPtr0 = ( Uint16 * )w0.getRowAddress( y );
		Uint16 *	oPtr1 = ( Uint16 * )w1.getRowAddress( y );
		Uint16 *	oPtr2 = ( Uint16 * )w2.getRowAddress( y );
		Uint16 *	oPtr3 = ( Uint16 * )w3.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			Uint16	Y;
			Uint16	U;
			Uint16	V;
			rgb2ryuv( *iPtr0++, *iPtr1++, *iPtr2++, Y, U, V );
			*oPtr0++ = Y;
			*oPtr1++ = U;
			*oPtr2++ = V;
			*oPtr3++ = *iPtr3++;
		}
	}

}

void Image::Converter_RGBA_RYUVA444P::engineBackward(
		ReaderArray &	pIPics,		// RYUVA444P
		WriterArray &	pOPics ) {	// RGBA

	Mem::Uint16Reader	r0 = pIPics[ 0 ];
	Mem::Uint16Reader	r1 = pIPics[ 1 ];
	Mem::Uint16Reader	r2 = pIPics[ 2 ];
	Mem::Uint16Reader	r3 = pIPics[ 3 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];
	Mem::UcharWriter	w3 = pOPics[ 3 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uint16 *	iPtr0 = ( const Uint16 * )r0.getRowAddress( y );
		const Uint16 *	iPtr1 = ( const Uint16 * )r1.getRowAddress( y );
		const Uint16 *	iPtr2 = ( const Uint16 * )r2.getRowAddress( y );
		const Uint16 *	iPtr3 = ( const Uint16 * )r3.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		Uchar *		oPtr3 = ( Uchar * )w3.getRowAddress( y );
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			Uchar	R;
			Uchar	G;
			Uchar	B;
			ryuv2rgb( *iPtr0++, *iPtr1++, *iPtr2++, R, G, B );
			*oPtr0++ = R;
			*oPtr1++ = G;
			*oPtr2++ = B;
			*oPtr3++ = (Uchar)*iPtr3++;
		}
	}

}

// ============================================================================

static Image::ConverterPtr dummy_RGBA_RYUVA = new Image::Converter_RGBA_RYUVA444P;

// ============================================================================

//#endif // BFC_Image_Converter_COMPILE

// ============================================================================

