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
//	BFC.Image.PlaneFormat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Image.PlaneFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::PlaneFormat::PlaneFormat(
	const	UUId &		uuid ) {

	try {
		idx = PlaneFormatRegistry::instance()->getIndex( uuid );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

Image::PlaneFormat::PlaneFormat(
	const	Uint32		bitsPerPixel,
	const	ColorChannel	colorChannel ) {

	for ( Uint32 i = MinFormat ; i < MaxFormat ; i++ ) {
		const PlaneFormatManifest & m
			= PlaneFormatRegistry::instance()->getManifest( i );
		if ( m.bitsPerPixel == bitsPerPixel
		  && ( colorChannel == ColorChannel::Unknown
		    || colorChannel == m.colorChannel ) ) {
			idx = i;
			return;
		}
	}

	idx = Unknown;

}

// ============================================================================

void Image::PlaneFormat::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	pOStream->putUUId( getUUId() );

}

void Image::PlaneFormat::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	try {
		idx = PlaneFormatRegistry::instance()->getIndex( pIStream->getUUId() );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

// ============================================================================

