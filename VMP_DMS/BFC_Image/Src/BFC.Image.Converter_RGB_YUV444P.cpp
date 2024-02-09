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
//	BFC.Image.Converter_RGB_YUV444P.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_YUV444P.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter_RGB_YUV444P::Converter_RGB_YUV444P() :

	RawConverter(
		PictureFormat::RGB,
		PictureFormat::YUV444P ) {

}

Image::Converter_RGB_YUV444P::~Converter_RGB_YUV444P() {

}

// ============================================================================

void Image::Converter_RGB_YUV444P::engineForward(
		ReaderArray	& pIPics,	// RGB
		WriterArray	& pOPics ) {	// YUV

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		Int32		Y;
		Int32		U;
		Int32		V;
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			transformRGB2YUV(
				( Int32 )( Uint32 )*iPtr0++,
				( Int32 )( Uint32 )*iPtr1++,
				( Int32 )( Uint32 )*iPtr2++,
				Y, U, V );
			*oPtr0++ = ( Uchar )( Uint32 )Y;
			*oPtr1++ = ( Uchar )( Uint32 )U;
			*oPtr2++ = ( Uchar )( Uint32 )V;
		}
	}

}

void Image::Converter_RGB_YUV444P::engineBackward(
		ReaderArray &	pIPics,		// YUV
		WriterArray &	pOPics ) {	// RGB

	Mem::UcharReader	r0 = pIPics[ 0 ];
	Mem::UcharReader	r1 = pIPics[ 1 ];
	Mem::UcharReader	r2 = pIPics[ 2 ];
	Mem::UcharWriter	w0 = pOPics[ 0 ];
	Mem::UcharWriter	w1 = pOPics[ 1 ];
	Mem::UcharWriter	w2 = pOPics[ 2 ];

	Uint32	nbrCols = r0.getNbrCols();
	Uint32	nbrRows = r0.getNbrRows();

	for ( Uint32 y = 0 ; y < nbrRows ; y++ ) {
		const Uchar *	iPtr0 = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	iPtr1 = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	iPtr2 = ( const Uchar * )r2.getRowAddress( y );
		Uchar *		oPtr0 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		oPtr1 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		oPtr2 = ( Uchar * )w2.getRowAddress( y );
		Int32		R;
		Int32		G;
		Int32		B;
		for ( Uint32 x = nbrCols ; x ; x-- ) {
			transformYUV2RGB(
				( Int32 )( Uint32 )*iPtr0++,
				( Int32 )( Uint32 )*iPtr1++,
				( Int32 )( Uint32 )*iPtr2++,
				R, G, B );
			*oPtr0++ = ( Uchar )( Uint32 )R;
			*oPtr1++ = ( Uchar )( Uint32 )G;
			*oPtr2++ = ( Uchar )( Uint32 )B;
		}
	}

}

// ============================================================================

static Image::ConverterPtr dummy_RGB_YUV444P = new Image::Converter_RGB_YUV444P;

// ============================================================================

#endif // BFC_Image_Converter_COMPILE

// ============================================================================

