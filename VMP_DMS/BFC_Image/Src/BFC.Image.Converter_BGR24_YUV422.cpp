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
//	BFC.Image.Converter_BGR24_YUV422.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Christophe Herreman
// 
// ============================================================================

#include "BFC.Image.Converter_BGR24_YUV422.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter_BGR24_YUV422::Converter_BGR24_YUV422() :

	RawConverter(
		PictureFormat::BGR24,
		PictureFormat::YUV422 ) {

}

// ============================================================================

void Image::Converter_BGR24_YUV422::engineForward(
		ReaderArray &	pIPics,		// BGR24
		WriterArray &	pOPics ) {	// YUV422

	Mem::UcharReader	r = pIPics[ 0 ];
	Mem::UcharWriter	w = pOPics[ 0 ];

	Uint32			bufWidth = r.getNbrCols() / 3;
	Uint32			bufHeight = r.getNbrRows();

	const Uchar *		iPtr;
	Uchar *			oPtr;
	
	Int32			R, G, B;
	Int32			Y1, Y2, U1, V1, U2, V2;

	for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
		iPtr = ( const Uchar * )r.getRowAddress( y );
		oPtr = ( Uchar * )w.getRowAddress( y );
		for ( Uint32 x = bufWidth / 2 ; x ; x-- ) {
			// Read the RGB values...
			B = ( Int32 )( Uint32 )*iPtr++;
			G = ( Int32 )( Uint32 )*iPtr++;
			R = ( Int32 )( Uint32 )*iPtr++;
			// ... then compute the YUV values...
			transformRGB2YUV( R, G, B, Y1, U1, V1 );
			// Read second RGB triple...
			B = ( Int32 )( Uint32 )*iPtr++;
			G = ( Int32 )( Uint32 )*iPtr++;
			R = ( Int32 )( Uint32 )*iPtr++;
			// ... then compute the second YUV triple...
			transformRGB2YUV( R, G, B, Y2, U2, V2 );
			// Write results!
			*oPtr++ = ( Uchar )( ( U1 + U2 + 1 ) >> 1 ); // downsample
			*oPtr++ = ( Uchar )( ( Y1          )      );
			*oPtr++ = ( Uchar )( ( V1 + V2 + 1 ) >> 1 );
			*oPtr++ = ( Uchar )( ( Y2          )      );
		}
	}

}

void Image::Converter_BGR24_YUV422::engineBackward(
		ReaderArray &	pIPics,		// YUV422
		WriterArray &	pOPics ) {	// BGR24

	Mem::UcharReader	r = pIPics[ 0 ];
	Mem::UcharWriter	w = pOPics[ 0 ];

	Uint32			bufWidth = r.getNbrCols() / 2;
	Uint32			bufHeight = r.getNbrRows();

	const Uchar *		iPtr;
	Uchar *			oPtr;

	Int32			R, G, B;
	Int32			Y1, U, V, Y2;

	for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
		iPtr = ( const Uchar * )r.getRowAddress( y );
		oPtr = ( Uchar * )w.getRowAddress( y );
		for ( Uint32 x = bufWidth / 2 ; x ; x-- ) {
			U  = ( Int32 )( Uint32 )*iPtr++;
			Y1 = ( Int32 )( Uint32 )*iPtr++;
			V  = ( Int32 )( Uint32 )*iPtr++;
			Y2 = ( Int32 )( Uint32 )*iPtr++;
			transformYUV2RGB( Y1, U, V, R, G, B );
			*oPtr++ = ( Uchar )B;
			*oPtr++ = ( Uchar )G;
			*oPtr++ = ( Uchar )R;
			transformYUV2RGB( Y2, U, V, R, G, B );
			*oPtr++ = ( Uchar )B;
			*oPtr++ = ( Uchar )G;
			*oPtr++ = ( Uchar )R;
		}
	}

}

// ============================================================================

static Image::ConverterPtr dummy_BGR24_YUV422 = new Image::Converter_BGR24_YUV422;

// ============================================================================

