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
//	TBFC.TL.Pair.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_TL_Pair_H_
#define _TBFC_TL_Pair_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Types.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A simple templated pair.
///
/// \ingroup TBFC_TL

template < typename E1, typename E2 >
class Pair {

public :

	Pair(
	) {
	}

	Pair(const E1& _e1, const E2& _e2) {
		e1 = _e1;
		e2 = _e2;
	}

	Pair(const Pair<E1,E2>& c) {
		e1 = c.e1;
		e2 = c.e2;
	}

	Pair<E1,E2>& operator = (const Pair<E1,E2>& c) {
		if (this == &c)
			return *this;
		e1 = c.e1;
		e2 = c.e2;
		return *this;
	}

	bool operator == (const Pair<E1,E2>& n) const {
		return (e1 == n.e1 && e2 == n.e2);
	}

	bool operator != (const Pair<E1,E2>& n) const {
		return (e1 != n.e1 || e2 != n.e2);
	}

	const E1& left() const {
		return e1;
	}

	E1& left() {
		return e1;
	}

	const E2& right() const {
		return e2;
	}

	E2& right() {
		return e2;
	}

private :

	E1	e1;
	E2	e2;

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_TL_Pair_H_

// ============================================================================

