// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "Base".
// 
// "Base" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "Base" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "Base"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.MuxedIndex.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Math.Utils.h"

#include "VMP.MuxedIndex.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL( MuxedIndex )

// ============================================================================

Uint32 MuxedIndex::lookup(
	const	Uint32		pSessionIndex,
	const	Uint64		pSubFrameIndex ) const {

	Uint32	nbr = table.getSize();

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( table[ i ].sessionIndex == pSessionIndex
		  && table[ i ].subFrameIndex == pSubFrameIndex ) {
			return i;
		}
	}

	throw InternalError();

}

// ============================================================================

Buffer MuxedIndex::toBuffer(
	const	Uint32		pMaxIndex ) const {

	Uint32	nbr = table.getSize();

	if ( pMaxIndex ) {
		nbr = Math::getMin( nbr, pMaxIndex );
	}

	Buffer res;

	for ( Uint32 i = 0 ; i < nbr ; i++ ) {
		if ( i ) {
			res += "\n";
		}
		res += Buffer( i, Buffer::Base10, 4 ) + " -> ( "
			+ Buffer( table[ i ].sessionIndex ) + ", "
			+ Buffer( table[ i ].subFrameIndex, Buffer::Base10, 4 ) + " )";
	}

	return res;

}

// ============================================================================

