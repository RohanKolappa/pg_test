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
//	BFC.Sound.Block.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Math.Utils.h"

#include "BFC.Mem.UnivS32Reader.h"
#include "BFC.Mem.UnivS32Writer.h"

#include "BFC.Sound.Block.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Sound, Block )

// ============================================================================

Sound::Block::Block(
		FormatCPtr	pFormat,
	const	Uint32		pLength ) :

	length	( pLength ),
	format	( pFormat ),
	planar	( false ) {

}

Sound::Block::Block(
		FormatCPtr	pFormat,
	const	Buffer &	pPacked ) :

	length	( 0 ),
	format	( pFormat ),
	planar	( false ) {

	setData( pPacked );

}

Sound::Block::Block(
	const	Block &		pOther ) :

	SObject	(),

	length	( pOther.length ),
	format	( pOther.format ),
	planar	( pOther.planar ),
	packed	( pOther.packed ),
	blocks	( pOther.blocks ) {

}

// ============================================================================

Sound::Block & Sound::Block::operator = (
	const	Block &		pOther ) {

	if ( this != &pOther ) {
		length	= pOther.length;
		format	= pOther.format;
		planar	= pOther.planar;
		packed	= pOther.packed;
		blocks	= pOther.blocks;
	}

	return *this;

}

// ============================================================================

void Sound::Block::kill() {

	length = 0;
	planar = false;
	packed.kill();
	blocks.kill();

}

// ============================================================================

Buffer Sound::Block::getData() const {

	moveToPacked();

	return packed.toBuffer();

}

void Sound::Block::setData(
	const	Buffer &	pData ) {

	Uint32 sze = format->getTotalSize( true );

	if ( ! sze ) {
		throw InternalError( "Invalid format: " + format->toBuffer() );
	}

	if ( ! length ) {
		length = pData.getLength() / sze;
	}

	if ( length * sze != pData.getLength() ) {
		throw InvalidArgument( "Length mismatch (exp: "
			+ Buffer( length * sze ) + ", got: "
			+ Buffer( pData.getLength() ) + ")!" );
	}

	planar = false;
	packed = pData;
	blocks.kill();

}

// ============================================================================

Buffer Sound::Block::getData(
	const	Uint32		pIndex ) const {

	moveToPlanar();

	return blocks[ pIndex ].toBuffer();

}

const Mem::Block & Sound::Block::getBlock(
	const	Uint32		pIndex ) const {

	moveToPlanar();

	return blocks[ pIndex ];

}

void Sound::Block::setData(
	const	Uint32		pIndex,
	const	Buffer &	pData ) {

	Uint32 sze = format->getSampleSize( true );

	if ( ! sze ) {
		throw InternalError();
	}

	if ( ! length ) {
		length = pData.getLength() / sze;
	}

	if ( length * sze != pData.getLength() ) {
		throw InvalidArgument( "Length mismatch (exp: "
			+ Buffer( length * sze ) + ", got: "
			+ Buffer( pData.getLength() ) + ")!" );
	}

	moveToPlanar();

	blocks[ pIndex ] = pData;

}

void Sound::Block::setBlock(
	const	Uint32		pIndex,
	const	Mem::Block &	pData ) {

	setData( pIndex, pData.toBuffer() );

}

// ============================================================================

Mem::Reader Sound::Block::getReader() const {

	moveToPacked();

	return Mem::Reader( packed, format->getSampleSize( true ) );

}

Mem::Writer Sound::Block::getWriter() {

	moveToPacked();

	return Mem::Writer( packed, format->getSampleSize( true ) );

}

Mem::Reader Sound::Block::getReader(
	const	Uint32		pIndex ) const {

	moveToPlanar();

	ensureBlock( pIndex );

	return Mem::Reader( blocks[ pIndex ], format->getSampleSize( true ) );

}

Mem::Writer Sound::Block::getWriter(
	const	Uint32		pIndex ) {

	moveToPlanar();

	ensureBlock( pIndex );

	return Mem::Writer( blocks[ pIndex ], format->getSampleSize( true ) );

}

