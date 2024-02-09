// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::STL".
// 
// "BFC::STL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::STL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::STL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.STL.SArray.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.STL.SArray.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::STL::SArray
// ============================================================================

BFC_PTR_IMPL_NS( STL, SArray )

// ============================================================================

STL::SArray::SArray(
	const	Uint32		pSize ) :

	ptr( 0 ),
	log( 0 ),
	phy( 0 ) {

	if ( pSize ) {
		insertHole( 0, pSize );
	}

}

STL::SArray::SArray(
	const	SArray &	pOther ) :

	SObject(),

	ptr( 0 ),
	log( 0 ),
	phy( 0 ) {

	doCopy( pOther, 0, pOther.log );

}

STL::SArray::SArray(
	const	SArray &	pOther,
	const	Uint32		pStart ) :

	SObject(),

	ptr( 0 ),
	log( 0 ),
	phy( 0 ) {

	doCopy( pOther, pStart, pOther.log );

}

STL::SArray::SArray(
	const	SArray &	pOther,
	const	Uint32		pStart,
	const	Uint32		pCount ) :

	SObject(),

	ptr( 0 ),
	log( 0 ),
	phy( 0 ) {

	doCopy( pOther, pStart, pCount );

}

// ============================================================================

STL::SArray::~SArray() {

	if ( phy ) {
		delete[] ptr;
	}

}

// ============================================================================

STL::SArray & STL::SArray::operator = (
	const	SArray &	pOther ) {

	if ( this != & pOther ) {
		doCopy( pOther, 0, pOther.log );
	}

	return * this;

}

// ============================================================================

STL::SArray STL::SArray::operator () (
	const	Uint32		pStart ) const {

	SArray res;

	res.doCopy( *this, pStart, log );

	return res;

}

STL::SArray STL::SArray::operator () (
	const	Uint32		pStart,
	const	Uint32		pCount ) const {

	SArray res;
	res.doCopy( *this, pStart, pCount );

	return res;

}

// ============================================================================

void STL::SArray::insert( 
	const	Uint32		index,
	const	SPtr &		value,
	const	Bool		pAllowDup ) {

	if ( index > log ) {
		throwOutOfRange( index );
	}

	if ( ! pAllowDup && contains( value ) ) {
		throwDuplicate();
	}

	insertHole( index, 1 );

	ptr[ index ] = value;

}

void STL::SArray::insert( 
	const	Uint32		index,
	const	SArray &	array,
	const	Bool		pAllowDup ) {

	if ( this == & array ) {

		SArray tmp = array;

		insert( index, tmp, pAllowDup );

	}
	else {

		if ( index > log ) {
			throwOutOfRange( index );
		}

		if ( ! pAllowDup ) {
			for ( Uint32 i = 0 ; i < array.getSize() ; i++ ) {
				if ( contains( array[ i ] ) ) {
					throwDuplicate();
				}
			}
		}

		Uint32	pos = index;
		Uint32	cnt = array.log;
	
		insertHole( pos, cnt );

		while ( cnt-- ) {
			ptr[ pos + cnt ] = array.ptr[ cnt ];
		}

	}

}

// ============================================================================

void STL::SArray::remove(
	const	SPtr &		elt,
	const	Bool		all ) {

	for ( Uint32 i = 0 ; i < log ; ) {
		if ( ptr[ i ] == elt ) {
			deleteHole( i, 1 );
			if ( ! all ) {
				break;
			}
		}
		else {
			i++;
		}
	}

}

SPtr STL::SArray::pop(
	const	Uint32		index ) {

	checkIndex( index );

	SPtr res = ptr[ index ];

	deleteHole( index, 1 );

	return res;

}

void STL::SArray::popFirst(
	const	Uint32		pSize ) {

	if ( ! pSize ) {
		return;
	}

	checkIndex( pSize - 1 );

	deleteHole( 0, pSize );

}

// ============================================================================

void STL::SArray::kill() {

	if ( log ) {
		deleteHole( 0, log );
	}

}

// ============================================================================

