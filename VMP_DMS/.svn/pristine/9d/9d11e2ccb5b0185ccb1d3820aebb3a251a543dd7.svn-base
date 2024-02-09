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
//	BFC.Sound.ChunkFormat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.ChunkFormat.h"
#include "BFC.Sound.TrackFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, ChunkFormat )

// ============================================================================

Sound::ChunkFormat::ChunkFormat(
	const	Frac64 &	pRate ) :

	rte	( pRate ),
	nbr	( 0 ) {

}

Sound::ChunkFormat::ChunkFormat(
	const	Uint32		pNbrChans,
	const	Uint32		pSampRate,
	const	Uint32		pSampSize,
	const	Bool		pInterlaced ) :

	rte	( Frac64( pSampRate, 1 ) ),
	nbr	( 0 ) {

	if ( pInterlaced ) {
		addTrack( new TrackFormat( pSampSize, pNbrChans ) );
	}
	else {
		TrackFormatCPtr t = new TrackFormat( pSampSize, 1 );
		for ( Uint32 i = 0 ; i < pNbrChans ; i++ ) {
			addTrack( t );
		}
	}

}

// ============================================================================

void Sound::ChunkFormat::addTrack(
		TrackFormatCPtr	pTrackFormat,
	const	Uint32		pNbrTracks ) {

	for ( Uint32 i = 0 ; i < pNbrTracks ; i++ ) {
		tbl += pTrackFormat;
		nbr += pTrackFormat->getNbrSamples();
	}

}

// ============================================================================

Bool Sound::ChunkFormat::isInterlaced() const {

	for ( Uint32 i = 0 ; i < tbl.getSize() ; i++ ) {
		if ( tbl[ i ]->isInterlaced() ) {
			return true;
		}
	}

	return false;

}

Sound::ChunkFormatCPtr Sound::ChunkFormat::getUnInterlaced() const {

	ChunkFormatPtr res = new ChunkFormat( rte );

	for ( Uint32 i = 0 ; i < tbl.getSize() ; i++ )
	for ( Uint32 j = 0 ; j < tbl[ i ]->getNbrSamples() ; j++ ) {
		res->addTrack(
			new TrackFormat(
				tbl[ i ]->getSample( j ),
				tbl[ i ]->getName( j ) ) );
	}

	return res;

}

// ============================================================================

Buffer Sound::ChunkFormat::toBuffer() const {

	Buffer res = Buffer( rte.getNum() ) + ":" + Buffer( rte.getDen() );

	for ( Uint32 i = 0 ; i < tbl.getSize() ; i++ ) {
		res += ", ";
		res += tbl[ i ]->toBuffer();
	}

	return res;

}

// ============================================================================

#endif // 0

// ============================================================================

