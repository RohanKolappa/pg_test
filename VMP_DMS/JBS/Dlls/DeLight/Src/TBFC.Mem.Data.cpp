// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Mem.Data.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.System.h"

#include "TBFC.Math.Utils.h"

#include "TBFC.Mem.Allocator.h"
#include "TBFC.Mem.Data.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Mem::Data::Data() :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ) {

}

Mem::Data::Data(
	const	Uint32		__len ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ) {

	if ( __len ) {
		allocator = Allocator::chooseBest( __len );
		allocator->createBlock(
			__len,
			phyVarPtr,
			phyLength );
		logLength = __len;
		phyCstPtr = phyVarPtr;
	}

}

Mem::Data::Data(
	const	Uchar *		__cstPtr,
	const	Uint32		__logLen,
	const	Uint32		__phyLen,
	const	Bool		__doCopy ) :

	logLength( __logLen ),
	phyLength( __phyLen ),
	phyCstPtr( __cstPtr ),
	phyVarPtr(        0 ) {

	if ( __doCopy ) {
		forceWriteAccess();
	}

}

Mem::Data::Data(
		Uchar *		__varPtr,
	const	Uint32		__phyLen,
		AllocatorPtr	__alloca ) :

	logLength( __phyLen ),
	phyLength( __phyLen ),
	phyCstPtr( __varPtr ),
	phyVarPtr( __varPtr ),
	allocator( __alloca ) {

}

Mem::Data::Data(
	const	Data &		other ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ) {

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
	phyVarPtr( 0 ) {

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
	const	Uchar *		__src1,
	const	Uint32		__len1,
	const	Uchar *		__src2,
	const	Uint32		__len2 ) :

	logLength( 0 ),
	phyLength( 0 ),
	phyCstPtr( 0 ),
	phyVarPtr( 0 ) {

	Uint32 newLength = __len1 + __len2;

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

	if ( __len1 ) {
		System::memcpy(
			phyVarPtr,
			__src1,
			__len1 );
	}

	if ( __len2 ) {
		System::memcpy(
			phyVarPtr + __len1,
			__src2,
			__len2 );
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
			allocator.kill();
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

Mem::AllocatorPtr Mem::Data::releaseData(
		Uchar *		& __phyVarPtr,
		Uint32		& __phyLength,
	const	Bool		__retrievable ) {

	AllocatorPtr res;

	if ( ! logLength ) {
		// No content ? No output!
		__phyVarPtr = 0;
		__phyLength = 0;
	}
	else if ( ! allocator
	  || ( __retrievable && ! allocator->isRetrievable() ) ) {
		// We have to make a copy here!
		res = Allocator::chooseBest( logLength );
		res->createBlock(
			logLength,
			__phyVarPtr,
			__phyLength );
		System::memcpy(
			__phyVarPtr,
			phyCstPtr,
			logLength );
	}
	else {
		// Transfer :-)
		res = allocator;
		__phyVarPtr = phyVarPtr;
		__phyLength = phyLength;
		logLength = 0;
		phyLength = 0;
		phyCstPtr = 0;
		phyVarPtr = 0;
		allocator.kill();
	}

	return res;

}

// ============================================================================

