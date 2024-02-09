// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.PicturesProducerBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.PicturesProducerBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, PicturesProducerBackend )

// ============================================================================

const DL::TypeCPtr & L3::PicturesProducerBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "9ca5e394-a5c2-4c09-b381-7aa00db0f77c" ),
		"VMP.L3.PicturesProducerBackend",
		L3::Backend::getClassType() );

	return i;

}

// ============================================================================

L3::PicturesProducerBackend::PicturesProducerBackend(
	const	Uint32		pNbrOutputs ) :

	nbrOutputs( pNbrOutputs ) {

	setType( getClassType() );

}

// ============================================================================

VideoSessionArray L3::PicturesProducerBackend::processSession(
		VideoSessionPtr		iVSession ) {

	VideoSessionArray res;

	res.resize( getNbrOutputs() );

	for ( Uint32 i = 0 ; i < getNbrOutputs() ; i++ ) {
		res[ i ] = iVSession;
	}

	return res;

}

// ============================================================================

