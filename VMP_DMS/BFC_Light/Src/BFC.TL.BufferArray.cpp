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
//	BFC.TL.BufferArray.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.TL.BufferArray.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BufferArray::BufferArray(
	const	Char **		pTbl ) {

	Uint32	len = 0;

	while ( pTbl[ len ] ) {
		len++;
	}

	resize( len );

	while ( len-- ) {
		elt( len ) = pTbl[ len ];
	}

}

BufferArray::BufferArray(
	const	Uint32		pNum,
	const	Char **		pTbl ) {

	Uint32	len = pNum;

	resize( len );

	while ( len-- ) {
		elt( len ) = pTbl[ len ];
	}

}

BufferArray::BufferArray(
		int		pNum,
		char **		pTbl ) {

	Uint32	len = ( Uint32 )pNum;

	resize( len );

	while ( len-- ) {
		elt( len ) = Buffer( ( const char * )pTbl[ len ] );
	}

}

// ============================================================================

Buffer BufferArray::join(
	const	Uchar		pChar ) const {

	Buffer res;

	for ( Uint32 i = 0 ; i < getSize() ; i++ ) {
		if ( i ) {
			res += pChar;
		}
		res += elt( i );
	}

	return res;

}

// ============================================================================