void STL::SArray::resize(
	const	Uint32		pNewLog ) {

	if ( pNewLog == log ) {
		return;
	}
	else if ( pNewLog < log ) {
		deleteHole( pNewLog, log - pNewLog );
	}
	else {
		insertHole( log, pNewLog - log );
	}

}

// ============================================================================

Bool STL::SArray::contains(
	const	SPtr &		t,
		Uint32 &	i ) const {

	for ( i = 0 ; i < log ; i++ ) {
		if ( ptr[ i ] == t ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

Uint32 STL::SArray::getLen(
	const	Uint32		pSize ) {

	if ( ! pSize ) {
		return 0;
	}

	Uint32	cnt = pSize;
	Uint32	res = 1;

	while ( cnt ) {
		cnt >>= 1;
		res <<= 1;
	}

	return ( res ? res : pSize );

}

// ============================================================================

void STL::SArray::throwOutOfRange(
	const	Uint32		pIndex ) const {

	throw OutOfRange(
		"STL::SArray:: idx: "
		+ Buffer( pIndex )
		+ ", sze: "
		+ Buffer( log ) );

}

void STL::SArray::throwDuplicate() const {

	throw InvalidArgument( "Duplicate entry!" );

}

// ============================================================================

void STL::SArray::doCopy(
	const	SArray &	pOther,
	const	Uint32		pStart,
	const	Uint32		pCount ) {

	kill();

	if ( pOther.log && pOther.log > pStart ) {
		Uint32	cnt = pOther.log - pStart;
		if ( cnt > pCount )
			cnt = pCount;
		insertHole( 0, cnt );
		const SPtr *	srcPtr = pOther.ptr + pStart;
		SPtr *		dstPtr = ptr;
		while ( cnt-- ) {
			*dstPtr++ = *srcPtr++;
		}
	}

}

// ============================================================================

void STL::SArray::insertHole(
	const	Uint32		pos,
	const	Uint32		cnt ) {

	//	 0       pos      log        phy
	//	 |        |        |          |
	//	+--------+--------+----------+
	//	|        |\        \         |
	//	|        | \        \        |
	//	+--------+--+--------+-------+
	//	 |           |        |       |
	//	 0        pos+cnt  log+cnt   phy

	if ( pos > log ) {
		throw InternalError();
	}

	if ( ! cnt ) {
		return;
	}

	Uint32	newLog = log + cnt;

	// Make sure we have a big enough array of pointers...

	if ( ! phy ) {
		// We surely have to allocate some space!
		phy = getLen( newLog );
		ptr = new SPtr[ phy ];
	}
	else if ( newLog > phy ) {
		// Not enough space in the actual array... Reallocate is the
		// solution!
		phy = getLen( newLog );
		SPtr * newPtr = new SPtr[ phy ];
		for ( Uint32 i = 0 ; i < pos ; i++ ) {
			newPtr[ i ] = ptr[ i ];
		}
		for ( Uint32 i = pos ; i < log ; i++ ) {
			newPtr[ i + cnt ] = ptr[ i ];
		}
		delete[] ptr;
		ptr = newPtr;
	}
	else if ( pos < log ) {
		Uint32 i = log;
		while ( i-- > pos ) {
			ptr[ i + cnt ] = ptr[ i ];
		}
		i = cnt;
		while ( i-- ) {
			ptr[ pos + i ].kill();
		}
	}

	// OK! Time to return...

	log = newLog;

}

void STL::SArray::deleteHole(
	const	Uint32		pos,
	const	Uint32		cnt ) {

	//	             pos
	//	              +
	//	 0       pos cnt      log     phy
	//	 |        |   |        |       |
	//	+--------+---+--------+-------+
	//	|        |  /        /|       |
	//	|        | /        / |       |
	//	|        |/        /  |       |
	//	+--------+--------+---+-------+
	//	 |        |        |           |
	//	 0       pos     newLog       phy

	if ( pos + cnt > log ) {
		throw InternalError();
	}

	Uint32 newLog = log - cnt;

	for ( Uint32 i = pos ; i < newLog ; i++ ) {
		ptr[ i ] = ptr[ i + cnt ];
	}

	for ( Uint32 i = newLog ; i < log ; i++ ) {
		ptr[ i ].kill();
	}

	log = newLog;

}

// ============================================================================

