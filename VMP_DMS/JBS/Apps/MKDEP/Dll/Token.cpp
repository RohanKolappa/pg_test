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
//	Token.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "Token.h"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

BFC_PTR_IMPL(Token)

// ============================================================================

static const char *tbl[] = {
	"<NONE>",
	"STR",
	"INT",
	"(",
	")",
	",",
	"+",
	"-",
	"*",
	"/",
	"%",
	"!",
	"&&",
	"||",
	"<",
	"<=",
	"==",
	"!=",
	">=",
	">"
};

Buffer Token::toBuffer() const {

	if ( type == CstString ) return string;
	if ( type == CstInteger ) return integer;

	return Buffer( tbl[type] );

}

// ============================================================================

