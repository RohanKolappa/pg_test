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
//	BFC.TL.TupleN.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_TL_TupleN_H_
#define _BFC_TL_TupleN_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A simple templated N-uple.
///
/// \ingroup BFC_TL

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

} // namespace BFC

// ============================================================================

#endif // _BFC_TL_TupleN_H_

// ============================================================================

