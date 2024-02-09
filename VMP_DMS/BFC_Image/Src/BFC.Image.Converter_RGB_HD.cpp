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
//	BFC.Image.Converter_RGB_HD.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Converter_RGB_HD.h"
#include "BFC.Image.Picture.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Image::Converter_RGB_HD_Swap
// ============================================================================

Image::Converter_RGB_HD_Swap::Converter_RGB_HD_Swap(
	const	Image::PictureFormat	pIFormat,
	const	Image::PictureFormat	pOFormat ) :

	RawConverter( pIFormat, pOFormat ) {

}

// ============================================================================

void Image::Converter_RGB_HD_Swap::engineForward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	swapHD( pIReaders, pOWriters );

}

void Image::Converter_RGB_HD_Swap::engineBackward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	swapHD( pIReaders, pOWriters );

}

// ============================================================================

void Image::Converter_RGB_HD_Swap::swapHD(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	Mem::Uint16Reader	reader = pIReaders[ 0 ];
	Mem::Uint16Writer	writer = pOWriters[ 0 ];

	Uint32			bufWidth = reader.getNbrCols() / 3;
	Uint32			bufHeight = reader.getNbrRows();

	const Uint16 *		iPtr;
	Uint16 *		oPtr;

	Uint16			cr;
	Uint16			cg;
	Uint16			cb;

	for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
		iPtr = ( const Uint16 * )reader.getRowAddress( y );
		oPtr = ( Uint16 * )writer.getRowAddress( y );
		for ( Uint32 x = bufWidth ; x ; x-- ) {
			cr = *iPtr++;
			cg = *iPtr++;
			cb = *iPtr++;
			*oPtr++ = cb;
			*oPtr++ = cg;
			*oPtr++ = cr;
		}
	}

}

// ============================================================================

static Image::ConverterPtr swap_48 = new Image::Converter_RGB_HD_Swap(
	Image::PictureFormat::BGR48,
	Image::PictureFormat::RGB48 );

static Image::ConverterPtr swap_36 = new Image::Converter_RGB_HD_Swap(
	Image::PictureFormat::BGR36,
	Image::PictureFormat::RGB36 );

static Image::ConverterPtr swap_30 = new Image::Converter_RGB_HD_Swap(
	Image::PictureFormat::BGR30,
	Image::PictureFormat::RGB30 );

// ============================================================================
// BFC::Image::Converter_RGB_HD_Shift
// ============================================================================

Image::Converter_RGB_HD_Shift::Converter_RGB_HD_Shift(
	const	Image::PictureFormat	pIFormat,
	const	Image::PictureFormat	pOFormat,
	const	Uint32			pShift,
	const	Bool			pFromSD ) :

	RawConverter( pIFormat, pOFormat ),
	shift( pShift ),
	fromSD( pFromSD ) {

}

// ============================================================================

void Image::Converter_RGB_HD_Shift::engineForward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	if ( fromSD ) {

		for ( Uint32 s = 0 ; s < pIReaders.getSize() ; s++ ) {
	
			Mem::UcharReader	reader = pIReaders[ s ];
			Mem::Uint16Writer	writer = pOWriters[ s ];
		
			Uint32			bufWidth = reader.getNbrCols();
			Uint32			bufHeight = reader.getNbrRows();
		
			const Uchar *		iPtr;
			Uint16 *		oPtr;
		
			for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
				iPtr = ( const Uchar * )reader.getRowAddress( y );
				oPtr = ( Uint16 * )writer.getRowAddress( y );
				for ( Uint32 x = bufWidth ; x ; x-- ) {
					*oPtr++ = ( ( ( Uint16 )*iPtr++ ) << shift );
				}
			}
	
		}

	}
	else {

		for ( Uint32 s = 0 ; s < pIReaders.getSize() ; s++ ) {
	
			Mem::Uint16Reader	reader = pIReaders[ s ];
			Mem::Uint16Writer	writer = pOWriters[ s ];
		
			Uint32			bufWidth = reader.getNbrCols();
			Uint32			bufHeight = reader.getNbrRows();
		
			const Uint16 *		iPtr;
			Uint16 *		oPtr;
		
			for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
				iPtr = ( const Uint16 * )reader.getRowAddress( y );
				oPtr = ( Uint16 * )writer.getRowAddress( y );
				for ( Uint32 x = bufWidth ; x ; x-- ) {
					*oPtr++ = ( *iPtr++ << shift );
				}
			}
	
		}

	}

}

