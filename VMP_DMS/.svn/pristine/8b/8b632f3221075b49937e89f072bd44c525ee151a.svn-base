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
//	TBFC.Math.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Math_Utils_H_
#define _TBFC_Math_Utils_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Types.h"

// ============================================================================

namespace TBFC {
namespace Math {

// ============================================================================

/// \brief Returns the minimum of \a i1 and \a i2.
///
/// \ingroup TBFC_Math

inline Uint32 getMin(
	const	Uint32		i1,
	const	Uint32		i2
) {
	return ( i1 < i2 ? i1 : i2 );
}

/// \brief Returns the minimum of \a i1, \a i2, and \a i3.
///
/// \ingroup TBFC_Math

inline Uint32 getMin(
	const	Uint32		i1,
	const	Uint32		i2,
	const	Uint32		i3
) {
	return getMin( i1, getMin( i2, i3 ) );
}

/// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
///
/// \ingroup TBFC_Math

inline Uint32 getMin(
	const	Uint32		i1,
	const	Uint32		i2,
	const	Uint32		i3,
	const	Uint32		i4
) {
	return getMin( i1, getMin( i2, getMin( i3, i4 ) ) );
}

/// \brief Returns the minimum of \a i1 and \a i2.
///
/// \ingroup TBFC_Math

inline Uint64 getMin(
	const	Uint64		i1,
	const	Uint64		i2
) {
	return ( i1 < i2 ? i1 : i2 );
}

/// \brief Returns the minimum of \a i1, \a i2, and \a i3.
///
/// \ingroup TBFC_Math

inline Uint64 getMin(
	const	Uint64		i1,
	const	Uint64		i2,
	const	Uint64		i3
) {
	return getMin( i1, getMin( i2, i3 ) );
}

/// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
///
/// \ingroup TBFC_Math

inline Uint64 getMin(
	const	Uint64		i1,
	const	Uint64		i2,
	const	Uint64		i3,
	const	Uint64		i4
) {
	return getMin( i1, getMin( i2, getMin( i3, i4 ) ) );
}

/// \brief Returns the maximum of \a i1 and \a i2.
///
/// \ingroup TBFC_Math

inline Uint32 getMax(
	const	Uint32		i1,
	const	Uint32		i2
) {
	return ( i1 > i2 ? i1 : i2 );
}

/// \brief Returns the maximum of \a i1, \a i2, and \a i3.
///
/// \ingroup TBFC_Math

inline Uint32 getMax(
	const	Uint32		i1,
	const	Uint32		i2,
	const	Uint32		i3
) {
	return getMax( i1, getMax( i2, i3 ) );
}

/// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
///
/// \ingroup TBFC_Math

inline Uint32 getMax(
	const	Uint32		i1,
	const	Uint32		i2,
	const	Uint32		i3,
	const	Uint32		i4
) {
	return getMax( i1, getMax( i2, getMax( i3, i4 ) ) );
}

/// \brief Returns the maximum of \a i1 and \a i2.
///
/// \ingroup TBFC_Math

inline Uint64 getMax(
	const	Uint64		i1,
	const	Uint64		i2
) {
	return ( i1 > i2 ? i1 : i2 );
}

/// \brief Returns the maximum of \a i1, \a i2, and \a i3.
///
/// \ingroup TBFC_Math

inline Uint64 getMax(
	const	Uint64		i1,
	const	Uint64		i2,
	const	Uint64		i3
) {
	return getMax( i1, getMax( i2, i3 ) );
}

/// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
///
/// \ingroup TBFC_Math

inline Uint64 getMax(
	const	Uint64		i1,
	const	Uint64		i2,
	const	Uint64		i3,
	const	Uint64		i4
) {
	return getMax( i1, getMax( i2, getMax( i3, i4 ) ) );
}

// ============================================================================

/// \brief Returns the biggest integer that could be returned by rand().
///
/// \ingroup TBFC_Math

TBFC_Light_DLL Uint32 getRandMax(
);

/// \brief Returns a pseudo-random integer between 0 and getRandMax()
///	(incl).
///
/// \ingroup TBFC_Math

TBFC_Light_DLL Uint32 rand(
);

/// \brief Returns a pseudo-random integer between 0 and \a range
///	(incl).
///
/// \ingroup TBFC_Math

TBFC_Light_DLL Uint32 rand(
	const	Uint32		range
);

/// \brief Returns a random boolean.
///
/// \ingroup TBFC_Math

TBFC_Light_DLL Bool randBool(
);

/// \brief Seeds a new random sequence.
///
/// \ingroup TBFC_Math

TBFC_Light_DLL void srand(
	const	Uint32		seed
);

// ============================================================================

} // namespace Math
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Math_Utils_H_

// ============================================================================

