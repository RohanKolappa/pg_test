// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Mem.Writer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Mem.Writer.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

Mem::Writer::Writer() {

	initBlock(
		0,
		1 );
	setDimensions(
		0,
		1 );

}

Mem::Writer::Writer(
	const	Block		& block,
	const	Uint32		eltLength ) {

	initBlock(
		block.branchPtr,
		eltLength );
	setDimensions(
		mBlkLength / eltLength,
		1 );

}

Mem::Writer::Writer(
	const	Block		& block,
	const	Uint32		eltLength,
	const	Uint32		nbrOfCols,
	const	Uint32		nbrOfRows ) {

	initBlock(
		block.branchPtr,
		eltLength );
	setDimensions(
		nbrOfCols,
		nbrOfRows );

}

Mem::Writer::Writer(
	const	Writer		& other ) {

	initBlock(
		other.mBranchPtr,
		other.mEltLength );
	setDimensions(
		other.mNbrOfCols,
		other.mNbrOfRows );

}

Mem::Writer::Writer(
	const	Writer		& other,
	const	Uint32		eltLength ) {

	initBlock(
		other.mBranchPtr,
		eltLength );

	if ( ( other.mNbrOfCols * other.mEltLength ) % eltLength ) {
		throw InvalidArgument( "Mem::Writer( Writer ): misaligned!" );
	}

	setDimensions(
		other.mNbrOfCols * other.mEltLength / eltLength,
		other.mNbrOfRows );

}

// ============================================================================

Mem::Writer::~Writer() {

	if ( mBranchPtr && ! mBranchPtr->delWriter() ) {
		delete mBranchPtr;
	}

}

// ============================================================================

Mem::Writer & Mem::Writer::operator = (
	const	Writer		& other ) {

	if ( this == & other ) {
		return * this;
	}

	Branch *	oldBranchPtr = mBranchPtr;

	initBlock(
		other.mBranchPtr,
		other.mEltLength );
	setDimensions(
		other.mNbrOfCols,
		other.mNbrOfRows );

	if ( oldBranchPtr && ! oldBranchPtr->delWriter() ) {
		delete oldBranchPtr;
	}

	return * this;

}

// ============================================================================

void Mem::Writer::setDimensions(
	const	Uint32		nbrOfCols,
	const	Uint32		nbrOfRows ) {

	if ( ! nbrOfRows ) {
		throw InvalidArgument( "Mem::Writer::setDimensions(): #rows == 0!" );
	}

	if ( mBlkLength % nbrOfRows ) {
		throw InvalidArgument( "Mem::Writer::setDimensions(blk len: "
			+ Buffer( mBlkLength ) + ", nbr rows: "
			+ Buffer( nbrOfRows ) + ")" );
	}

	Uint32	newRowLength = mBlkLength / nbrOfRows;

	if ( nbrOfCols * mEltLength > newRowLength ) {
		throw InvalidArgument( "Mem::Writer::setDimensions(str len: "
			+ Buffer( newRowLength ) + ", elt len: "
			+ Buffer( mEltLength ) + ", nbr cols: "
			+ Buffer( nbrOfCols ) );
	}

	mNbrOfRows = nbrOfRows;
	mNbrOfCols = nbrOfCols;
	mNbrOfElts = mNbrOfCols * mNbrOfRows;
	mRowLength = newRowLength;
	mRowPaddng = mRowLength - mNbrOfCols * mEltLength;

	gotoElement( 0, 0 );

}

// ============================================================================

void Mem::Writer::gotoElement(
	const	Uint32		x,
	const	Uint32		y ) {

	Uint32	newCursorIdx = x + y * mNbrOfCols;

	if ( x > mNbrOfCols
	  || y > mNbrOfRows
	  || newCursorIdx > mNbrOfElts ) {
		throw OutOfRange();
	}

	mRowMinPtr = mBlkMinPtr + y * mRowLength;
	mRowMaxPtr = mRowMinPtr + mNbrOfCols * mEltLength;
	mCursorPtr = mBlkMinPtr + y * mRowLength + x * mEltLength;
	mCursorIdx = newCursorIdx;

}

void Mem::Writer::gotoElement(
	const	Uint32		index ) {

	if ( index > mNbrOfElts ) {
		throw OutOfRange();
	}

	if ( mNbrOfCols ) {
		Uint32	i = index % mNbrOfCols;
		Uint32	j = index / mNbrOfCols;
		mRowMinPtr = mBlkMinPtr + j * mRowLength;
		mRowMaxPtr = mRowMinPtr + mNbrOfCols * mEltLength;
		mCursorPtr = mBlkMinPtr + j * mRowLength + i * mEltLength;
		mCursorIdx = index;
	}
	else {
		mRowMinPtr = mBlkMinPtr;
		mRowMaxPtr = mRowMinPtr;
		mCursorPtr = mBlkMinPtr;
		mCursorIdx = 0;
	}

}

