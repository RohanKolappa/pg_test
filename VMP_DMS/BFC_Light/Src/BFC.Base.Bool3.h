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
//	BFC.Base.Bool3.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_Bool3_H_
#define _BFC_Light_Bool3_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

enum Choice {

	No,
	Yes,
	Maybe

};

/// \brief [no brief description]
///
/// \ingroup BFC_Light

class BFC_Light_DLL Bool3 {

public :

	/// \brief Creates a new Bool3.

	Bool3(
		const	Choice		pChoice = No
	) :
		c( pChoice ) {
	}

	/// \brief Creates a new Bool3.

	Bool3(
		const	Bool		pChoice
	) :
		c( pChoice ? Yes : No ) {
	}

	/// \brief Forbidden copy constructor.

	Bool3(
		const	Bool3 &		pOther
	) :
		c( pOther.c ) {
	}

	/// \brief Forbidden copy operator.

	Bool3 & operator = (
		const	Bool3 &		pOther
	) {
		c = pOther.c;
		return *this;
	}

	Bool operator == (
		const	Bool3 &		pOther
	) const {
		return ( c == pOther.c );
	}

	Bool operator != (
		const	Bool3 &		pOther
	) const {
		return ( c != pOther.c );
	}

	Bool isCompatibleWith(
		const	Bool3 &		pOther
	) const {
		return ( c == Maybe
		      || pOther.c == Maybe
		      || c == pOther.c );
	}

protected :

private :

	Choice		c;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_Bool3_H_

// ============================================================================

