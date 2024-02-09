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
//	BFC.Image.PictureFormatManifest.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "BFC.Image.PictureFormatManifest.h"
#include "BFC.Image.PlaneFormatRegistry.h"

#include "BFC.Math.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::PictureFormatManifest::PictureFormatManifest() :

	shortName	( "n/a" ),
	longName	( "n/a" ),
	colorSpace	( ColorSpace::Unknown ) {

	computeCachedValues();

}

Image::PictureFormatManifest::PictureFormatManifest(
	const	UUId &		__uuid ) :

	uuid		( __uuid ),
	shortName	( "n/a" ),
	longName	( "n/a" ),
	colorSpace	( ColorSpace::Unknown ) {

	computeCachedValues();

}

Image::PictureFormatManifest::PictureFormatManifest(
	const	UUId &		__uuid,
	const	Buffer &	__fourCCs,
	const	Char *		__shortName,
	const	Char *		__longName,
	const	ColorSpace	__colorSpace,
	const	Uint32		__plane0 ) :

	uuid		( __uuid ),
	shortName	( __shortName ),
	longName	( __longName ),
	colorSpace	( __colorSpace ) {

	planes.resize( 1 );

	planes[ 0 ] = __plane0;

	parseFourCCs( __fourCCs );
	computeCachedValues();

}

Image::PictureFormatManifest::PictureFormatManifest(
	const	UUId &		__uuid,
	const	Buffer &	__fourCCs,
	const	Char *		__shortName,
	const	Char *		__longName,
	const	ColorSpace	__colorSpace,
	const	Uint32		__plane0,
	const	Uint32		__plane1,
	const	Uint32		__plane2 ) :

	uuid		( __uuid ),
	shortName	( __shortName ),
	longName	( __longName ),
	colorSpace	( __colorSpace ) {

	planes.resize( 3 );

	planes[ 0 ] = __plane0;
	planes[ 1 ] = __plane1;
	planes[ 2 ] = __plane2;

	parseFourCCs( __fourCCs );
	computeCachedValues();

}

Image::PictureFormatManifest::PictureFormatManifest(
	const	UUId &		__uuid,
	const	Buffer &	__fourCCs,
	const	Char *		__shortName,
	const	Char *		__longName,
	const	ColorSpace	__colorSpace,
	const	Uint32		__plane0,
	const	Uint32		__plane1,
	const	Uint32		__plane2,
	const	Uint32		__plane3 ) :

	uuid		( __uuid ),
	shortName	( __shortName ),
	longName	( __longName ),
	colorSpace	( __colorSpace ) {

	planes.resize( 4 );

	planes[ 0 ] = __plane0;
	planes[ 1 ] = __plane1;
	planes[ 2 ] = __plane2;
	planes[ 3 ] = __plane3;

	parseFourCCs( __fourCCs );
	computeCachedValues();

}

Image::PictureFormatManifest::PictureFormatManifest(
	const	PictureFormatManifest &	other ) :

	SObject		(),
	DObject		(),
	VarUser		( other ),
	uuid		( other.uuid ),
	fourCCs		( other.fourCCs ),
	shortName	( other.shortName ),
	longName	( other.longName ),
	colorSpace	( other.colorSpace ),
	bitsPerPixel	( other.bitsPerPixel ),
	planes		( other.planes ),
	minXDownsmpl	( other.minXDownsmpl ),
	minYDownsmpl	( other.minYDownsmpl ),
	maxXDownsmpl	( other.maxXDownsmpl ),
	maxYDownsmpl	( other.maxYDownsmpl ) {

}

Image::PictureFormatManifest & Image::PictureFormatManifest::operator = (
	const	PictureFormatManifest &	other ) {

	if ( this != & other ) {
		VarUser::operator = ( other );
		uuid = other.uuid;
		fourCCs = other.fourCCs;
		shortName = other.shortName;
		longName = other.longName;
		colorSpace = other.colorSpace;
		bitsPerPixel = other.bitsPerPixel;
		planes = other.planes;
		minXDownsmpl = other.minXDownsmpl;
		minYDownsmpl = other.minYDownsmpl;
		maxXDownsmpl = other.maxXDownsmpl;
		maxYDownsmpl = other.maxYDownsmpl;
	}

	return * this;

}

// ============================================================================

void Image::PictureFormatManifest::parseFourCCs(
	const	Buffer &	list ) {

	BufferArray tokens = BufferParser::tokenize( list, "," );

	fourCCs.resize( tokens.getSize() );

	for ( Uint32 i = 0 ; i < tokens.getSize() ; i++ ) {
		fourCCs[ i ] = tokens[ i ];
	}

}

// ============================================================================

void Image::PictureFormatManifest::computeCachedValues() {

	// Compute min & max downsampling...

	if ( planes.isEmpty() ) {

		minXDownsmpl = 1;
		minYDownsmpl = 1;
		maxXDownsmpl = 1;
		maxYDownsmpl = 1;
		bitsPerPixel = 0;

	}
	else {

		PlaneFormatRegistryPtr reg = PlaneFormatRegistry::instance();

		minXDownsmpl = reg->getManifest( planes[ 0 ] ).minXDownsmpl;
		minYDownsmpl = reg->getManifest( planes[ 0 ] ).minYDownsmpl;
		maxXDownsmpl = reg->getManifest( planes[ 0 ] ).maxXDownsmpl;
		maxYDownsmpl = reg->getManifest( planes[ 0 ] ).maxYDownsmpl;

		for ( Uint32 cnt = 1 ; cnt < planes.size() ; cnt++ ) {
			minXDownsmpl = Math::getMin( minXDownsmpl, reg->getManifest( planes[ cnt ] ).minXDownsmpl );
			minYDownsmpl = Math::getMin( minYDownsmpl, reg->getManifest( planes[ cnt ] ).minYDownsmpl );
			maxXDownsmpl = Math::getMax( maxXDownsmpl, reg->getManifest( planes[ cnt ] ).maxXDownsmpl );
			maxYDownsmpl = Math::getMax( maxYDownsmpl, reg->getManifest( planes[ cnt ] ).maxYDownsmpl );
		}

		// Compute bits per pixel...

		bitsPerPixel = 0;

		for ( Uint32 cnt = 0 ; cnt < planes.size() ; cnt++ ) {
			bitsPerPixel +=
				reg->getManifest( planes[ cnt ] ).bitsPerPixel
				* maxXDownsmpl
				/ reg->getManifest( planes[ cnt ] ).minXDownsmpl
				* maxYDownsmpl
				/ reg->getManifest( planes[ cnt ] ).minYDownsmpl;
		}

		bitsPerPixel /= maxXDownsmpl * maxYDownsmpl;

	}

}

// ============================================================================