// ============================================================================

void Mem::Writer::clearAll() {

	System::bzero( mBlkMinPtr, mBlkLength );

}

void Mem::Writer::fillAll(
	const	Uint32		v ) {

// FIXME: optimize drastically (later) :-)

	if ( mEltLength == 1 ) {
		System::memset( mBlkMinPtr, ( Uchar )v, mBlkLength );
	}
	else if ( mEltLength == 2 ) {
		for ( Uint32 y = 0 ; y < mNbrOfRows ; y++ ) {
			// setup cursor
			Uint16 *ptr = ( Uint16 * )( mBlkMinPtr + y * mRowLength );
			for ( Uint32 x = mNbrOfCols ; x ; x-- ) {
				*ptr++ = ( Uchar )v;
			}
		}
	}
	else if ( mEltLength == 3 ) {
		for ( Uint32 y = 0 ; y < mNbrOfRows ; y++ ) {
			// setup cursor
			Uchar *ptr = mBlkMinPtr + y * mRowLength;
			for ( Uint32 x = mNbrOfCols ; x ; x-- ) {
// FIXME: Little endian only now!
				*ptr++ = ( Uchar )( v       );
				*ptr++ = ( Uchar )( v >>  8 );
				*ptr++ = ( Uchar )( v >> 16 );
			}
		}
	}
	else if ( mEltLength == 4 ) {
		for ( Uint32 y = 0 ; y < mNbrOfRows ; y++ ) {
			// setup cursor
			Uint32 *ptr = ( Uint32 * )( mBlkMinPtr + y * mRowLength );
			for ( Uint32 x = mNbrOfCols ; x ; x-- ) {
				*ptr++ = v;
			}
		}
	}
	else if ( mEltLength == 8 ) {
		for ( Uint32 y = 0 ; y < mNbrOfRows ; y++ ) {
			// setup cursor
			Uint64 *ptr = ( Uint64 * )( mBlkMinPtr + y * mRowLength );
			for ( Uint32 x = mNbrOfCols ; x ; x-- ) {
				*ptr++ = v;
			}
		}
	}
	else {
		throw InternalError( "Mem::Writer::fillAll: not implemented!" );
	}

}

// ============================================================================

void Mem::Writer::fillRectangle(
	const	Uint32		x,
	const	Uint32		y,
	const	Uint32		w,
	const	Uint32		h,
	const	Uchar		v ) {

	if ( mEltLength == 1 ) {
		if ( x + w > mNbrOfCols
		  || y + h > mNbrOfRows ) {
			throw InvalidArgument();
		}
		for ( Uint32 y2 = y ; y2 < y + h ; y2++ ) {
			System::memset(
				mBlkMinPtr + y2 * mRowLength + x,
				v,
				w );
		}
	}
	else {
		throw InternalError( "Mem::Writer::fillRectangle: not implemented!" );
	}

}

// ============================================================================

void Mem::Writer::copyDataFrom(
	const	Reader		& reader ) {

	if ( reader.getEltLength() != getEltLength() ) {
		throw InvalidArgument( "Writer::copyDataFrom(): != sizeof( T )" );
	}

	if ( reader.getNbrCols() != getNbrCols() ) {
		throw InvalidArgument( "Writer::copyDataFrom(): cols: this: "
			+ Buffer( getNbrCols() ) + ", from: "
			+ Buffer( reader.getNbrCols() ) );
	}

	if ( reader.getNbrRows() != getNbrRows() ) {
		throw InvalidArgument( "Writer::copyDataFrom(): != N" );
	}

//	cerr << "Block address: " << getBlockAddress() << endl;

	if ( getBlockLength() == reader.getBlockLength() ) {

//		cerr << "ACCELERATED COPY :-)" << endl;

		System::memcpy(
			( Uchar * )getBlockAddress(),
			( const Uchar * )reader.getBlockAddress(),
			getBlockLength() );

	}
	else {

//		cerr << "SLOOOOW COPY :-) "
//			<< "Writer: " << getBlockLength()
//			<< ", reader: " << reader.getBlockLength() << endl;

		Uint32 l = getNbrCols() * getEltLength();
		Uint32 n = getNbrRows();
	
		while ( n-- ) {
			System::memcpy(
				( Uchar * )getRowAddress( n ),
				( const Uchar * )reader.getRowAddress( n ),
				l );
		}

	}

}