void Image::Converter_RGB_HD_Shift::engineBackward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	if ( fromSD ) {

		for ( Uint32 s = 0 ; s < pIReaders.getSize() ; s++ ) {
	
			Mem::Uint16Reader	reader = pIReaders[ s ];
			Mem::UcharWriter	writer = pOWriters[ s ];
		
			Uint32			bufWidth = reader.getNbrCols();
			Uint32			bufHeight = reader.getNbrRows();
		
			const Uint16 *		iPtr;
			Uchar *			oPtr;
		
			for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
				iPtr = ( const Uint16 * )reader.getRowAddress( y );
				oPtr = ( Uchar * )writer.getRowAddress( y );
				for ( Uint32 x = bufWidth ; x ; x-- ) {
					*oPtr++ = ( *iPtr++ >> shift );
				}
			}
	
		}

	}
	else {

		for ( Uint32 s = 0 ; s < pIReaders.getSize() ; s++ ) {
	
			Mem::Uint16Reader	reader = pIReaders[ s ];
			Mem::Uint16Writer	writer = pOWriters[ s ];
		
			Uint32			bufWidth = reader.getNbrCols();
			Uint32			bufHeight = reader.getNbrRows();
		
			const Uint16 *		iPtr;
			Uint16 *		oPtr;
		
			for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
				iPtr = ( const Uint16 * )reader.getRowAddress( y );
				oPtr = ( Uint16 * )writer.getRowAddress( y );
				for ( Uint32 x = bufWidth ; x ; x-- ) {
					*oPtr++ = ( *iPtr++ >> shift );
				}
			}
	
		}

	}

}

// ============================================================================

static Image::ConverterPtr shift_rgb_24_30 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB24,
	Image::PictureFormat::RGB30,
	2,
	true );

static Image::ConverterPtr shift_rgb_24_36 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB24,
	Image::PictureFormat::RGB36,
	4,
	true );

static Image::ConverterPtr shift_rgb_24_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB24,
	Image::PictureFormat::RGB48,
	8,
	true );

static Image::ConverterPtr shift_rgb_30_36 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB30,
	Image::PictureFormat::RGB36,
	2,
	false );

static Image::ConverterPtr shift_rgb_30_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB30,
	Image::PictureFormat::RGB48,
	6,
	false );

static Image::ConverterPtr shift_rgb_36_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB36,
	Image::PictureFormat::RGB48,
	4,
	false );

static Image::ConverterPtr shift_bgr_24_30 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR24,
	Image::PictureFormat::BGR30,
	2,
	true );

static Image::ConverterPtr shift_bgr_24_36 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR24,
	Image::PictureFormat::BGR36,
	4,
	true );

static Image::ConverterPtr shift_bgr_24_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR24,
	Image::PictureFormat::BGR48,
	8,
	true );

static Image::ConverterPtr shift_bgr_30_36 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR30,
	Image::PictureFormat::BGR36,
	2,
	false );

static Image::ConverterPtr shift_bgr_30_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR30,
	Image::PictureFormat::BGR48,
	6,
	false );

static Image::ConverterPtr shift_bgr_36_48 = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::BGR36,
	Image::PictureFormat::BGR48,
	4,
	false );

static Image::ConverterPtr shift_rgb_24_30_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB,
	Image::PictureFormat::RGB30P,
	2,
	true );

static Image::ConverterPtr shift_rgb_24_36_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB,
	Image::PictureFormat::RGB36P,
	4,
	true );

static Image::ConverterPtr shift_rgb_24_48_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB,
	Image::PictureFormat::RGB48P,
	8,
	true );

static Image::ConverterPtr shift_rgb_30_36_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB30P,
	Image::PictureFormat::RGB36P,
	2,
	false );

static Image::ConverterPtr shift_rgb_30_48_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB30P,
	Image::PictureFormat::RGB48P,
	6,
	false );

static Image::ConverterPtr shift_rgb_36_48_p = new Image::Converter_RGB_HD_Shift(
	Image::PictureFormat::RGB36P,
	Image::PictureFormat::RGB48P,
	4,
	false );

// ============================================================================
// BFC::Image::Converter_RGB_HD_Split
// ============================================================================

Image::Converter_RGB_HD_Split::Converter_RGB_HD_Split(
	const	Image::PictureFormat	pIFormat,
	const	Image::PictureFormat	pOFormat,
	const	Bool			pToRGB ) :

	RawConverter( pIFormat, pOFormat ),
	toRGB( pToRGB ) {

}

// ============================================================================

