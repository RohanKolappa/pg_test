// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Mem.Data.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.Math.Utils.h"

#include "BFC.Mem.Allocator.h"
#include "BFC.Mem.Data.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::Data::Data() :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ),
	allocator( 0 ) {

}

Mem::Data::Data(
	const	Uint32		pLen ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ),
	allocator( 0 ) {

	if ( pLen ) {
		allocator = Allocator::chooseBest( pLen );
		allocator->createBlock(
			pLen,
			phyVarPtr,
			phyLength );
		logLength = pLen;
		phyCstPtr = phyVarPtr;
	}

}

Mem::Data::Data(
	const	Uchar *		pCstPtr,
	const	Uint32		pLogLen,
	const	Uint32		pPhyLen,
	const	Bool		pDoCopy ) :

	logLength( pLogLen ),
	phyLength( pPhyLen ),
	phyCstPtr( pCstPtr ),
	phyVarPtr(       0 ),
	allocator(       0 ) {

	if ( pDoCopy ) {
		forceWriteAccess();
	}

}

Mem::Data::Data(
		Uchar *		pVarPtr,
	const	Uint32		pPhyLen,
		Allocator *	pAlloca ) :

	logLength( pPhyLen ),
	phyLength( pPhyLen ),
	phyCstPtr( pVarPtr ),
	phyVarPtr( pVarPtr ),
	allocator( pAlloca ) {

}

Mem::Data::Data(
	const	Data &		other ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ),
	allocator( 0 ) {

	if ( ! other.logLength ) {
		return;
	}

	if ( other.phyVarPtr ) {
		// We have to do a deep copy of the data!
		allocator = Allocator::chooseBest( other.logLength );
		allocator->createBlock(
			other.logLength,
			phyVarPtr,
			phyLength );
		System::memcpy(
			phyVarPtr,
			other.phyCstPtr,
			other.logLength );
		logLength = other.logLength;
		phyCstPtr = phyVarPtr;
	}
	else {
		// Other is using a read-only string!
		logLength = other.logLength;
		phyLength = other.phyLength;
		phyCstPtr = other.phyCstPtr;
	}

}

Mem::Data::Data(
	const	Data &		other,
	const	Uint32		newLength ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ),
	allocator( 0 ) {

	if ( ! newLength ) {
		return;
	}

	allocator = Allocator::chooseBest( newLength );

	allocator->createBlock(
		newLength,
		phyVarPtr,
		phyLength );

	logLength = newLength;
	phyCstPtr = phyVarPtr;

	if ( other.logLength ) {
		System::memcpy(
			phyVarPtr,
			other.phyCstPtr,
			Math::getMin( logLength, other.logLength ) );
	}

}

Mem::Data::Data(
	const	Uchar *		pSrc1,
	const	Uint32		pLen1,
	const	Uchar *		pSrc2,
	const	Uint32		pLen2 ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ),
	allocator( 0 ) {

	Uint32 newLength = pLen1 + pLen2;

	if ( ! newLength ) {
		return;
	}

	allocator = Allocator::chooseBest( newLength );

	allocator->createBlock(
		newLength,
		phyVarPtr,
		phyLength );

	logLength = newLength;
	phyCstPtr = phyVarPtr;

	if ( pLen1 ) {
		System::memcpy(
			phyVarPtr,
			pSrc1,
			pLen1 );
	}

	if ( pLen2 ) {
		System::memcpy(
			phyVarPtr + pLen1,
			pSrc2,
			pLen2 );
	}

}

// ============================================================================

Mem::Data::~Data() {

	kill();

}

// ============================================================================

void Mem::Data::resize(
	const	Uint32		newLength ) {

	if ( ! newLength ) {
		kill();
		return;
	}

	if ( newLength <= phyLength ) {
		logLength = newLength;
		return;
	}

	if ( allocator ) {
		allocator->resizeBlock(
			newLength,
			phyVarPtr,
			phyLength );
		logLength = newLength;
		phyCstPtr = phyVarPtr;
	}
	else {
		// We are maybe attached to some block, but we are sure this
		// was not allocated by us! We have to allocate our own copy!
		Uchar *		tmpVarPtr;
		Uint32		tmpLength;
		allocator = Allocator::chooseBest( newLength );
		allocator->createBlock(
			newLength,
			tmpVarPtr,
			tmpLength );
		if ( logLength ) {
			System::memcpy(
				tmpVarPtr,
				phyCstPtr,
				logLength );
		}
		logLength = newLength;
		phyLength = tmpLength;
		phyCstPtr = tmpVarPtr;
		phyVarPtr = tmpVarPtr;
	}

}

// ============================================================================

void Mem::Data::kill() {

	if ( phyLength ) {
		if ( allocator ) {
			allocator->deleteBlock( phyVarPtr, phyLength );
			allocator = 0;
		}
		logLength = 0;
		phyLength = 0;
		phyCstPtr = 0;
		phyVarPtr = 0;
	}

}

// ============================================================================

void Mem::Data::forceWriteAccess() {

	if ( logLength && ! phyVarPtr ) {
		Uchar *		tmpVarPtr;
		Uint32		tmpLength;
		allocator = Allocator::chooseBest( logLength );
		allocator->createBlock(
			logLength,
			tmpVarPtr,
			tmpLength );
		System::memcpy(
			tmpVarPtr,
			phyCstPtr,
			logLength );
		phyLength = tmpLength;
		phyCstPtr = tmpVarPtr;
		phyVarPtr = tmpVarPtr;
	}

}

// ============================================================================

Mem::Allocator * Mem::Data::releaseData(
		Uchar * &	pPhyVarPtr,
		Uint32 &	pPhyLength,
	const	Bool		pRetrievable ) {

	Allocator * res = 0;

	if ( ! logLength ) {
		// No content ? No output!
		pPhyVarPtr = 0;
		pPhyLength = 0;
	}
	else if ( ! allocator
	  || ( pRetrievable && ! allocator->isRetrievable() ) ) {
		// We have to make a copy here!
		res = Allocator::chooseBest( logLength );
		res->createBlock(
			logLength,
			pPhyVarPtr,
			pPhyLength );
		System::memcpy(
			pPhyVarPtr,
			phyCstPtr,
			logLength );
	}
	else {
		// Transfer :-)
		res = allocator;
		pPhyVarPtr = phyVarPtr;
		pPhyLength = phyLength;
		logLength = 0;
		phyLength = 0;
		phyCstPtr = 0;
		phyVarPtr = 0;
		allocator = 0;
	}

	return res;

}

// ============================================================================

