// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.List.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.List.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, List )

// ============================================================================

Dyn::ListPtr Dyn::List::clone() const {

	ListPtr res = new List;

	for ( Uint32 i = 0 ; i < map.getSize() ; i++ ) {
		res->map.add(
			map.getKey( i ),
			map.getValue( i )->clone() );
	}

	return res;

}

Dyn::VarCArray Dyn::List::getAll() const {

	Map< Buffer, VarPtr >::ValArrayType tmp = map.getAllValues();

	VarCArray res;

	for ( Uint32 i = 0 ; i < tmp.getSize() ; i++ ) {
		res += tmp[ i ];
	}

	return res;

}

void Dyn::List::setAll(
	const	VarArray &	pArray ) {

	kill();

	for ( Uint32 i = 0 ; i < pArray.getSize() ; i++ ) {
		add( pArray[ i ] );
	}

}

// ============================================================================

