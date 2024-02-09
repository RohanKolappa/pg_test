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
//	BFC.Image.ResolutionManifest.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.ResolutionManifest.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::ResolutionManifest::ResolutionManifest() :

	width( 0 ),
	height( 0 ),
	longName( "n/a" ),
	shortName( "n/a" ) {

}

Image::ResolutionManifest::ResolutionManifest(
	const	UUId &		pUuid ) :

	uuid( pUuid ),
	width( 0 ),
	height( 0 ),
	longName( "n/a" ),
	shortName( "n/a" ) {

}

Image::ResolutionManifest::ResolutionManifest(
	const	UUId &		pUuid,
	const	Uint32		pWidth,
	const	Uint32		pHeight,
	const	Char *		pLongName,
	const	Char *		pShortName ) :

	uuid( pUuid ),
	width( pWidth ),
	height( pHeight ),
	longName( pLongName ),
	shortName( pShortName ) {

}

Image::ResolutionManifest::ResolutionManifest(
	const	ResolutionManifest &	other ) :

	SObject		(),
	DObject		(),
	Dyn::VarUser	(),
	uuid		( other.uuid ),
	width		( other.width ),
	height		( other.height ),
	longName	( other.longName ),
	shortName	( other.shortName ) {

}

Image::ResolutionManifest & Image::ResolutionManifest::operator = (
	const	ResolutionManifest &	other ) {

	if ( this != &other ) {
		uuid = other.uuid;
		width = other.width;
		height = other.height;
		longName = other.longName;
		shortName = other.shortName;
	}

	return *this;

}

// ============================================================================

