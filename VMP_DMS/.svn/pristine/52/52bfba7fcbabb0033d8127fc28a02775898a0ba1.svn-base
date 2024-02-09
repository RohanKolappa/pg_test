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
//	BFC.Sound.Track.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.Track.h"
#include "BFC.Sound.TrackFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, Track )

// ============================================================================

Sound::Track::Track() :

	trackL	( 0 ) {

	setObjectName( "BFC.Sound.Track" );

}

Sound::Track::Track(
		TrackFormatCPtr	pFormat ) :

	trackL	( 0 ),
	trackF	( pFormat ) {

	setObjectName( "BFC.Sound.Track" );

}

Sound::Track::Track(
	const	TrackCArray &	pArray ) {

	setObjectName( "BFC.Sound.Track" );

	trackL = 0;

	if ( pArray.isEmpty() ) {
		return;
	}

	TrackFormatPtr tmpTrackF = new TrackFormat;
	trackL = pArray[ 0 ]->getLength();

	Uint32	tot = 0;

	for ( Uint32 i = 0 ; i < pArray.getSize() ; i++ ) {
		if ( pArray[ i ]->getLength() != trackL ) {
			throw InvalidArgument();
		}
		TrackFormatCPtr tf = pArray[ i ]->getFormat();
		tot += tf->getTotalLength();
		for ( Uint32 j = 0 ; j < tf->getNbrSamples() ; j++ ) {
			tmpTrackF->addSample( tf->getSample( j ), tf->getName( j ) );
		}
	}

	tot /= 8;

	trackF = tmpTrackF;
	trackD = Mem::Block( trackL * tot );

	Mem::UcharWriter wr( trackD );

	Uint32 curOff = 0;
	for ( Uint32 i = 0 ; i < pArray.getSize() ; i++ ) {
		TrackFormatCPtr tf = pArray[ i ]->getFormat();
		Uint32 subTot = tf->getTotalLength() / 8;
		Mem::UcharReader rd = pArray[ i ]->getReader();
		for ( Uint32 j = 0 ; j < trackL ; j++ ) {
			wr.gotoElement( j * tot + curOff );
			for ( Uint32 k = 0 ; k < subTot ; k++ ) {
				*wr = *rd;
				wr++;
				rd++;
			}
		}
		curOff += subTot;
	}

}

// ============================================================================

void Sound::Track::setData(
	const	Buffer &	pData ) {

	if ( ! trackF ) {
		throw InvalidArgument( "No format specified!" );
	}

	Uint32	blkLen = trackF->getTotalLength();

	if ( blkLen % 8 ) {
		throw InvalidArgument( "Spurious block length!" );
	}

	blkLen /= 8;

	trackD = pData;
	trackL = pData.getLength() / blkLen;

//	msgDbg( "setData(): len: " + Buffer( pData.getLength() )
//		+ ", blk: " + Buffer( blkLen ) );

}

void Sound::Track::allocate(
	const	Uint32		pLength ) {

//	msgDbg( "allocate(): length: " + Buffer( pLength ) );

	if ( ! trackF ) {
		throw InvalidArgument( "No format specified!" );
	}

	Uint32	blkLen = trackF->getTotalLength();

	if ( blkLen % 8 ) {
		throw InvalidArgument( "Spurious block length!" );
	}

	blkLen /= 8;

	trackD = Mem::Block( pLength * blkLen );
	trackL = pLength;

}

// ============================================================================

Sound::TrackCPtr Sound::Track::getUnInterlaced(
	const	Uint32		pIndex ) const {

//	msgDbg( "getUnInterlaced()" );

	Uint32	off = trackF->getBitOffset( pIndex );
	Uint32	len = trackF->getBitLength( pIndex );
	Uint32	tot = trackF->getTotalLength();

	if ( off % 8 || len % 8 || tot % 8 ) {
		throw InternalError( "Unaligned samples!" );
	}

	off /= 8;
	len /= 8;
	tot /= 8;

	if ( trackD.getLength() != trackL * tot ) {
		throw InternalError( "Invalid Block geometry!" );
	}

	Mem::Block oBlock = Mem::Block( trackL * len );

	Mem::UcharWriter wr( oBlock );
	Mem::UcharReader rd( trackD );

	for ( Uint32 i = 0 ; i < trackL ; i++ ) {
		rd.gotoElement( i * tot + off );
		wr.gotoElement( i * len );
		for ( Uint32 j = 0 ; j < len ; j++ ) {
			*wr = *rd;
			rd++;
			wr++;
		}
	}

	TrackPtr res = new Track;

	res->trackL = trackL;
	res->trackF = new TrackFormat(
				trackF->getSample( pIndex ),
				trackF->getName( pIndex ) );
	res->trackD = oBlock;

	return res;

}

// ============================================================================

Mem::Reader Sound::Track::getReader() const {

//	msgDbg( "getReader()" );

	if ( ! trackF ) {
		throw InvalidArgument( "No format specified!" );
	}

	return Mem::Reader( trackD, trackF->getTotalLength() );

}

Mem::Writer Sound::Track::getWriter() {

//	msgDbg( "getWriter()" );

	if ( ! trackF ) {
		throw InvalidArgument( "No format specified!" );
	}

	return Mem::Writer( trackD, trackF->getTotalLength() );

}

// ============================================================================

#endif // 0

// ============================================================================

