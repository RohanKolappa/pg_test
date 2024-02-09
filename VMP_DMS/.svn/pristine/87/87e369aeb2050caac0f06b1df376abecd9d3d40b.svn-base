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
//	TBFC.TL.TupleN.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_TupleN_H_
#define _TBFC_TL_TupleN_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.TL.CompArray.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A simple templated N-uple.
///
/// \ingroup TBFC_TL

template < typename T, Uint32 N >
class TupleN {

public :

	TupleN(
	) {
		a.resize( N );
	}

	TupleN(
		const	TupleN< T, N >	& c
	) : a( c.a ) {
	}

	TupleN< T, N > & operator = (
		const	TupleN< T, N >	& c
	) {
		if ( this != & c ) {
			a = c.a;
		}
		return *this;
	}

	Bool operator == (
		const	TupleN< T, N >	& n
	) const {
		return ( a == n.a );
	}

	Bool operator != (
		const	TupleN< T, N >	& n
	) const {
		return ( a != n.a );
	}

	const T & elt(
		const	Uint32		idx
	) const {
		return a[ idx ];
	}

	T & elt(
		const	Uint32		idx
	) {
		return a[ idx ];
	}

private :

	CompArray< T >		a;

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_TupleN_H_

// ============================================================================

