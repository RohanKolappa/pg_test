// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Image.Converter_Bayer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_Bayer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Converter_RGB_GBRG::Converter_RGB_GBRG() :

	Converter(
		PictureFormat::RGB,
		PictureFormat::SGBRG8 ) {

}

// ============================================================================

Image::Picture Image::Converter_RGB_GBRG::convertForward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::Converter_RGB_GBRG::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	if ( oPic.getWidth() != iPic.getWidth()
	  || oPic.getHeight() != iPic.getHeight()
	  || oPic.getFormat() != getOFormat() ) {

		oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getOFormat(),
			false );

	}

	doBayerFwd(
		iPic.getPlane( 0 ),
		iPic.getPlane( 1 ),
		iPic.getPlane( 2 ),
		oPic.getPlane( 0 ) );

}

Image::Picture Image::Converter_RGB_GBRG::convertBackward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::Converter_RGB_GBRG::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	if ( oPic.getWidth() != iPic.getWidth()
	  || oPic.getHeight() != iPic.getHeight()
	  || oPic.getFormat() != getIFormat() ) {

		oPic = Picture(
			iPic.getWidth(),
			iPic.getHeight(),
			getIFormat(),
			false );

	}

	doBayerBck(
		iPic.getPlane( 0 ),
		oPic.getPlane( 0 ),
		oPic.getPlane( 1 ),
		oPic.getPlane( 2 ) );

}

// ============================================================================

void Image::Converter_RGB_GBRG::doBayerFwd(
	const	Plane &		iPic0,
	const	Plane &		iPic1,
	const	Plane &		iPic2,
		Plane &		oPic0 ) {

	Uint32	cols = oPic0.getWidth();
	Uint32	rows = oPic0.getHeight();

	Mem::Reader	r0 = iPic0.getDataReader();
	Mem::Reader	r1 = iPic1.getDataReader();
	Mem::Reader	r2 = iPic2.getDataReader();
	Mem::Writer	w0 = oPic0.getDataWriter();

	for ( Uint32 y = 0 ; y < rows ; y += 2 ) {
		const Uchar *	prG = ( const Uchar * )r1.getRowAddress( y );
		const Uchar *	prB = ( const Uchar * )r2.getRowAddress( y );
		Uchar *		pw = ( Uchar * )w0.getRowAddress( y );
		for ( Uint32 x = 0 ; x < cols ; x += 2 ) {
			*pw++ = *prG++;
			*pw++ = *prB++;
		}
	}

	for ( Uint32 y = 1 ; y < rows ; y += 2 ) {
		const Uchar *	prR = ( const Uchar * )r0.getRowAddress( y );
		const Uchar *	prG = ( const Uchar * )r1.getRowAddress( y );
		Uchar *		pw = ( Uchar * )w0.getRowAddress( y );
		for ( Uint32 x = 0 ; x < cols ; x += 2 ) {
			*pw++ = *prR++;
			*pw++ = *prG++;
		}
	}

}

void Image::Converter_RGB_GBRG::doBayerBck(
	const	Plane &		iPic0,
		Plane &		oPic0,
		Plane &		oPic1,
		Plane &		oPic2 ) {

	Uint32	cols = iPic0.getWidth();
	Uint32	rows = iPic0.getHeight();

	if ( ( cols % 2 ) || ( rows % 2 ) ) {
		throw InternalError( "Converter_RGB_GBRG::doBayerBck(): not divisible by 2" );
	}

	Mem::Reader	r0 = iPic0.getDataReader();
	Mem::Writer	w0 = oPic0.getDataWriter();
	Mem::Writer	w1 = oPic1.getDataWriter();
	Mem::Writer	w2 = oPic2.getDataWriter();

	for ( Uint32 y = 0 ; y < rows ; ) {
		const Uchar *	pr00 = ( const Uchar * )r0.getRowAddress( y );
		Uchar *		pw00 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		pw10 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		pw20 = ( Uchar * )w2.getRowAddress( y );
		y++;
		const Uchar *	pr01 = ( const Uchar * )r0.getRowAddress( y );
		Uchar *		pw01 = ( Uchar * )w0.getRowAddress( y );
		Uchar *		pw11 = ( Uchar * )w1.getRowAddress( y );
		Uchar *		pw21 = ( Uchar * )w2.getRowAddress( y );
		y++;
		for ( Uint32 x = 0 ; x < cols ; x += 2 ) {
			Uchar G0 = *pr00++;
			Uchar B0 = *pr00++;
			Uchar R1 = *pr01++;
			Uchar G1 = *pr01++;
			*pw00++ = R1; *pw00++ = R1; *pw01++ = R1; *pw01++ = R1;
			*pw10++ = G0; *pw10++ = G0; *pw11++ = G1; *pw11++ = G1;
			*pw20++ = B0; *pw20++ = B0; *pw21++ = B0; *pw21++ = B0;
		}
	}

}

// ============================================================================

static Image::ConverterPtr dummy_RGB_GBRG = new Image::Converter_RGB_GBRG;

// ============================================================================

