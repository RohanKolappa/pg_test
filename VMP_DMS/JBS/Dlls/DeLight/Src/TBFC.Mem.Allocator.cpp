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
//	TBFC.Mem.Allocator.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <iostream>

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.FastCSLocker.h"
#include "TBFC.Base.System.h"
#include "TBFC.Math.Utils.h"

#include "TBFC.Mem.Allocator.h"

// ============================================================================

using namespace std;
using namespace TBFC;

// ============================================================================
// ============================================================================

Mem::Allocator::Allocator(
	const	Bool		__retrieve ) :

	retrieve( __retrieve ),
	totAlloc( 0 ),
	curAlloc( 0 ) {

}

Mem::Allocator::~Allocator() {

//	cerr << "~Allocator(): total allocated: " << totAlloc << endl;

	if ( curAlloc ) {
		cerr << "~Allocator(): still allocated: " << curAlloc << endl;
	}

}

// ============================================================================

Mem::AllocatorPtr Mem::Allocator::chooseBest(
	const	Uint32		/* __len */ ) {

// FIXME: be smarter ;-)

	return StdCAllocator::instance( true );

}

Mem::AllocatorPtr Mem::Allocator::retrieveAllocator(
	const	Uchar		* __ptr ) {

	const Uchar * hdrPtr = __ptr - 16;

	if ( * ( const Uint32 * )( hdrPtr ) != getMagicString() ) {
		throw InternalError( "Mem::Allocator::retrieveAllocator(): not one of us!" );
	}

	return * ( Allocator ** )( hdrPtr + 8 );

}

Uint32 Mem::Allocator::retrieveLength(
	const	Uchar		* __ptr ) {

	const Uchar * hdrPtr = __ptr - 16;

	if ( * ( const Uint32 * )( hdrPtr ) != getMagicString() ) {
		throw InternalError( "Mem::Allocator::retrieveAllocator(): not one of us!" );
	}

	return * ( const Uint32 * )( hdrPtr + 4 ) - 16;

}

// ============================================================================

void Mem::Allocator::createBlock(
	const	Uint32		reqLength,
		Uchar * &	phyVarPtr,
		Uint32 &	phyLength ) {

	phyVarPtr = 0;
	phyLength = 0;

	if ( ! reqLength ) {
		return;
	}

	Uint32	toAsk = reqLength;

	if ( retrieve ) {
		toAsk += 16;
	}

	if ( toAsk < 256 ) {
		toAsk = 256;
	}

	Uchar *	newPhyVarPtr;
	Uint32	newPhyLength;

	implCreateBlock( toAsk, newPhyVarPtr, newPhyLength );

	addAllocated( newPhyLength );

	phyVarPtr = newPhyVarPtr;
	phyLength = newPhyLength;

	if ( retrieve ) {
		* ( Uint32     * )( newPhyVarPtr     ) = getMagicString();
		* ( Uint32     * )( newPhyVarPtr + 4 ) = phyLength;
		* ( Allocator ** )( newPhyVarPtr + 8 ) = this;
		phyVarPtr += 16;
		phyLength -= 16;
	}

}

void Mem::Allocator::resizeBlock(
	const	Uint32		reqLength,
		Uchar * &	phyVarPtr,
		Uint32 &	phyLength ) {

	if ( ! reqLength ) {
		deleteBlock( phyVarPtr, phyLength );
		phyVarPtr = 0;
		phyLength = 0;
		return;
	}

	if ( ! phyLength ) {
		createBlock( reqLength, phyVarPtr, phyLength );
		return;
	}

	if ( reqLength <= phyLength ) {
		return;
	}

	Uint32	toAsk = reqLength;

	if ( retrieve ) {
		toAsk += 16;
	}

	if ( toAsk < 256 ) {
		toAsk = 256;
	}

	Uchar *	oldPhyVarPtr = phyVarPtr;
	Uint32	oldPhyLength = phyLength;

	if ( retrieve ) {
		oldPhyVarPtr -= 16;
		oldPhyLength += 16;
		if ( * ( const Uint32     * )( oldPhyVarPtr     ) != getMagicString() ) {
			throw InternalError( "Mem::Allocator::resizeBlock(): not one of us!" );
		}
		if ( * ( const Uint32     * )( oldPhyVarPtr + 4 ) != oldPhyLength ) {
			throw InternalError( "Mem::Allocator::resizeBlock(): lengths diverge!" );
		}
		if ( * ( const Allocator ** )( oldPhyVarPtr + 8 ) != this ) {
			throw InternalError( "Mem::Allocator::resizeBlock(): not one of mine!" );
		}
	}

	Uchar *	newPhyVarPtr = oldPhyVarPtr;
	Uint32	newPhyLength = oldPhyLength;

	implResizeBlock( toAsk, newPhyVarPtr, newPhyLength );

	addAllocated( newPhyLength - oldPhyLength );

	phyVarPtr = newPhyVarPtr;
	phyLength = newPhyLength;

	if ( retrieve ) {
		* ( Uint32     * )( newPhyVarPtr     ) = getMagicString();
		* ( Uint32     * )( newPhyVarPtr + 4 ) = newPhyLength;
		* ( Allocator ** )( newPhyVarPtr + 8 ) = this;
		phyVarPtr += 16;
		phyLength -= 16;
	}

}

