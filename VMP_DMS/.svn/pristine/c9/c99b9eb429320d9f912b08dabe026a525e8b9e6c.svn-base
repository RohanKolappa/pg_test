// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.TL.Tuple3.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_Tuple3_H_
#define _BFC_TL_Tuple3_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated 3-uple.
///
/// \ingroup BFC_TL

template < typename E1, typename E2, typename E3 >
class Tuple3 {

public :

	Tuple3(
	) {
	}

	Tuple3(
		const	E1		& __e1,
		const	E2		& __e2,
		const	E3		& __e3
	) : e1( __e1 ), e2( __e2 ), e3( __e3 ) {
	}

	Tuple3(
		const	Tuple3< E1, E2, E3 >	& c
	) : e1( c.e1 ), e2( c.e2 ), e3( c.e3 ) {
	}

	Tuple3< E1, E2, E3 > & operator = (
		const	Tuple3< E1, E2, E3 >	& c
	) {
		if ( this != & c ) {
			e1 = c.e1;
			e2 = c.e2;
			e3 = c.e3;
		}
		return * this;
	}

	Bool operator == (
		const	Tuple3< E1, E2, E3 >	& n
	) const {
		return ( e1 == n.e1 && e2 == n.e2 && e3 == n.e3 );
	}

	Bool operator != (
		const	Tuple3< E1, E2, E3 >	& n
	) const {
		return ( e1 != n.e1 || e2 != n.e2 || e3 != n.e3 );
	}

	const E1 & val1(
	) const {
		return e1;
	}

	const E2 & val2(
	) const {
		return e2;
	}

	const E3 & val3(
	) const {
		return e3;
	}

	E1 & val1(
	) {
		return e1;
	}

	E2 & val2(
	) {
		return e2;
	}

	E3 & val3(
	) {
		return e3;
	}

private :

	E1	e1;
	E2	e2;
	E3	e3;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_Tuple3_H_

// ============================================================================