void Mem::Writer::copyDataFrom(
	const	Reader &	pReader,
	const	Uint32		pDstOff,
	const	Uint32		pDstBlk,
	const	Uint32		pSrcOff,
	const	Uint32		pSrcBlk ) {

	if ( ! pDstBlk || ! pSrcBlk ) {
		throw InvalidArgument( "Block size must be != 0!" );
	}

	if ( pDstOff >= pDstBlk || pSrcOff >= pSrcBlk ) {
		throw InvalidArgument( "Offset out of block!" );
	}

	Uint32	eltLen = getEltLength();

	if ( pReader.getEltLength() != eltLen ) {
		throw InvalidArgument( "Writer::copyDataFrom(): != sizeof( T )" );
	}

	if ( getNbrRows() * pReader.getNbrRows() != 1 ) {
		throw InvalidArgument( "Writer::copyDataFrom(): rows != 1" );
	}

	Uint32	dstCol = getNbrCols();
	Uint32	srcCol = pReader.getNbrCols();

	if ( dstCol % pDstBlk || srcCol % pSrcBlk ) {
		throw InvalidArgument( "cols must be multple of block size!" );
	}

	Uint32	toCopy = dstCol / pDstBlk;

	if ( toCopy != srcCol / pSrcBlk ) {
		throw InvalidArgument( "# dst != # src" );
	}

	const Uchar *	srcTop = ( const Uchar * )pReader.getRowAddress( 0 );
	Uchar *		dstTop = ( Uchar * )getRowAddress( 0 );

	for ( Uint32 i = 0 ; i < toCopy ; i++ ) {
		const Uchar *	srcPtr = srcTop + ( i * pSrcBlk + pSrcOff ) * eltLen;
		Uchar *		dstPtr = dstTop + ( i * pDstBlk + pDstOff ) * eltLen;
		for ( Uint32 j = eltLen ; j ; j-- ) {
			*dstPtr++ = *srcPtr++;
		}
	}

}

void Mem::Writer::copyRectangle(
	const	Uint32		dx,
	const	Uint32		dy,
	const	Uint32		rw,
	const	Uint32		rh,
	const	Reader		& r,
	const	Uint32		sx,
	const	Uint32		sy,
	const	Bool		/* c */ ) {

	if ( getSizeOfElement() != r.getSizeOfElement() ) {
		throw NotImplemented( "Mem::Writer::copyRectangle()" );
	}

	if ( dx + rw > getNbrCols()
	  || dy + rh > getNbrRows()
	  || sx + rw > r.getNbrCols()
	  || sy + rh > r.getNbrRows() ) {
		throw InvalidArgument();
	}

	for ( Uint32 j = 0 ; j < rh ; j++ ) {
		System::memcpy(
			( Uchar * )getElementAddress( dx, dy + j ),
			( const Uchar * )r.getElementAddress( sx, sy + j ),
			rw * getSizeOfElement() );
	}

}

void Mem::Writer::copySegment(
	const	Uint32		dx,
	const	Uint32		dy,
	const	Uint32		sw,
	const	Reader		& r,
	const	Uint32		sx,
	const	Uint32		sy,
	const	Bool		/* c */ ) {

	if ( getSizeOfElement() != r.getSizeOfElement() ) {
		throw NotImplemented( "Mem::Writer::copySegment()" );
	}

	if ( dx + sw > getNbrCols()
	  || dy >= getNbrRows()
	  || sx + sw > r.getNbrCols()
	  || sy >= r.getNbrRows() ) {
		throw InvalidArgument();
	}

	System::memcpy(
		( Uchar * )getElementAddress( dx, dy ),
		( const Uchar * )r.getElementAddress( sx, sy ),
		sw * getSizeOfElement() );

}

// ============================================================================

void Mem::Writer::doCopy(
	const	Writer		& other,
	const	Uint32		eltLength ) {

	if ( ! eltLength ) {
		throw InternalError( "Mem::Writer::doCopy(): elt len == 0!" );
	}

	Uint32	pldLength = other.mNbrOfCols * other.mEltLength;

	if ( pldLength % eltLength ) {
		throw InvalidArgument( "Mem::Writer::doCopy(): misaligned!" );
	}

	Branch * oldBranchPtr = mBranchPtr;

	initBlock( other.mBranchPtr, eltLength );
	setDimensions( pldLength / eltLength, other.mNbrOfRows );

	if ( oldBranchPtr && ! oldBranchPtr->delReader() ) {
		delete oldBranchPtr;
	}

}

// ============================================================================

void Mem::Writer::initBlock(
		Branch *	branchPtr,
	const	Uint32		eltLength ) {

	mBranchPtr = 0;

	if ( ! eltLength ) {
		// Let's throw exceptions in ctors :-) Really funny to debug!
		// Better that than segfaults or unexpected behaviour!
		throw InvalidArgument( "Mem::Writer::init() NULL element length!" );
	}

	mBranchPtr = branchPtr;
	mEltLength = eltLength;

	if ( mBranchPtr ) {
		mBranchPtr->addWriter();
		mBlkLength = mBranchPtr->getLogLength();
		mBlkMinPtr = mBranchPtr->getLogVarPtr();
	}
	else {
		mBlkLength = 0;
		mBlkMinPtr = 0;
	}

}

// ============================================================================

