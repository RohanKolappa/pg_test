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
// WITHOUmEltLength ANY WARRANTY; without even the implied warranty of
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
//	BFC.Mem.Reader.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Mem.Reader.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::Reader::Reader() {

	initBlock(
		0,
		1 );
	setDimensions(
		0,
		1 );

}

Mem::Reader::Reader(
	const	Block		& block,
	const	Uint32		eltLength ) {

	initBlock(
		block.branchPtr,
		eltLength );
	setDimensions(
		mBlkLength / eltLength,
		1 );

}

Mem::Reader::Reader(
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

Mem::Reader::Reader(
	const	Reader		& other ) {

	initBlock(
		other.mBranchPtr,
		other.mEltLength );
	setDimensions(
		other.mNbrOfCols,
		other.mNbrOfRows );

}

Mem::Reader::Reader(
	const	Reader		& other,
	const	Uint32		eltLength ) {

	initBlock(
		other.mBranchPtr,
		eltLength );

	if ( ( other.mNbrOfCols * other.mEltLength ) % eltLength ) {
		throw InvalidArgument( "Mem::Reader( Reader ): misaligned!" );
	}

	setDimensions(
		other.mNbrOfCols * other.mEltLength / eltLength,
		other.mNbrOfRows );

}

// ============================================================================

Mem::Reader::~Reader() {

	if ( mBranchPtr && ! mBranchPtr->delReader() ) {
		delete mBranchPtr;
	}

}

// ============================================================================

Mem::Reader & Mem::Reader::operator = (
	const	Reader		& other ) {

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

	if ( oldBranchPtr && ! oldBranchPtr->delReader() ) {
		delete oldBranchPtr;
	}

	return * this;

}

// ============================================================================

void Mem::Reader::setDimensions(
	const	Uint32		nbrOfCols,
	const	Uint32		nbrOfRows ) {

	if ( ! nbrOfRows ) {
		throw InvalidArgument(
			"Mem::Reader::setDimensions(): #rows == 0!" );
	}

	if ( mBlkLength % nbrOfRows ) {
		throw InvalidArgument(
			"Mem::Reader::setDimensions(blk len: "
			+ Buffer( mBlkLength ) + ", nbr rows: "
			+ Buffer( nbrOfRows ) + ")" );
	}

	Uint32	newRowLength = mBlkLength / nbrOfRows;

	if ( nbrOfCols * mEltLength > newRowLength ) {
		throw InvalidArgument(
			"Mem::Reader::setDimensions(str len: "
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

Bool Mem::Reader::operator == (
	const	Reader &	pOther ) const {

	if ( mNbrOfCols != pOther.mNbrOfCols
	  || mNbrOfRows != pOther.mNbrOfRows
	  || mEltLength != pOther.mEltLength ) {
		return false;
	}

	if ( mBranchPtr == pOther.mBranchPtr
	  && mBlkLength == pOther.mBlkLength
	  && mRowLength == pOther.mRowLength ) {
		return true;
	}

	Uint32 len = mRowLength - mRowPaddng;

	for ( Uint32 y = 0 ; y < mNbrOfRows ; y++ ) {
		if ( System::memcmp(
				( const Uchar * )getRowAddress( y ),
				( const Uchar * )pOther.getRowAddress( y ),
				len ) ) {
			return false;
		}
	}

	return true;

}

// ============================================================================

void Mem::Reader::gotoElement(
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
	mCursorPtr = mRowMinPtr + x * mEltLength;
	mCursorIdx = newCursorIdx;

}

void Mem::Reader::gotoElement(
	const	Uint32		index ) {

	if ( index > mNbrOfElts ) {
		throw OutOfRange();
	}

	if ( mNbrOfCols ) {
		Uint32	i = index % mNbrOfCols;
		Uint32	j = index / mNbrOfCols;
		mRowMinPtr = mBlkMinPtr + j * mRowLength;
		mRowMaxPtr = mRowMinPtr + mNbrOfCols * mEltLength;
		mCursorPtr = mRowMinPtr + i * mEltLength;
		mCursorIdx = index;
	}
	else {
		mRowMinPtr = mBlkMinPtr;
		mRowMaxPtr = mRowMinPtr;
		mCursorPtr = mRowMinPtr;
		mCursorIdx = 0;
	}

}

// ============================================================================

void Mem::Reader::doCopy(
	const	Reader		& other,
	const	Uint32		eltLength ) {

	if ( ! eltLength ) {
		throw InternalError( "Mem::Reader::doCopy(): elt len == 0!" );
	}

	Uint32	pldLength = other.mNbrOfCols * other.mEltLength;

	if ( pldLength % eltLength ) {
		throw InvalidArgument( "Mem::Reader::doCopy(): misaligned!" );
	}

	Branch * oldBranchPtr = mBranchPtr;

	initBlock( other.mBranchPtr, eltLength );
	setDimensions( pldLength / eltLength, other.mNbrOfRows );

	if ( oldBranchPtr && ! oldBranchPtr->delReader() ) {
		delete oldBranchPtr;
	}

}

// ============================================================================

void Mem::Reader::initBlock(
		Branch		* branchPtr,
	const	Uint32		eltLength ) {

	mBranchPtr = 0;

	if ( ! eltLength ) {
		// Let's throw exceptions in ctors :-) Really funny to debug!
		// Beter that than segfaults or unexpected behaviour!
		throw InvalidArgument( "Mem::Reader::init() NULL element length!" );
	}

	mBranchPtr = branchPtr;
	mEltLength = eltLength;

	if ( mBranchPtr ) {
		mBranchPtr->addReader();
		mBlkLength = mBranchPtr->getLogLength();
		mBlkMinPtr = mBranchPtr->getLogCstPtr();
	}
	else {
		mBlkLength = 0;
		mBlkMinPtr = 0;
	}

}

// ============================================================================

