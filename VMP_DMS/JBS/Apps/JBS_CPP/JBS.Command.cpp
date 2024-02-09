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
//	JBS.Command.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "JBS.Command.h"
#include "JBS.Context.h"

// ============================================================================

using namespace BFC;
using namespace JBS;

// ============================================================================

BFC_PTR_IMPL( Command )

// ============================================================================

Command::Command(
	const	Type		__type )
	: type( __type ) {



}

Command::~Command() {



}

// ============================================================================

void Command::convert(
	const	BFC::Buffer&	typeName ) {

	for ( Uint32 i = MinType ; i < MaxType ; i++ ) {
		if ( typeName == Buffer( convTbl[ i ] ) ) {
			type = (Type)i;
			return;
		}
	}

	throw InternalError( "No such command: " + typeName );

}

Buffer Command::getTypeName() const {

	return Buffer( convTbl[ type ] );

}

// ============================================================================

const char *Command::convTbl[] = {
	"build",
	"clean",
	"distclean",
	"packs",
	"doc",
	"dump"
};

// ============================================================================

