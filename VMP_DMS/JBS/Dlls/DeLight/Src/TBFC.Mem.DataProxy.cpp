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
//	TBFC.Mem.DataProxy.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.FastCSLocker.h"
#include "TBFC.Base.System.h"

#include "TBFC.Math.Utils.h"

#include "TBFC.Mem.DataProxy.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Mem::DataProxy::~DataProxy() {

	if ( dataPtr ) {
		delete dataPtr;
	}

}

// ============================================================================

Mem::DataProxy * Mem::DataProxy::resize(
	const	Uint32		len ) {

	FastCSLocker locker( critSec );

	if ( counter >= 2 ) {
		--counter;
		DataProxy * res = new DataProxy( new Data( * dataPtr, len ) );
		return res;
	}
	else {
		dataPtr->resize( len );
		return this;
	}

}

// ============================================================================

Mem::DataProxy * Mem::DataProxy::memmove(
	const	Uint32		__dstOffset,
	const	Uint32		__srcOffset,
	const	Uint32		__srcLength,
	const	Uint32		__minLength ) {

	FastCSLocker locker( critSec );

	Uint32	minLength = Math::getMax(
					__minLength,
					__dstOffset + __srcLength );

	if ( counter >= 2 ) {
		// We have to use a cloned proxy!
		--counter;
		DataProxy * newProx = new DataProxy( new Data( minLength ) );
		if ( __srcLength ) {
			System::memcpy(
				newProx->getPhyVarPtr() + __dstOffset,
				dataPtr->getPhyCstPtr() + __srcOffset,
				__srcLength );
		}
		return newProx;
	}
	else {
		if ( minLength > dataPtr->getLogLength() ) {
			// We have to modify our block!
			dataPtr->resize( minLength );
		}
		if ( __dstOffset != __srcOffset && __srcLength ) {
			System::memmove(
				dataPtr->getPhyVarPtr() + __dstOffset,
				dataPtr->getPhyCstPtr() + __srcOffset,
				__srcLength );
		}
		return this;
	}

}

// ============================================================================

void Mem::DataProxy::incCounter() {

	FastCSLocker locker( critSec );

	++counter;

}

Uint32 Mem::DataProxy::decCounter() {

	FastCSLocker locker( critSec );

	return --counter;

}

// ============================================================================

Mem::DataProxy * Mem::DataProxy::ensureSingleUser() {

	FastCSLocker locker( critSec );

	DataProxy * res;

	if ( counter >= 2 ) {
		--counter;
		res = new DataProxy( new Data( * dataPtr ) );
	}
	else {
		dataPtr->forceWriteAccess();
		res = this;
	}

	return res;

}

Mem::DataProxy * Mem::DataProxy::ensureLocalCopy() {

	FastCSLocker locker( critSec );

	DataProxy * res;

	if ( dataPtr->hasWriteAccess() ) {
		res = this;
	}
	else if ( counter >= 2 ) {
		--counter;
		res = new DataProxy( new Data( * dataPtr ) );
	}
	else {
		dataPtr->forceWriteAccess();
		res = this;
	}

	return res;

}

// ============================================================================

Mem::AllocatorPtr Mem::DataProxy::releaseData(
		Uchar * &	__phyVarPtr,
		Uint32 &	__phyLength,
	const	Bool		__retrievable ) {

	FastCSLocker	locker( critSec );

	return dataPtr->releaseData( __phyVarPtr, __phyLength, __retrievable );

}

// ============================================================================

