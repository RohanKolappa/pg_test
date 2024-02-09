// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.XML.Utils.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.XML.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

static const Char Y = ( Char )XML::BegName;
static const Char M = ( Char )XML::MidName;
static const Char N = ( Char )XML::NotName;

static const Char catTable[ 128 ] = {

	N, N, N, N, N, N, N, N,	// "        "
	N, N, N, N, N, N, N, N,	// "        "
	N, N, N, N, N, N, N, N,	// "        "
	N, N, N, N, N, N, N, N,	// "        "
	N, N, N, N, N, N, N, N,	// "        "
	N, N, N, N, N, M, M, N,	// "     -. "
	M, M, M, M, M, M, M, M,	// "01234567"
	M, M, Y, N, N, N, N, N,	// "89:     "
	N, Y, Y, Y, Y, Y, Y, Y,	// " ABCDEFG"
	Y, Y, Y, Y, Y, Y, Y, Y,	// "HIJKLMNO"
	Y, Y, Y, Y, Y, Y, Y, Y,	// "PQRSTUVW"
	Y, Y, Y, N, N, N, N, Y,	// "XYZ    _"
	N, Y, Y, Y, Y, Y, Y, Y,	// " abcdefg"
	Y, Y, Y, Y, Y, Y, Y, Y,	// "hijklmno"
	Y, Y, Y, Y, Y, Y, Y, Y,	// "pqrstuvw"
	Y, Y, Y, N, N, N, N, N	// "xyz"

};

XML::CharCategory XML::getCharCategory(
	const	Uchar		uc ) {

	return ( uc < 128
		? ( CharCategory )catTable[ uc ]
		: NotName );

}

// ============================================================================

