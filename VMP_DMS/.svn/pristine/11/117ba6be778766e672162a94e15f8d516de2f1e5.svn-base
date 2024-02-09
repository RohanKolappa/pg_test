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
//	BFC.Sound.Chunk.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Sound.Chunk.h"
#include "BFC.Sound.ChunkFormat.h"
#include "BFC.Sound.TrackFormat.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, Chunk )

// ============================================================================

Sound::Chunk::Chunk(
		ChunkFormatCPtr	pFormat,
	const	Uint32		pLength ) :

	chunkL	( pLength ),
	chunkF	( pFormat ) {

	setObjectName( "BFC.Sound.Chunk" );

	Uint32 nbr = chunkF->getNbrTracks();

	chunkT.resize( nbr );

}

Sound::Chunk::Chunk(
	const	Chunk &		pOther ) :

	SObject	(),
	DObject	(),

	chunkL	( pOther.chunkL ),
	chunkF	( pOther.chunkF ),
	chunkT	( pOther.chunkT ) {

	setObjectName( "BFC.Sound.Chunk" );

}

// ============================================================================

Sound::Chunk & Sound::Chunk::operator = (
	const	Chunk &		pOther ) {

	if ( this != &pOther ) {
		chunkL = pOther.chunkL;
		chunkF = pOther.chunkF;
		chunkT = pOther.chunkT;
	}

	return *this;

}

void Sound::Chunk::kill() {

	chunkL = 0;
	chunkF.kill();
	chunkT.kill();

}

// ============================================================================

Bool Sound::Chunk::operator == (
	const	Chunk &		pOther ) const {

	return ( chunkL == pOther.chunkL
	      && chunkF == pOther.chunkF
	      && chunkT == pOther.chunkT );

}

Bool Sound::Chunk::operator != (
	const	Chunk &		pOther ) const {

	return ( ! operator == ( pOther ) );

}

// ============================================================================

Bool Sound::Chunk::isSimilarTo(
	const	Chunk &		pOther ) const {

	return ( chunkL == pOther.chunkL
	      && chunkF == pOther.chunkF
	      && chunkT == pOther.chunkT );

}

Bool Sound::Chunk::isEmpty() const {

	return ( chunkL == 0 );

}

Uint32 Sound::Chunk::getLength() const {

	return chunkL;

}

Sound::ChunkFormatCPtr Sound::Chunk::getFormat() const {

	return chunkF;

}

Uint32 Sound::Chunk::getNbrTracks() const {

	return ( chunkF ? chunkF->getNbrTracks() : 0 );

}

void Sound::Chunk::setTrack(
	const	Uint32		pIndex,
		TrackCPtr	pTrack ) {

	if ( pIndex >= chunkT.getSize() ) {
		throw InvalidArgument( "setTrack(): out of range!" );
	}

	if ( ! chunkF ) {
		throw InvalidArgument( "setTrack(): unknown current format!" );
	}

//	if ( pTrack->getFormat() != chunkF->getTrack( pIndex ) ) {
//		throw InvalidArgument( "setTrack(): formats differ!" );
//	}

	Uint32 trckLen = pTrack->getLength();

	if ( ! chunkL ) {
		chunkL = trckLen;
	}
	else if ( chunkL < trckLen ) {
		throw InvalidArgument();
	}
	else if ( ! trckLen ) {
		throw InvalidArgument( "Adding empty track ?" );
	}
	else if ( chunkL % trckLen ) {
		throw InvalidArgument( "ChunkL % TrackLen!!!" );
	}

	chunkT[ pIndex ] = pTrack;

}

Sound::TrackCPtr Sound::Chunk::getTrack(
	const	Uint32		pIndex ) const {

	return chunkT[ pIndex ];

}

// ============================================================================

//Sound::ChunkPtr Sound::Chunk::doInterlace() const {
//
//	throw NotImplemented();
//
//}
//
//Sound::ChunkPtr Sound::Chunk::unInterlace() const {
//
//	throw NotImplemented();
//
//}

Bool Sound::Chunk::isInterlaced() const {

	return ( chunkF->isInterlaced() );

}

Sound::ChunkCPtr Sound::Chunk::getUnInterlaced() const {

	ChunkPtr res = new Chunk( chunkF->getUnInterlaced(), chunkL );

	Uint32 c = 0;

	for ( Uint32 i = 0 ; i < chunkF->getNbrTracks() ; i++ )
	for ( Uint32 j = 0 ; j < chunkF->getTrack( i )->getNbrSamples() ; j++ ) {
		res->setTrack( c++, chunkT[ i ]->getUnInterlaced( j ) );
	}

	return res;

}

Sound::ChunkCPtr Sound::Chunk::getInterlaced() const {

	TrackPtr	t = new Track( chunkT );
	ChunkFormatPtr	cf = new ChunkFormat( chunkF->getRate() );
	cf->addTrack( t->getFormat() );
	ChunkPtr	r = new Chunk( cf );
	r->setTrack( 0, t );

	return r;

}

// ============================================================================

void Sound::Chunk::checkValidness() {

	if ( ! chunkF ) {
		throw InvalidArgument(
			"Invalid format!" );
	}

//	if ( chunkF->getNbrTracks() != chunkT.getSize() ) {
//		throw InvalidArgument(
//			"Number of planes mismatch (given: "
//			+ Buffer( chunkT.getSize() )
//			+ ", expected: "
//			+ Buffer( chunkF->getNbrTracks() )
////			+ " for "
////			+ chunkF->getName()
//			+ ")!" );
//	}

//	if ( ! chunkF->canRepresent( chunkL ) ) {
//		throw InvalidArgument(
//			"Format incompatible with length "
//			+ Buffer( chunkL )
////			+ " with \""
////			+ chunkF->getName()
//			+ "\"!" );
//	}

//	for ( Uint32 i = 0 ; i < chunkF->getNbrTracks() ; i++ ) {
//
//		if ( chunkT[ i ].getLength() != chunkF->getTrackFormat( i )->getLength( chunkL ) ) {
//			throw InvalidArgument(
//				"Invalid width for track[ "
//				+ Buffer( i )
//				+ " ]! Exp: "
//				+ Buffer( chunkF->getTrackFormat( i )->getLength( chunkL ) )
//				+ ", got: "
//				+ Buffer( chunkT[ i ].getLength() ) );
//		}
//
//		if ( ! chunkF->getTrack( i ) ) {
//			throw InvalidArgument(
//				"Unknown plane[ "
//				+ Buffer( i )
//				+ " ] format!" );
//		}
//
//		if ( ! chunkT[ i ].getFormat() ) {
//			chunkT[ i ].setFormat( chunkF->getTrack( i ) );
//		}
//		else if ( chunkF->getTrack( i ) != chunkT[ i ].getFormat() ) {
//			// WARNING!
//			chunkT[ i ].setFormat( chunkF->getTrack( i ) );
//		}
//
//	}

}

void Sound::Chunk::testConvertible() const {

//	if ( chunkF->isUserDefined() ) {
//		throw InvalidArgument( "Can't convert: User types not convertible!" );
//	}
//
//	if ( chunkF->getPredefined() == ChunkFormat::Unknown ) {
//		throw InvalidArgument( "Can't convert: Unknown type not convertible!" );
//	}

}

// ============================================================================

#endif // 0

// ============================================================================

