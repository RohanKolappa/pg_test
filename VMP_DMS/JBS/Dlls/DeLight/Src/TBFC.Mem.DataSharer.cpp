// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC".
// 
// "TBFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC"; if not, write to:
//			    Free Software Foundation
//			    59 Temple Place, Suite 330
//			    Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Mem.DataSharer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.Exception.h"
#include "TBFC.Base.FastCSLocker.h"
#include "TBFC.Base.System.h"

#include "TBFC.Mem.Allocator.h"
#include "TBFC.Mem.Data.h"
#include "TBFC.Mem.DataProxy.h"
#include "TBFC.Mem.DataSharer.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Mem::DataSharer::DataSharer(
		Data *		__dataBlock ) {

	if ( ! __dataBlock ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = new DataProxy( __dataBlock );
		blkLength = dataProxy->getLogLength();
		logOffset = 0;
		logLength = dataProxy->getLogLength();
		logCstPtr = dataProxy->getPhyCstPtr();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	__other ) {

	FastCSLocker	locker( __other.criticalS );

	if ( ! __other.dataProxy ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = __other.dataProxy;
		blkLength = __other.blkLength;
		logOffset = __other.logOffset;
		logLength = __other.logLength;
		logCstPtr = __other.logCstPtr;
		dataProxy->incCounter();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	__other,
	const	Uint32		__beg ) {

	FastCSLocker	locker( __other.criticalS );

	if ( ! __other.dataProxy
	  || __beg >= __other.logLength ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = __other.dataProxy;
		blkLength = __other.blkLength;
		logOffset = __other.logOffset + __beg;
		logLength = __other.logLength - __beg;
		logCstPtr = __other.logCstPtr + __beg;
		dataProxy->incCounter();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	__other,
	const	Uint32		__beg,
	const	Uint32		__len ) {

	FastCSLocker	locker( __other.criticalS );

	if ( ! __other.dataProxy
	  || __beg >= __other.logLength ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = __other.dataProxy;
		blkLength = __other.blkLength;
		logOffset = __other.logOffset + __beg;
		if ( __beg + __len >= __other.logLength ) {
			logLength = __other.logLength - __beg;
		}
		else {
			logLength = __len;
		}
		logCstPtr = __other.logCstPtr + __beg;
		dataProxy->incCounter();
	}

}

// ============================================================================

Mem::DataSharer & Mem::DataSharer::operator = (
	const	DataSharer &	other ) {

	if ( this == & other ) {
		return * this;
	}

	DataProxy	* newDataProxy;
	Uint32		newBlkLength;
	Uint32		newLogOffset;
	Uint32		newLogLength;
	const Uchar	* newLogCstPtr;

	{
		FastCSLocker	locker( other.criticalS );
		newDataProxy = other.dataProxy;
		newBlkLength = other.blkLength;
		newLogOffset = other.logOffset;
		newLogLength = other.logLength;
		newLogCstPtr = other.logCstPtr;
		if ( newDataProxy ) {
			newDataProxy->incCounter();
		}
	}

	DataProxy	* oldDataProxy;

	{
		FastCSLocker	locker( criticalS );
		oldDataProxy = dataProxy;
		dataProxy = newDataProxy;
		blkLength = newBlkLength;
		logOffset = newLogOffset;
		logLength = newLogLength;
		logCstPtr = newLogCstPtr;
	}

	if ( oldDataProxy && ! oldDataProxy->decCounter() ) {
		delete oldDataProxy;
	}

	return * this;

}

// ============================================================================

void Mem::DataSharer::setLogLength(
	const	Uint32		__newLogLength ) {

	if ( ! __newLogLength ) {
		deleteData();
		return;
	}

	FastCSLocker	locker( criticalS );

	if ( __newLogLength <= logLength ) {
		logLength = __newLogLength;
		return;
	}

	if ( logOffset + __newLogLength <= blkLength ) {
		logLength = __newLogLength;
		return;
	}

	// Here, our simple tricks do not work anymore :-(

	if ( dataProxy ) {
		dataProxy = dataProxy->memmove(
				0, logOffset, logLength, __newLogLength );
	}
	else {
		dataProxy = new DataProxy( new Data( __newLogLength ) );
	}

	blkLength = dataProxy->getLogLength();
	logOffset = 0;
	logLength = __newLogLength;
	logCstPtr = dataProxy->getPhyCstPtr();

}

// ============================================================================

Uchar * Mem::DataSharer::getLogVarPtr() {

	FastCSLocker	locker( criticalS );

	if ( dataProxy ) {
		dataProxy = dataProxy->ensureSingleUser();
		blkLength = dataProxy->getLogLength();
		logCstPtr = dataProxy->getPhyCstPtr() + logOffset;
		return dataProxy->getPhyVarPtr() + logOffset;
	}
	else {
		return 0;
	}

}

// ============================================================================

void Mem::DataSharer::setBlkLength(
	const	Uint32		__minBlkLength ) {

	FastCSLocker	locker( criticalS );

	if ( __minBlkLength <= blkLength ) {
		return;
	}

	if ( dataProxy ) {
		dataProxy = dataProxy->memmove(
				0, logOffset, logLength, __minBlkLength );
		logOffset = 0;
	}
	else {
		dataProxy = new DataProxy( new Data( __minBlkLength ) );
	}

	blkLength = dataProxy->getLogLength();
	logCstPtr = dataProxy->getPhyCstPtr();

}

// ============================================================================

Uint32 Mem::DataSharer::getPhyLength() const {

	return ( dataProxy ? dataProxy->getPhyLength() : 0 );

}

// ============================================================================

void Mem::DataSharer::attachData(
		Data		* __dataBlock,
	const	Uint32		__beg,
	const	Uint32		__len ) {

	DataProxy	* oldDataProxy;

	{
		FastCSLocker	locker( criticalS );
		oldDataProxy = dataProxy;
		if ( ! __dataBlock
		  || __beg >= __dataBlock->getLogLength() ) {
			dataProxy = 0;
			blkLength = 0;
			logOffset = 0;
			logLength = 0;
			logCstPtr = 0;
			if ( __dataBlock ) {
				delete __dataBlock;
			}
		}
		else {
			dataProxy = new DataProxy( __dataBlock );
			blkLength = dataProxy->getLogLength();
			logOffset = __beg;
			if ( __beg + __len >= blkLength ) {
				logLength = blkLength - __beg;
			}
			else {
				logLength = __len;
			}
			logCstPtr = dataProxy->getPhyCstPtr() + logOffset;
		}
	}

	if ( oldDataProxy && ! oldDataProxy->decCounter() ) {
		delete oldDataProxy;
	}

}

// ============================================================================

void Mem::DataSharer::deleteData() {

	DataProxy	* oldDataProxy;

	{
		FastCSLocker	locker( criticalS );
		oldDataProxy = dataProxy;
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}

	if ( oldDataProxy && ! oldDataProxy->decCounter() ) {
		delete oldDataProxy;
	}

}

// ============================================================================

Mem::AllocatorPtr Mem::DataSharer::releaseData(
		Uchar * &	__phyVarPtr,
		Uint32 &	__phyLength,
	const	Bool		__retrievable ) {

	FastCSLocker	locker( criticalS );
	AllocatorPtr	res;

	if ( dataProxy->getCounter() >= 2
	  || logOffset ) {
		// Copy mode!
		res = Allocator::chooseBest( logLength );
		res->createBlock(
			logLength,
			__phyVarPtr,
			__phyLength );
		System::memcpy(
			__phyVarPtr,
			logCstPtr,
			logLength );
	}
	else {
		// Transfer mode!
		res = dataProxy->releaseData(
			__phyVarPtr,
			__phyLength,
			__retrievable );
	}

	return res;

}

// ============================================================================

void Mem::DataSharer::genOutOfRange(
	const	Uint32		__offset ) const {

	throw OutOfRange( "DataSharer: requested: "
		+ Buffer( __offset )
		+ ", but length: "
		+ Buffer( logLength ) );

}

// ============================================================================

