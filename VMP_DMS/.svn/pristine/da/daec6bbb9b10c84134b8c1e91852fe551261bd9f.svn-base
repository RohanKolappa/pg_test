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
//	BFC.STL.SMap.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Comparable.h"

#include "BFC.STL.SMap.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( STL, SMap )

// ============================================================================

const SPtr & STL::SMap::operator [] (
	const	ComparableCPtr &
				pKey ) const {

	Uint32 pos;

	if ( ! lookup( pKey, pos ) ) {
		throw InvalidArgument();
	}

	return vals[ pos ];

}

SPtr & STL::SMap::operator [] (
	const	ComparableCPtr &
				pKey ) {

	Uint32 pos;

	if ( ! lookup( pKey, pos ) ) {
		throw InvalidArgument();
	}

	return vals[ pos ];

}

const SPtr & STL::SMap::get(
	const	ComparableCPtr &
				pKey,
	const	SPtr &		pDft ) const {

	Uint32 pos;

	return ( lookup( pKey, pos )
		? vals[ pos ]
		: pDft );

}

SPtr & STL::SMap::getOrAdd(
	const	ComparableCPtr &
				pKey ) {

	Uint32 pos;

	if ( ! lookup( pKey, pos ) ) {
		keys.insert( pos, pKey );
		vals.insert( pos, SPtr() );
	}

	return vals[ pos ];

}

// ============================================================================

const ComparableCPtr & STL::SMap::getKey(
	const	Uint32		pIndex ) const {

	if ( pIndex >= keys.getSize() ) {
		throw InvalidArgument();
	}

	return keys[ pIndex ];

}

const SPtr & STL::SMap::getValue(
	const	Uint32		pIndex ) const {

	if ( pIndex >= vals.getSize() ) {
		throw InvalidArgument();
	}

	return vals[ pIndex ];

}

SPtr & STL::SMap::getValue(
	const	Uint32		pIndex ) {

	if ( pIndex >= vals.getSize() ) {
		throw InvalidArgument();
	}

	return vals[ pIndex ];

}

// ============================================================================

const ComparableCPtr & STL::SMap::keyOf(
	const	SPtr &		pVal ) const {

	for ( Uint32 pos = 0 ; pos < vals.getSize() ; pos++ ) {
		if ( vals[ pos ] == pVal ) {
			return keys[ pos ];
		}
	}

	throw InvalidArgument();

}

// ============================================================================

void STL::SMap::add(
	const	ComparableCPtr &
				pKey,
	const	SPtr &		pVal ) {

	Uint32 pos;

	if ( lookup( pKey, pos ) ) {
		throw InvalidArgument();
	}

	keys.insert( pos, pKey );
	vals.insert( pos, pVal );

}

void STL::SMap::setOrAdd(
	const	ComparableCPtr &
				pKey,
	const	SPtr &		pVal ) {

	Uint32 pos;

	if ( lookup( pKey, pos ) ) {
		vals[ pos ] = pVal;
	}
	else {
		keys.insert( pos, pKey );
		vals.insert( pos, pVal );
	}

}

void STL::SMap::del(
	const	ComparableCPtr &
				pKey ) {

	Uint32	pos = 0;

	if ( ! lookup( pKey, pos ) ) {
		throw InvalidArgument();
	}

	keys.pop( pos );
	vals.pop( pos );

}

void STL::SMap::pop(
	const	Uint32		pIndex ) {

	if ( pIndex >= keys.getSize() ) {
		throw InvalidArgument();
	}

	keys.pop( pIndex );
	vals.pop( pIndex );

}

// ============================================================================

Bool STL::SMap::lookup(
	const	ComparableCPtr &
				pKey,
		Uint32 &	pIndex ) const {

	if ( keys.isEmpty() ) {
		pIndex = 0;
		return false;
	}

	Uint32	minIdx = 0;
	Uint32	maxIdx = keys.getSize();

	for (;;) {

		// Invariant: maxIdx - minIdx >= 1

		pIndex = ( ( minIdx + maxIdx ) / 2 );
			// minIdx <= pIndex < maxIdx now !

		if ( keys[ pIndex ] == pKey ) {
			return true;
		}

		Int32 res = pKey->compareTo( keys[ pIndex ] );

		if ( res == 0 ) {
			return true;
		}
		else if ( res == -1 ) {
			if ( pIndex == minIdx ) {
				return false;
			}
			maxIdx = pIndex;
		}
		else if ( res == 1 ) {
			if ( pIndex == minIdx ) {
				pIndex = maxIdx;
				return false;
			}
			minIdx = pIndex;
		}
		else {
			throw InternalError();
		}

	}

}

// ============================================================================

