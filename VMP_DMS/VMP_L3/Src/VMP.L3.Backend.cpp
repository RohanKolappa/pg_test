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
//	VMP.L3.Backend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.L3.Backend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, Backend )

// ============================================================================

const DL::TypeCPtr & L3::Backend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "66414903-6526-4431-bf8e-637dd6017d7c" ),
		"VMP.L3.Backend",
		UI::UObject::getClassType() );

	return i;

}

// ============================================================================

DynVar L3::Backend::getUserSettings() {

	DynVar res;

	for ( Uint32 i = 0 ; i < getDynVars().getSize() ; i++ ) {
		res.add(
			getDynVars()[ i ]->getName(),
			getDynVars()[ i ]->toBuffer() );
	}

	return res;

}

void L3::Backend::setUserSettings(
	const	DynVar &		settings ) {

	for ( Uint32 i = 0 ; i < settings.size() ; i++ ) {
		Buffer	t = settings.getTag( i );
		Buffer	v = settings.getVal( i );
		getDynVar( t )->fromBuffer( v );
	}

}

// ============================================================================

void L3::Backend::startBackend() {

}

void L3::Backend::stopBackend() {

}

// ============================================================================

