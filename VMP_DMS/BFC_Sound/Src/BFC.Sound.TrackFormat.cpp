// ============================================================================

#if defined( SOUND_OLD_API )

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
//	BFC.Sound.TrackFormat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.SampleFormat.h"
#include "BFC.Sound.TrackFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, TrackFormat )

// ============================================================================

Sound::TrackFormat::TrackFormat(
	const	Uint32		pSampSize,
	const	Uint32		pNbrChans ) {

	SampleFormatCPtr t = SampleFormat::getLEInteger( pSampSize );

	for ( Uint32 i = 0 ; i < pNbrChans ; i++ ) {
		addSample( t, "Channel_" + Buffer( i ) );
	}

}

// ============================================================================

Uint32 Sound::TrackFormat::getTotalLength() const {

	return ( tbl.isEmpty()
		? 0
		: tbl.getLast()->off + tbl.getLast()->len );

}

Bool Sound::TrackFormat::isInterlaced() const {

	return ( tbl.getSize() > 1 );

}

// ============================================================================

void Sound::TrackFormat::addSample(
		SampleFormatCPtr
				pFormat,
	const	Buffer &	pName ) {

	Uint32 offset = tbl.isEmpty()
		? 0
		: tbl.getLast()->off + tbl.getLast()->len;

	tbl += new Entry(
		pFormat,
		pName,
		offset,
		pFormat->getNbrBits() );

}

// ============================================================================

Buffer Sound::TrackFormat::toBuffer() const {

	Buffer res = "( ";

	for ( Uint32 i = 0 ; i < tbl.getSize() ; i++ ) {
		if ( i ) {
			res += ", ";
		}
		res += Buffer( tbl[ i ]->nme ) + ":"
			+ Buffer( tbl[ i ]->off ) + ":"
			+ Buffer( tbl[ i ]->len ) + ":"
			+ tbl[ i ]->fmt->getName();
	}

	res += " )";

	return res;

}

// ============================================================================

BFC_PTR_IMPL_NS_CC( Sound, TrackFormat, Entry )

// ============================================================================

#endif // 0

// ============================================================================

