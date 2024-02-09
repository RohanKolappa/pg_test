// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "JBS".
// 
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	JBS.UserDefsMap.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.UserDefsMap.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( UserDefsMap )

// ============================================================================

UserDefsMap::UserDefsMap()
	: last( 0 ) {



}

UserDefsMap::~UserDefsMap() {



}

// ============================================================================

void UserDefsMap::reset(
	const	BFC::Buffer&		varname ) {

	getDefinition( varname )->reset();

}

void UserDefsMap::append(
	const	BFC::Buffer&		varname,
	const	BFC::Buffer&		varvalue ) {

	getDefinition( varname )->append( varvalue );

}

UserDefinitionPtr UserDefsMap::find(
	const	BFC::Buffer&		varname ) const {

	for ( Uint32 i = 0 ; i < defs.size() ; i++ ) {
		if ( defs[ i ]->getName() == varname ) {
			return defs[ i ];
		}
	}

	return UserDefinitionPtr();

}

// ============================================================================

UserDefinitionPtr UserDefsMap::getDefinition(
	const	Buffer&		varname ) {

	if ( last < defs.size() && defs[ last ]->getName() == varname ) {
		return defs[ last ];
	}

	for ( last = 0 ; last < defs.size() ; last++ ) {
		if ( defs[ last ]->getName() == varname ) {
			return defs[ last ];
		}
	}

	defs += new UserDefinition( varname );

	return defs[ last ];

}

// ============================================================================

