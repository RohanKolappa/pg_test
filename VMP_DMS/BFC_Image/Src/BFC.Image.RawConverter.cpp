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
//	BFC.Image.RawConverter.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Picture.h"
#include "BFC.Image.RawConverter.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::RawConverter::RawConverter(
	const	PictureFormat &	pIFormat,
	const	PictureFormat &	pOFormat ) :

	Converter( pIFormat, pOFormat ) {

}

// ============================================================================

Image::Picture Image::RawConverter::convertForward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertForward( iPic, oPic );

	return oPic;

}

void Image::RawConverter::convertForward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	// Sanity checks...

	if ( iPic.getFormat() != getIFormat() ) {
		throw InternalError( "Invalid input format!" );
	}

	Uint32		picW = iPic.getWidth();
	Uint32		picH = iPic.getHeight();

	if ( ! getOFormat().canRepresent( picW, picH ) ) {
		throw InternalError(
			"Can't represent picture in \""
			+ getOFormat().getShortName()
			+ "\"!" );
	}

	// Let's prepare the arrays to pass to the internal converter...

	ReaderArray	iPicData;
	WriterArray	oPicData;

	if ( oPic.getWidth() != picW
	  || oPic.getHeight() != picH
	  || oPic.getFormat() != getOFormat() ) {

		oPic = Picture( picW, picH, getOFormat(), false ); // result!

	}

	iPicData.resize( iPic.getNbrPlanes() );

	for ( Uint32 i = 0 ; i < iPic.getNbrPlanes() ; i++ ) {
		iPicData[ i ] = iPic.getPlaneReader( i );
	}

	oPicData.resize( oPic.getNbrPlanes() );

	for ( Uint32 i = 0 ; i < oPic.getNbrPlanes() ; i++ ) {
		oPicData[ i ] = oPic.getPlaneWriter( i );
	}

	// Let's proceed to the real conversion...

	try {
		engineForward( iPicData, oPicData );
	}
	catch ( Exception & e ) {
		throw CantConvertPicture( e );
	}

}

Image::Picture Image::RawConverter::convertBackward(
	const	Picture &	iPic ) {

	Picture	oPic;

	convertBackward( iPic, oPic );

	return oPic;

}

void Image::RawConverter::convertBackward(
	const	Picture &	iPic,
		Picture &	oPic ) {

	// Sanity checks...

	if ( iPic.getFormat() != getOFormat() ) {
		throw InternalError( "Invalid input format!" );
	}

	Uint32		picW = iPic.getWidth();
	Uint32		picH = iPic.getHeight();

	if ( ! getIFormat().canRepresent( picW, picH ) ) {
		throw InternalError( "Can't represent picture in \""
			+ getIFormat().getShortName()
			+ "\"!" );
	}

	// Let's prepare the arrays to pass to the internal converter...

	ReaderArray	iPicData;
	WriterArray	oPicData;

	if ( oPic.getWidth() != picW
	  || oPic.getHeight() != picH
	  || oPic.getFormat() != getOFormat() ) {

		oPic = Picture( picW, picH, getIFormat(), false ); // result!

	}

	iPicData.resize( iPic.getNbrPlanes() );

	for ( Uint32 i = 0 ; i < iPic.getNbrPlanes() ; i++ ) {
		iPicData[ i ] = iPic.getPlaneReader( i );
	}

	oPicData.resize( oPic.getNbrPlanes() );

	for ( Uint32 i = 0 ; i < oPic.getNbrPlanes() ; i++ ) {
		oPicData[ i ] = oPic.getPlaneWriter( i );
	}

	// Let's proceed to the real conversion...

	try {
		engineBackward( iPicData, oPicData );
	}
	catch ( Exception & e ) {
		throw CantConvertPicture( e );
	}

}

// ============================================================================

