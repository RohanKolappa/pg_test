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
//	BFC.Image.Resolution.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Image.Resolution.h"
#include "BFC.Image.ResolutionRegistry.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::Resolution::Resolution(
	const	Uint32		pWidth,
	const	Uint32		pHeight ) {

	for ( Uint32 i = MinFormat
	    ; i < ResolutionRegistry::instance()->getNbrManifests()
	    ; i++ ) {
		const ResolutionManifest & m
			= ResolutionRegistry::instance()->getManifest( i );
		if ( m.getWidth() == pWidth
		  && m.getHeight() == pHeight ) {
			idx = i;
			return;
		}
	}

	idx = Unknown;

}

Image::Resolution::Resolution(
	const	UUId &		uuid ) {

	try {
		idx = ResolutionRegistry::instance()->getIndex( uuid );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

Image::Resolution::Resolution(
	const	Buffer &	name ) {

	Buffer tmpName = name.toLower();

	for ( Uint32 i = MinFormat
	    ; i < ResolutionRegistry::instance()->getNbrManifests()
	    ; i++ ) {
		const ResolutionManifest & m
			= ResolutionRegistry::instance()->getManifest( i );
		if ( m.getShortName() == tmpName ) {
			idx = i;
			return;
		}
	}

	idx = Unknown;

}

// ============================================================================

const Image::ResolutionManifest & Image::Resolution::getManifest() const {

	return ( ResolutionRegistry::instance()->getManifest( idx ) );

}

// ============================================================================

