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
//	BFC.Image.PlaneFormatManifest.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.PlaneFormatManifest.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::PlaneFormatManifest::PlaneFormatManifest() :

	colorChannel	( ColorChannel::Unknown ),
	bitsPerPixel	( 0 ),
	pixelsPerElt	( 1 ),
	minXDownsmpl	( 1 ),
	minYDownsmpl	( 1 ),
	maxXDownsmpl	( 1 ),
	maxYDownsmpl	( 1 ) {

}

Image::PlaneFormatManifest::PlaneFormatManifest(
	const	UUId &		__univUniqueId ) :

	univUniqueId	( __univUniqueId ),
	colorChannel	( ColorChannel::Unknown ),
	bitsPerPixel	( 0 ),
	pixelsPerElt	( 1 ),
	minXDownsmpl	( 1 ),
	minYDownsmpl	( 1 ),
	maxXDownsmpl	( 1 ),
	maxYDownsmpl	( 1 ) {

}

Image::PlaneFormatManifest::PlaneFormatManifest(
	const	UUId &		__univUniqueId,
	const	ColorChannel	__colorChannel,
	const	Uint32		__bitsPerPixel,
	const	Uint32		__pixelsPerElt,
	const	Uint32		__minXDownsmpl,
	const	Uint32		__minYDownsmpl,
	const	Uint32		__maxXDownsmpl,
	const	Uint32		__maxYDownsmpl ) :

	univUniqueId	( __univUniqueId ),
	colorChannel	( __colorChannel ),
	bitsPerPixel	( __bitsPerPixel ),
	pixelsPerElt	( __pixelsPerElt ),
	minXDownsmpl	( __minXDownsmpl ),
	minYDownsmpl	( __minYDownsmpl ),
	maxXDownsmpl	( __maxXDownsmpl ),
	maxYDownsmpl	( __maxYDownsmpl ) {

}

Image::PlaneFormatManifest::PlaneFormatManifest(
	const	PlaneFormatManifest &	other ) :

	SObject		(),
	DObject		(),
	VarUser		( other ),
	univUniqueId	( other.univUniqueId ),
	colorChannel	( other.colorChannel ),
	bitsPerPixel	( other.bitsPerPixel ),
	pixelsPerElt	( other.pixelsPerElt ),
	minXDownsmpl	( other.minXDownsmpl ),
	minYDownsmpl	( other.minYDownsmpl ),
	maxXDownsmpl	( other.maxXDownsmpl ),
	maxYDownsmpl	( other.maxYDownsmpl ) {

}

// ============================================================================

Image::PlaneFormatManifest & Image::PlaneFormatManifest::operator = (
	const	PlaneFormatManifest &	other ) {

	if ( this != & other ) {
		VarUser::operator = ( other );
		univUniqueId = other.univUniqueId;
		colorChannel = other.colorChannel;
		bitsPerPixel = other.bitsPerPixel;
		pixelsPerElt = other.pixelsPerElt;
		minXDownsmpl = other.minXDownsmpl;
		minYDownsmpl = other.minYDownsmpl;
		maxXDownsmpl = other.maxXDownsmpl;
		maxYDownsmpl = other.maxYDownsmpl;
	}

	return *this;

}

// ============================================================================

Buffer Image::PlaneFormatManifest::toBuffer() const {

	return Buffer( bitsPerPixel )
		+ " bpp, "
		+ Buffer( pixelsPerElt )
		+ " ppe, min downs: "
		+ Buffer( minXDownsmpl )
		+ "x"
		+ Buffer( minYDownsmpl )
		+ ", max downs: "
		+ Buffer( maxXDownsmpl )
		+ "x"
		+ Buffer( maxYDownsmpl );

}

// ============================================================================