// ============================================================================

Sound::BlockCPtr Sound::Block::getLeft(
	const	Uint32		pLength ) const {

	moveToPacked();

	Uint32  cpy = Math::getMin( pLength, length );
	Buffer	cur = packed.toBuffer();
	Buffer	res = cur( 0, cpy * format->getTotalSize( true ) );

	return new Block( format, res );

}

Sound::BlockCPtr Sound::Block::getMid(
	const	Uint32		pOffset,
	const	Uint32		pLength ) const {

	moveToPacked();

	Uint32  cpy;

	if ( pOffset >= length ) {
		cpy = 0;
	}
	else if ( pOffset + pLength >= length ) {
		cpy = length - pOffset;
	}
	else {
		cpy = pLength;
	}

	Buffer	cur = packed.toBuffer();
	Buffer	res = cur( pOffset * format->getTotalSize( true ), cpy * format->getTotalSize( true ) );

	return new Block( format, res );

}

// ============================================================================

Sound::BlockCPtr Sound::Block::cut(
	const	Uint32		pLength ) {

	moveToPacked();

	Uint32  cpy = Math::getMin( pLength, length );
	Buffer	cur = packed.toBuffer();
	Buffer	res = cur( 0, cpy * format->getTotalSize( true ) );

	packed = Mem::Block( cur( cpy * format->getTotalSize( true ) ) );

	length -= cpy;

	return new Block( format, res );

}

void Sound::Block::add(
		BlockCPtr	pBlock ) {

	if ( format != pBlock->getFormat() ) {
		throw InvalidArgument();
	}

	moveToPacked();
	pBlock->moveToPacked();

	Buffer	res = packed.toBuffer() + pBlock->packed.toBuffer();

	packed = Mem::Block( res );

	length += pBlock->length;

}

// ============================================================================

void Sound::Block::moveToPacked() const {

	if ( ! length ) {
		planar = false;
		return;
	}

	Uint32	nbrChans = format->getNbrChannels();
	Uint32	smplSize = format->getSampleSize( true );

	ensurePacked();

	if ( planar ) {
		Mem::Writer wr( packed, smplSize );
		for ( Uint32 i = 0 ; i < blocks.getSize() ; i++ ) {
			if ( blocks[ i ].getLength() ) {
				wr.copyDataFrom(
					Mem::Reader( blocks[ i ], smplSize ),
					i, nbrChans,
					0, 1 );
			}
		}
		planar = false;
		blocks.kill();
	}

}

void Sound::Block::moveToPlanar() const {

	if ( ! planar ) {

		Uint32	nbrChans = format->getNbrChannels();
		Uint32	smplSize = format->getSampleSize( true );
		Uint32	blckSize = length * format->getSampleSize( true );

		blocks.resize( nbrChans );

		if ( packed.getLength() ) {
			Mem::Reader rd( packed, smplSize );
			for ( Uint32 i = 0 ; i < nbrChans ; i++ ) {
				blocks[ i ] = Mem::Block( blckSize );
				Mem::Writer wr( blocks[ i ], smplSize );
				wr.copyDataFrom(
					rd,
					0, 1,
					i, nbrChans );
			}
		}

		planar = true;
		packed.kill();

	}

}

// ============================================================================

void Sound::Block::ensurePacked() const {

	if ( length && ! packed.getLength() ) {
		Uint32	smplSize = format->getSampleSize( true );
		Uint32	packSize = length * format->getTotalSize( true );
		packed = Mem::Block( packSize );
		Mem::Writer wr( packed, smplSize );
		wr.clearAll();
	}

}

void Sound::Block::ensureBlock(
	const	Uint32		pIndex ) const {

	if ( length && ! blocks[ pIndex ].getLength() ) {
		Uint32	smplSize = format->getSampleSize( true );
		Uint32	blckSize = length * smplSize;
		blocks[ pIndex ] = Mem::Block( blckSize );
		Mem::Writer wr( blocks[ pIndex ], smplSize );
		wr.clearAll();
	}

}

// ============================================================================