void Image::Converter_RGB_HD_Split::engineForward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	Mem::Uint16Reader	reader0 = pIReaders[ 0 ];
	Mem::Uint16Reader	reader1 = pIReaders[ 1 ];
	Mem::Uint16Reader	reader2 = pIReaders[ 2 ];
	Mem::Uint16Writer	writer = pOWriters[ 0 ];

	Uint32			bufWidth = reader0.getNbrCols();
	Uint32			bufHeight = reader0.getNbrRows();

	const Uint16 *		iPtr0;
	const Uint16 *		iPtr1;
	const Uint16 *		iPtr2;
	Uint16 *		oPtr;

	if ( toRGB ) {

		for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
			iPtr0 = ( const Uint16 * )reader0.getRowAddress( y );
			iPtr1 = ( const Uint16 * )reader1.getRowAddress( y );
			iPtr2 = ( const Uint16 * )reader2.getRowAddress( y );
			oPtr = ( Uint16 * )writer.getRowAddress( y );
			for ( Uint32 x = bufWidth ; x ; x-- ) {
				*oPtr++ = *iPtr0++;
				*oPtr++ = *iPtr1++;
				*oPtr++ = *iPtr2++;
			}
		}

	}
	else {

		for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
			iPtr0 = ( const Uint16 * )reader0.getRowAddress( y );
			iPtr1 = ( const Uint16 * )reader1.getRowAddress( y );
			iPtr2 = ( const Uint16 * )reader2.getRowAddress( y );
			oPtr = ( Uint16 * )writer.getRowAddress( y );
			for ( Uint32 x = bufWidth ; x ; x-- ) {
				*oPtr++ = *iPtr2++;
				*oPtr++ = *iPtr1++;
				*oPtr++ = *iPtr0++;
			}
		}

	}

}

void Image::Converter_RGB_HD_Split::engineBackward(
		ReaderArray &	pIReaders,
		WriterArray &	pOWriters ) {

	Mem::Uint16Reader	reader = pIReaders[ 0 ];
	Mem::Uint16Writer	writer0 = pOWriters[ 0 ];
	Mem::Uint16Writer	writer1 = pOWriters[ 1 ];
	Mem::Uint16Writer	writer2 = pOWriters[ 2 ];

	Uint32			bufWidth = writer0.getNbrCols();
	Uint32			bufHeight = writer0.getNbrRows();

	const Uint16 *		iPtr;
	Uint16 *		oPtr0;
	Uint16 *		oPtr1;
	Uint16 *		oPtr2;

	if ( toRGB ) {

		for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
			iPtr = ( const Uint16 * )reader.getRowAddress( y );
			oPtr0 = ( Uint16 * )writer0.getRowAddress( y );
			oPtr1 = ( Uint16 * )writer1.getRowAddress( y );
			oPtr2 = ( Uint16 * )writer2.getRowAddress( y );
			for ( Uint32 x = bufWidth ; x ; x-- ) {
				*oPtr0++ = *iPtr++;
				*oPtr1++ = *iPtr++;
				*oPtr2++ = *iPtr++;
			}
		}

	}
	else {

		for ( Uint32 y = 0 ; y < bufHeight ; y++ ) {
			iPtr = ( const Uint16 * )reader.getRowAddress( y );
			oPtr0 = ( Uint16 * )writer0.getRowAddress( y );
			oPtr1 = ( Uint16 * )writer1.getRowAddress( y );
			oPtr2 = ( Uint16 * )writer2.getRowAddress( y );
			for ( Uint32 x = bufWidth ; x ; x-- ) {
				*oPtr2++ = *iPtr++;
				*oPtr1++ = *iPtr++;
				*oPtr0++ = *iPtr++;
			}
		}

	}

}

// ============================================================================

static Image::ConverterPtr split_rgb_30 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB30P,
	Image::PictureFormat::RGB30,
	true );

static Image::ConverterPtr split_rgb_36 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB36P,
	Image::PictureFormat::RGB36,
	true );

static Image::ConverterPtr split_rgb_48 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB48P,
	Image::PictureFormat::RGB48,
	true );

static Image::ConverterPtr split_bgr_30 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB30P,
	Image::PictureFormat::BGR30,
	false );

static Image::ConverterPtr split_bgr_36 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB36P,
	Image::PictureFormat::BGR36,
	false );

static Image::ConverterPtr split_bgr_48 = new Image::Converter_RGB_HD_Split(
	Image::PictureFormat::RGB48P,
	Image::PictureFormat::BGR48,
	false );

// ============================================================================

