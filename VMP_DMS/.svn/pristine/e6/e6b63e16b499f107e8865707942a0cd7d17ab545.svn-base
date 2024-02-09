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
//	TBFC.Base.PropertyUser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.Base.PropertyUser.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

PropertyUser::~PropertyUser() {



}

// ============================================================================

const Buffer & PropertyUser::getProperty(
	const	Buffer		& __propertyName ) const {

	for ( Uint32 i = 0 ; i < propertyBag.size() ; i++ ) {
		if ( propertyBag[ i ].getTag() == __propertyName ) {
			return propertyBag[ i ].getVal();
		}
	}

	throw NoSuchProperty( "Name: \"" + __propertyName + "\"" );

}

void PropertyUser::setProperty(
	const	Buffer		& __propertyName,
	const	Buffer		& __propertyValue ) {

	for ( Uint32 i = 0 ; i < propertyBag.size() ; i++ ) {
		if ( propertyBag[ i ].getTag() == __propertyName ) {
			propertyBag[ i ].setVal( __propertyValue );
			return;
		}
	}

	throw NoSuchProperty( "Name: \"" + __propertyName + "\"" );

}

// ============================================================================

