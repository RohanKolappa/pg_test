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
//	BFC.Frac64.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <math.h>

#include "BFC.Base.Exception.h"
#include "BFC.Base.Frac64.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Frac64::Frac64(
	const	Double		pVal ) {

	if ( pVal > Int64Max || pVal < Int64Min ) {
		throw InvalidArgument();
	}

	den = 1;

	Double	tmpNum = pVal;
	Double	tmpDbl;

	while ( ::modf( tmpNum, &tmpDbl ) != 0.0 ) {
		tmpNum *= 10.0;
		den *= 10;
	}

	num = ( Int64 )tmpNum;

}

// ============================================================================

void Frac64::simplify() {

	if ( den == 0 ) {
		throw InternalError();
	}

	if ( num == 0 ) {
		den = 1;
		return;
	}

	if ( den < 0 ) {
		num = -num;
		den = -den;
	}

	Uint64 f = ( num < 0 ? gcd( -num, den ) : gcd( num, den ) );

	num /= f;
	den /= f;

}

// ============================================================================

void Frac64::throwDivideByZero() {

	throw InternalError( "Divide by zero!" );

}

// ============================================================================

Uint64 Frac64::gcd(
		Uint64		a,
		Uint64		b ) {

	for (;;) {
		if ( a < b ) {
			Uint64 t = a;
			a = b;
			b = t;
		}
		if ( b == 0 ) {
			return a;
		}
		a %= b;
	}

}

// ============================================================================

