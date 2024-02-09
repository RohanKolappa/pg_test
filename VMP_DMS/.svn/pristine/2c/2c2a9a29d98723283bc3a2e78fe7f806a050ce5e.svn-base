// ============================================================================
// 
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
// FileName:
//	TBFC.Base.Version.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

// #include <iostream>

#include "TBFC.Base.BufferParser.h"
#include "TBFC.Base.Version.h"
#include "TBFC.TL.Array.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

void Version::parseBuffer(
	const	Buffer&		src) {

// FIXME: implement this a lot faster!

	v[0] = v[1] = v[2] = prec = 0;

	BufferArray tokens = BufferParser::tokenize( src, "." );

	for ( Uint32 i = 0 ; i < 3 && i < tokens.size() ; i++) {
		v[i] = tokens[i].toUint32();
		prec++;
	}

}

// ============================================================================

Buffer Version::toBuffer() const {

	Buffer res;

	switch ( prec ) {
	case 1 :
		res = Buffer(v[0]);
		break;
	case 2 :
		res = Buffer(v[0]) + "." + Buffer(v[1]);
		break;
	case 3 :
		res = Buffer(v[0]) + "." + Buffer(v[1]) + "." + Buffer(v[2]);
		break;
	default :
		break;
	}

	return res;

}

// ============================================================================

//std::ostream& operator << (
//		std::ostream&		os,
//	const	Version&		src) {
//
//	os << src.toBuffer();
//
//	return os;
//
//}

// ============================================================================





