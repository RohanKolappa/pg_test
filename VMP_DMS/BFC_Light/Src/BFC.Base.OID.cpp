// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.OID.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.OID.h"

// ============================================================================

using namespace BFC;

// ============================================================================

OID::OID(
	const	Uint32		pVal0,
	const	Uint32		pVal1,
	const	Uint32		pVal2,
	const	Uint32		pVal3,
	const	Uint32		pVal4,
	const	Uint32		pVal5,
	const	Uint32		pVal6,
	const	Uint32		pVal7,
	const	Uint32		pVal8,
	const	Uint32		pVal9,
	const	Uint32		pVal10,
	const	Uint32		pVal11,
	const	Uint32		pVal12,
	const	Uint32		pVal13,
	const	Uint32		pVal14,
	const	Uint32		pVal15 ) {

	v[ 0 ]	= pVal0;
	v[ 1 ]	= pVal1;
	v[ 2 ]	= pVal2;
	v[ 3 ]	= pVal3;
	v[ 4 ]	= pVal4;
	v[ 5 ]	= pVal5;
	v[ 6 ]	= pVal6;
	v[ 7 ]	= pVal7;
	v[ 8 ]	= pVal8;
	v[ 9 ]	= pVal9;
	v[ 10 ] = pVal10;
	v[ 11 ] = pVal11;
	v[ 12 ] = pVal12;
	v[ 13 ] = pVal13;
	v[ 14 ] = pVal14;
	v[ 15 ] = pVal15;

	Uint32 i = 0;

	while ( i < MaxSize && v[ i ] != ( Uint32 )-1 ) {
		i++;
	}

	l = i;

}

OID::OID(
	const	OID &		pOther ) {

	l = pOther.l;

	for ( Uint32 i = 0 ; i < l ; i++ ) {
		v[ i ] = pOther.v[ i ];
	}

}

OID & OID::operator = (
	const	OID &		pOther ) {

	if ( this != &pOther ) {
		l = pOther.l;
		for ( Uint32 i = 0 ; i < l ; i++ ) {
			v[ i ] = pOther.v[ i ];
		}
	}

	return *this;

}

// ============================================================================

Bool OID::operator == (
	const	OID &		pOther ) const {

	if ( this == &pOther ) {
		return true;
	}

	if ( l != pOther.l ) {
		return false;
	}

	for ( Uint32 i = 0 ; i < l ; i++ ) {
		if ( v[ i ] != pOther.v[ i ] ) {
			return false;
		}
	}

	return true;

}

// ============================================================================

