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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//			    Free Software Foundation
//			    59 Temple Place, Suite 330
//			    Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Mem.DataSharer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCSLocker.h"
#include "BFC.Base.System.h"

#include "BFC.Mem.Allocator.h"
#include "BFC.Mem.Data.h"
#include "BFC.Mem.DataProxy.h"
#include "BFC.Mem.DataSharer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Mem::DataSharer::DataSharer(
		Data *		pDataBlock ) {

	if ( ! pDataBlock ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = new DataProxy( pDataBlock );
		blkLength = dataProxy->getLogLength();
		logOffset = 0;
		logLength = dataProxy->getLogLength();
		logCstPtr = dataProxy->getPhyCstPtr();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	pOther ) {

	FastCSLocker	locker( pOther.criticalS );

	if ( ! pOther.dataProxy ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = pOther.dataProxy;
		blkLength = pOther.blkLength;
		logOffset = pOther.logOffset;
		logLength = pOther.logLength;
		logCstPtr = pOther.logCstPtr;
		dataProxy->incCounter();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	pOther,
	const	Uint32		pBeg ) {

	FastCSLocker	locker( pOther.criticalS );

	if ( ! pOther.dataProxy
	  || pBeg >= pOther.logLength ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = pOther.dataProxy;
		blkLength = pOther.blkLength;
		logOffset = pOther.logOffset + pBeg;
		logLength = pOther.logLength - pBeg;
		logCstPtr = pOther.logCstPtr + pBeg;
		dataProxy->incCounter();
	}

}

Mem::DataSharer::DataSharer(
	const	DataSharer &	pOther,
	const	Uint32		pBeg,
	const	Uint32		pLen ) {

	FastCSLocker	locker( pOther.criticalS );

	if ( ! pOther.dataProxy
	  || pBeg >= pOther.logLength ) {
		dataProxy = 0;
		blkLength = 0;
		logOffset = 0;
		logLength = 0;
		logCstPtr = 0;
	}
	else {
		dataProxy = pOther.dataProxy;
		blkLength = pOther.blkLength;
		logOffset = pOther.logOffset + pBeg;
		if ( pBeg + pLen >= pOther.logLength ) {
			logLength = pOther.logLength - pBeg;
		}
		else {
			logLength = pLen;
		}
		logCstPtr = pOther.logCstPtr + pBeg;
		dataProxy->incCounter();
	}

}

// ============================================================================

Mem::DataSharer & Mem::DataSharer::operator = (
	const	DataSharer &	other ) {

	if ( this == & other ) {
		return * this;
	}

	DataProxy *	newDataProxy;
	Uint32		newBlkLength;
	Uint32		newLogOffset;
	Uint32		newLogLength;
	const Uchar *	newLogCstPtr;

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

	DataProxy *	oldDataProxy;

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

void Mem::DataSharer::setLogOffset(
	const	Uint32		pNewLogOffset ) {

	FastCSLocker	locker( criticalS );

	if ( pNewLogOffset <= logOffset ) {
		Uint32 toAdd = logOffset - pNewLogOffset;
		logOffset = pNewLogOffset;
		logLength += toAdd;
		logCstPtr -= toAdd;
	}
	else {
		Uint32 toDel = pNewLogOffset - logOffset;
		if ( toDel > logLength ) {
			toDel = logLength;
		}
		logOffset = pNewLogOffset;
		logLength -= toDel;
		logCstPtr += toDel;
	}

}

void Mem::DataSharer::setLogLength(
	const	Uint32		pNewLogLength ) {

	if ( ! pNewLogLength ) {
		deleteData();
		return;
	}

	FastCSLocker	locker( criticalS );

	if ( pNewLogLength <= logLength ) {
		logLength = pNewLogLength;
		return;
	}

	if ( logOffset + pNewLogLength <= blkLength ) {
		logLength = pNewLogLength;
		return;
	}

	// Here, our simple tricks do not work anymore :-(

	if ( dataProxy ) {
		dataProxy = dataProxy->memmove(
				0, logOffset, logLength, pNewLogLength );
	}
	else {
		dataProxy = new DataProxy( new Data( pNewLogLength ) );
	}

	blkLength = dataProxy->getLogLength();
	logOffset = 0;
	logLength = pNewLogLength;
	logCstPtr = dataProxy->getPhyCstPtr();

}

// ============================================================================

Uchar * Mem::DataSharer::getLogVarPtr() {

	FastCSLocker	locker( criticalS );

	if ( ! dataProxy ) {
		return 0;
	}

	dataProxy = dataProxy->ensureSingleUser();
	blkLength = dataProxy->getLogLength();
	logCstPtr = dataProxy->getPhyCstPtr() + logOffset;

	return ( dataProxy->getPhyVarPtr() + logOffset );

}

// ============================================================================

void Mem::DataSharer::setBlkLength(
	const	Uint32		pMinBlkLength ) {

	FastCSLocker	locker( criticalS );

	if ( pMinBlkLength <= blkLength ) {
		return;
	}

	if ( dataProxy ) {
		dataProxy = dataProxy->memmove(
				0, logOffset, logLength, pMinBlkLength );
		logOffset = 0;
	}
	else {
		dataProxy = new DataProxy( new Data( pMinBlkLength ) );
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
		Data *		pDataBlock,
	const	Uint32		pBeg,
	const	Uint32		pLen ) {

	DataProxy *	oldDataProxy;

	{
		FastCSLocker	locker( criticalS );
		oldDataProxy = dataProxy;
		if ( ! pDataBlock
		  || pBeg >= pDataBlock->getLogLength() ) {
			dataProxy = 0;
			blkLength = 0;
			logOffset = 0;
			logLength = 0;
			logCstPtr = 0;
			if ( pDataBlock ) {
				delete pDataBlock;
			}
		}
		else {
			dataProxy = new DataProxy( pDataBlock );
			blkLength = dataProxy->getLogLength();
			logOffset = pBeg;
			if ( pBeg + pLen >= blkLength ) {
				logLength = blkLength - pBeg;
			}
			else {
				logLength = pLen;
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

	DataProxy *	oldDataProxy;

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
		Uchar * &	pPhyVarPtr,
		Uint32 &	pPhyLength,
	const	Bool		pRetrievable ) {

	FastCSLocker	locker( criticalS );
	AllocatorPtr	res;

	if ( dataProxy->getCounter() >= 2
	  || logOffset ) {
		// Copy mode!
		res = Allocator::chooseBest( logLength );
		res->createBlock(
			logLength,
			pPhyVarPtr,
			pPhyLength );
		System::memcpy(
			pPhyVarPtr,
			logCstPtr,
			logLength );
	}
	else {
		// Transfer mode!
		res = dataProxy->releaseData(
			pPhyVarPtr,
			pPhyLength,
			pRetrievable );
	}

	return res;

}

// ============================================================================

void Mem::DataSharer::genOutOfRange(
	const	Uint32		pOffset ) const {

	throw OutOfRange( "DataSharer: requested: "
		+ Buffer( pOffset )
		+ ", but length: "
		+ Buffer( logLength ) );

}

// ============================================================================

