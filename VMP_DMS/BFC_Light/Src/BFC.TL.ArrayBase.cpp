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
//	BFC.TL.ArrayBase.cpp
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
#include "BFC.Base.System.h"

#include "BFC.TL.ArrayBase.h"

// ============================================================================

using namespace BFC;

// ============================================================================

ArrayBase::ArrayBase(
	const	Uint32		__nbr ) {

	ptr = 0;
	nbr = 0;
	len = 0;

	if ( __nbr ) {
		insertHole( 0, __nbr );
	}

}

ArrayBase::~ArrayBase() {

	if ( nbr ) {
		deleteHole( 0, nbr );
	}

}

// ============================================================================

void ArrayBase::checkIndex(
	const	Uint32		__idx,
	const	Bool		__incl ) const {

	if ( ( ! __incl && __idx >= nbr )
	  || ( __incl && __idx > nbr ) ) {
		throw OutOfRange(
			"ArrayBase:: idx: "
			+ Buffer( __idx )
			+ ", sze: "
			+ Buffer( nbr ) );
	}

}

// ============================================================================

Uint32 ArrayBase::getLen(
	const	Uint32		__nbr ) {

	if ( ! __nbr ) {
		return 0;
	}

	Uint32	cnt = __nbr;
	Uint32	res = 1;

	while ( cnt ) {
		cnt >>= 1;
		res <<= 1;
	}

	return ( res ? res : __nbr );

}

// ============================================================================

void ArrayBase::insertHole(
	const	Uint32		pos,
	const	Uint32		cnt ) {

	//	 0       pos      nbr        len
	//	 |        |        |          |
	//	+--------+--------+----------+
	//	|        |\        \         |
	//	|        | \        \        |
	//	+--------+--+--------+-------+
	//	 |           |        |       |
	//	 0        pos+cnt  nbr+cnt   len

	if ( ! cnt ) {
		return;
	}

	Uint32	newNbr = nbr + cnt;

	// Make sure we have a big enough array of pointers...

	if ( ! len ) {
		// We surely have to allocate some space!
		len = getLen( newNbr );
		ptr = ( void ** )( System::malloc(
				len * sizeof( void * ) ) );
	}
	else if ( newNbr > len ) {
		// Not enough space in the actual array... Reallocate is the
		// solution!
		len = getLen( newNbr );
		ptr = ( void ** )( System::realloc(
				( Uchar * )( ptr ),
				len * sizeof( void * ) ) );
	}

	// Now, optionally move some pointers to the end of the array...

	if ( pos < nbr ) {
		System::memmove(
			( Uchar * )& ptr[ pos + cnt ],
			( const Uchar * )& ptr[ pos ],
			( nbr - pos ) * sizeof( void * ) );
	}

	// OK! Time to return...

	nbr = newNbr;

}

void ArrayBase::deleteHole(
	const	Uint32		pos,
	const	Uint32		cnt ) {

	//	             pos
	//	              +
	//	 0       pos cnt      nbr     len
	//	 |        |   |        |       |
	//	+--------+---+--------+-------+
	//	|        |  /        /|       |
	//	|        | /        / |       |
	//	|        |/        /  |       |
	//	+--------+--------+---+-------+
	//	 |        |        |           |
	//	 0       pos      nbr         len

	nbr -= cnt;

	if ( pos < nbr ) {
		System::memmove(
			( Uchar * )& ptr[ pos ],
			( const Uchar * )& ptr[ pos + cnt ],
			( nbr - pos ) * sizeof( void * ) );
	}

	if ( len && ! nbr ) {
		System::free( ( Uchar * )ptr );
		ptr = 0;
		len = 0;
	}

}

// ============================================================================

