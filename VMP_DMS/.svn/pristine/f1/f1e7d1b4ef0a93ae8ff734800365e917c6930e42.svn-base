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
//	BFC.Image.PictureFormat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TEnum.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Image.PictureFormat.h"

#include "BFC.Math.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Image::PictureFormat::PictureFormat(
	const	UUId &		uuid ) {

	try {
		idx = PictureFormatRegistry::instance()->getIndex( uuid );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

Image::PictureFormat::PictureFormat(
	const	FourCC &	fourcc ) {

	try {
		idx = PictureFormatRegistry::instance()->getIndex( fourcc );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

Image::PictureFormat::PictureFormat(
	const	Uint32		pIndex ) {

	idx = ( pIndex < PictureFormatRegistry::instance()->getNbrManifests()
		? pIndex
		: ( Uint32 )Unknown );

}

Image::PictureFormat::PictureFormat(
	const	Uint32		bitsPerPixel,
	const	Uint32		nbrPlanes,
	const	ColorSpace	csp ) {

	for ( Uint32 i = MinFormat ; i < MaxFormat ; i++ ) {
		const PictureFormatManifest & m
			= PictureFormatRegistry::instance()->getManifest( i );
		if ( m.bitsPerPixel == bitsPerPixel
		  && m.planes.size() == nbrPlanes
		  && ( csp == ColorSpace::Unknown
		    || csp == m.colorSpace ) ) {
			idx = i;
			return;
		}
	}

	idx = Unknown;

}

Image::PictureFormat::PictureFormat(
	const	Buffer		& name ) {

	for ( Uint32 i = MinFormat ; i < MaxFormat ; i++ ) {
		const PictureFormatManifest & m
			= PictureFormatRegistry::instance()->getManifest( i );
		if ( m.shortName == name ) {
			idx = i;
			return;
		}
	}

	idx = Unknown;

}

// ============================================================================

Image::PictureFormat Image::PictureFormat::getPlanar() const {

// FIXME: stupid algo.

	if ( isUserDefined() ) {
		throw InvalidArgument( "User types not convertible!" );
	}

	if ( getPredefined() == Unknown ) {
		throw InvalidArgument( "Unknown type not convertible!" );
	}

	if ( ! isPacked() ) {
		return *this;
	}

	ColorSpace cs = getColorSpace();

	if ( cs == ColorSpace::RGB ) {
		return RGB;
	}

	if ( cs == ColorSpace::YUV && *this != YUV400 ) {
		return YUV444P;
	}

	if ( cs == ColorSpace::JYUV && *this != JYUV400 ) {
		return JYUV444P;
	}

	if ( cs == ColorSpace::RYUV ) {
		return RYUV444P;
	}

	throw InvalidArgument(
		"Can't force planar with \""
		+ getShortName()
		+ "\"!" );

}

// ============================================================================

Uint32 Image::PictureFormat::getBytesPerPicture(
	const	Uint32		picWidth,
	const	Uint32		picHeight ) const {

	Uint32	res = 0;
	Uint32	cnt = getNbrPlanes();

	while ( cnt-- ) {
		res += getBytesPerPlane( cnt, picWidth, picHeight );
	}

	return res;

}

// ============================================================================

Bool Image::PictureFormat::canRepresent(
	const	Uint32		picWidth,
	const	Uint32		picHeight ) const {

	return ( isKnown()
	      && ( picWidth % getMaxXDownsampling() ) == 0
	      && ( picHeight % getMaxYDownsampling() ) == 0 );

}

// ============================================================================

void Image::PictureFormat::doSerialize(
		IO::ByteOutputStreamPtr
				pOStream ) const {

	pOStream->putUUId( getUUId() );

}

void Image::PictureFormat::unSerialize(
		IO::ByteInputStreamPtr
				pIStream ) {

	try {
		idx = PictureFormatRegistry::instance()->getIndex( pIStream->getUUId() );
	}
	catch ( Exception & ) {
		idx = Unknown;
	}

}

// ============================================================================

Dyn::TEnumCPtr Image::PictureFormat::getTEnum(
	const	Bool		pUnknown ) {

	static Dyn::TEnumCPtr t[ 2 ] = {
			makeTEnum( false ),
			makeTEnum( true ) };

	return t[ pUnknown ? 1 : 0 ];

}

// ============================================================================

Dyn::TEnumCPtr Image::PictureFormat::makeTEnum(
	const	Bool		pUnknown ) {

	PictureFormatRegistryCPtr r = PictureFormatRegistry::instance();

	Dyn::TEnumPtr	res = new Dyn::TEnum;

	if ( pUnknown ) {
		res->addEntry( r->getManifest( Unknown ).shortName );
	}

	for ( Uint32 i = MinFormat ; i < MaxFormat ; i++ ) {
		res->addEntry( r->getManifest( i ).shortName );
	}

	return res;

}

// ============================================================================