void Mem::Allocator::deleteBlock(
		Uchar *		phyVarPtr,
		Uint32		phyLength ) {

	if ( ! phyLength ) {
		return;
	}

	if ( retrieve ) {
		phyVarPtr -= 16;
		phyLength += 16;
		if ( * ( const Uint32     * )( phyVarPtr     ) != getMagicString() ) {
			throw InternalError( "Mem::Allocator::deleteBlock(): not one of us!" );
		}
		if ( * ( const Uint32     * )( phyVarPtr + 4 ) != phyLength ) {
			throw InternalError(
				"Mem::Allocator::deleteBlock(): lengths diverge! mem: "
				+ Buffer( * ( const Uint32 * )( phyVarPtr + 4 ) )
				+ ", claimed: "
				+ Buffer( phyLength ) );
		}
		if ( * ( const Allocator ** )( phyVarPtr + 8 ) != this ) {
			throw InternalError( "Mem::Allocator::deleteBlock(): not one of mine!" );
		}
	}

	implDeleteBlock( phyVarPtr, phyLength );

	delAllocated( phyLength );

}

// ============================================================================

Uint64 Mem::Allocator::getTotalAllocated() const {

	FastCSLocker locker( allocCrt );

	return totAlloc;

}

Uint64 Mem::Allocator::getCurrentAllocated() const {

	FastCSLocker locker( allocCrt );

	return curAlloc;

}

// ============================================================================

void Mem::Allocator::addAllocated(
	const	Uint32		length ) {

	FastCSLocker locker( allocCrt );

	totAlloc += ( Uint64 )length;
	curAlloc += ( Uint64 )length;

}

void Mem::Allocator::delAllocated(
	const	Uint32		length ) {

	FastCSLocker locker( allocCrt );

	curAlloc -= ( Uint64 )length;

}

// ============================================================================
// ============================================================================

Mem::StdCAllocator::StdCAllocator(
	const	Bool		__retrieve ) :

	Allocator( __retrieve ) {

}

Mem::StdCAllocator::~StdCAllocator() {



}

// ============================================================================

Mem::AllocatorPtr Mem::StdCAllocator::instance(
	const	Bool		__retrieve ) {

	static AllocatorPtr retT = new StdCAllocator(  true );
	static AllocatorPtr retF = new StdCAllocator( false );

	return ( __retrieve ? retT : retF );

}

// ============================================================================

void Mem::StdCAllocator::implCreateBlock(
	const	Uint32		newLen,
		Uchar		* & ptr,
		Uint32		& phy ) {

	if ( ! ( ptr = System::malloc( ( phy = newLen ) ) ) ) {
		throw OutOfMemory();
	}

}

void Mem::StdCAllocator::implResizeBlock(
	const	Uint32		newLen,
		Uchar		* & ptr,
		Uint32		& phy ) {

	if ( ! ( ptr = System::realloc( ptr, newLen ) ) ) {
		throw OutOfMemory();
	}

	phy = newLen;

}

void Mem::StdCAllocator::implDeleteBlock(
		Uchar		* ptr,
	const	Uint32		/* phy */ ) {

	System::free( ptr );

}

// ============================================================================
// ============================================================================

Mem::StdCppAllocator::StdCppAllocator(
	const	Bool		__retrieve ) :

	Allocator( __retrieve ) {

}

Mem::StdCppAllocator::~StdCppAllocator() {



}

// ============================================================================

Mem::AllocatorPtr Mem::StdCppAllocator::instance(
	const	Bool		__retrieve ) {

	static AllocatorPtr retT = new StdCppAllocator(  true );
	static AllocatorPtr retF = new StdCppAllocator( false );

	return ( __retrieve ? retT : retF );

}

// ============================================================================

void Mem::StdCppAllocator::implCreateBlock(
	const	Uint32		newLen,
		Uchar		* & ptr,
		Uint32		& phy ) {

	if ( ! ( ptr = new Uchar[ ( phy = newLen ) ] ) ) {
		throw OutOfMemory();
	}

}

void Mem::StdCppAllocator::implResizeBlock(
	const	Uint32		newLen,
		Uchar		* & ptr,
		Uint32		& phy ) {

	Uchar	* newPtr;

	if ( ! ( newPtr = new Uchar[ newLen ] ) ) {
		throw OutOfMemory();
	}

	System::memcpy(
		newPtr,
		ptr,
		phy );

	delete[] ptr;

	ptr = newPtr;
	phy = newLen;

}

void Mem::StdCppAllocator::implDeleteBlock(
		Uchar		* ptr,
	const	Uint32		/* phy */ ) {

	delete[] ptr;

}

// ============================================================================
// ============================================================================

