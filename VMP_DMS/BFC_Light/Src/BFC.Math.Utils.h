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
//	BFC.Math.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Math_Utils_H_
#define _BFC_Math_Utils_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {
namespace Math {

// ============================================================================

template < class T >
inline T getMin(
	const	T		t1,
	const	T		t2
) {
	return ( t1 < t2 ? t1 : t2 );
}

template < class T >
inline T getMin(
	const	T		t1,
	const	T		t2,
	const	T		t3
) {
	return getMin( getMin( t1, t2 ), t3 );
}

template < class T >
inline T getMin(
	const	T		t1,
	const	T		t2,
	const	T		t3,
	const	T		t4
) {
	return getMin( getMin( getMin( t1, t2 ), t3 ), t4 );
}

template < class T >
inline T getMax(
	const	T		t1,
	const	T		t2
) {
	return ( t1 > t2 ? t1 : t2 );
}

template < class T >
inline T getMax(
	const	T		t1,
	const	T		t2,
	const	T		t3
) {
	return getMax( getMax( t1, t2 ), t3 );
}

template < class T >
inline T getMax(
	const	T		t1,
	const	T		t2,
	const	T		t3,
	const	T		t4
) {
	return getMax( getMax( getMax( t1, t2 ), t3 ), t4 );
}

template < class T >
inline T getClip(
	const	T		vCur,
	const	T		vMin,
	const	T		vMax
) {
	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
}

// ============================================================================
// Min / Uint32
// ============================================================================

///// \brief Returns the minimum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMin(
//	const	Uint32		i1,
//	const	Uint32		i2
//) {
//	return ( i1 < i2 ? i1 : i2 );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMin(
//	const	Uint32		i1,
//	const	Uint32		i2,
//	const	Uint32		i3
//) {
//	return getMin( i1, getMin( i2, i3 ) );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMin(
//	const	Uint32		i1,
//	const	Uint32		i2,
//	const	Uint32		i3,
//	const	Uint32		i4
//) {
//	return getMin( getMin( i1, i2 ), getMin( i3, i4 ) );
//}

// ============================================================================
// Min / Int32
// ============================================================================

///// \brief Returns the minimum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMin(
//	const	Int32		i1,
//	const	Int32		i2
//) {
//	return ( i1 < i2 ? i1 : i2 );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMin(
//	const	Int32		i1,
//	const	Int32		i2,
//	const	Int32		i3
//) {
//	return getMin( i1, getMin( i2, i3 ) );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMin(
//	const	Int32		i1,
//	const	Int32		i2,
//	const	Int32		i3,
//	const	Int32		i4
//) {
//	return getMin( getMin( i1, i2 ), getMin( i3, i4 ) );
//}

// ============================================================================
// Min / Uint64
// ============================================================================

///// \brief Returns the minimum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMin(
//	const	Uint64		i1,
//	const	Uint64		i2
//) {
//	return ( i1 < i2 ? i1 : i2 );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMin(
//	const	Uint64		i1,
//	const	Uint64		i2,
//	const	Uint64		i3
//) {
//	return getMin( i1, getMin( i2, i3 ) );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMin(
//	const	Uint64		i1,
//	const	Uint64		i2,
//	const	Uint64		i3,
//	const	Uint64		i4
//) {
//	return getMin( getMin( i1, i2 ), getMin( i3, i4 ) );
//}

// ============================================================================
// Min / Double
// ============================================================================

///// \brief Returns the minimum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Double getMin(
//	const	Double		i1,
//	const	Double		i2
//) {
//	return ( i1 < i2 ? i1 : i2 );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Double getMin(
//	const	Double		i1,
//	const	Double		i2,
//	const	Double		i3
//) {
//	return getMin( i1, getMin( i2, i3 ) );
//}
//
///// \brief Returns the minimum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Double getMin(
//	const	Double		i1,
//	const	Double		i2,
//	const	Double		i3,
//	const	Double		i4
//) {
//	return getMin( getMin( i1, i2 ), getMin( i3, i4 ) );
//}

// ============================================================================
// Max / Uint32
// ============================================================================

///// \brief Returns the maximum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMax(
//	const	Uint32		i1,
//	const	Uint32		i2
//) {
//	return ( i1 > i2 ? i1 : i2 );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMax(
//	const	Uint32		i1,
//	const	Uint32		i2,
//	const	Uint32		i3
//) {
//	return getMax( i1, getMax( i2, i3 ) );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Uint32 getMax(
//	const	Uint32		i1,
//	const	Uint32		i2,
//	const	Uint32		i3,
//	const	Uint32		i4
//) {
//	return getMax( getMax( i1, i2 ), getMax( i3, i4 ) );
//}

// ============================================================================
// Max / Int32
// ============================================================================

///// \brief Returns the maximum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMax(
//	const	Int32		i1,
//	const	Int32		i2
//) {
//	return ( i1 > i2 ? i1 : i2 );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMax(
//	const	Int32		i1,
//	const	Int32		i2,
//	const	Int32		i3
//) {
//	return getMax( i1, getMax( i2, i3 ) );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Int32 getMax(
//	const	Int32		i1,
//	const	Int32		i2,
//	const	Int32		i3,
//	const	Int32		i4
//) {
//	return getMax( getMax( i1, i2 ), getMax( i3, i4 ) );
//}

// ============================================================================
// Max / Uint64
// ============================================================================

///// \brief Returns the maximum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMax(
//	const	Uint64		i1,
//	const	Uint64		i2
//) {
//	return ( i1 > i2 ? i1 : i2 );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMax(
//	const	Uint64		i1,
//	const	Uint64		i2,
//	const	Uint64		i3
//) {
//	return getMax( i1, getMax( i2, i3 ) );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Uint64 getMax(
//	const	Uint64		i1,
//	const	Uint64		i2,
//	const	Uint64		i3,
//	const	Uint64		i4
//) {
//	return getMax( getMax( i1, i2 ), getMax( i3, i4 ) );
//}

// ============================================================================
// Max / Double
// ============================================================================

///// \brief Returns the maximum of \a i1 and \a i2.
/////
///// \ingroup BFC_Base
//
//inline Double getMax(
//	const	Double		i1,
//	const	Double		i2
//) {
//	return ( i1 > i2 ? i1 : i2 );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, and \a i3.
/////
///// \ingroup BFC_Base
//
//inline Double getMax(
//	const	Double		i1,
//	const	Double		i2,
//	const	Double		i3
//) {
//	return getMax( i1, getMax( i2, i3 ) );
//}
//
///// \brief Returns the maximum of \a i1, \a i2, \a i3, and \a i4.
/////
///// \ingroup BFC_Base
//
//inline Double getMax(
//	const	Double		i1,
//	const	Double		i2,
//	const	Double		i3,
//	const	Double		i4
//) {
//	return getMax( getMax( i1, i2 ), getMax( i3, i4 ) );
//}

// ============================================================================
// Clip
// ============================================================================

//inline Uint32 getClip(
//	const	Uint32		vCur,
//	const	Uint32		vMin,
//	const	Uint32		vMax
//) {
//	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
//}
//
//inline Int32 getClip(
//	const	Int32		vCur,
//	const	Int32		vMin,
//	const	Int32		vMax
//) {
//	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
//}
//
//inline Uint64 getClip(
//	const	Uint64		vCur,
//	const	Uint64		vMin,
//	const	Uint64		vMax
//) {
//	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
//}
//
//inline Int64 getClip(
//	const	Int64		vCur,
//	const	Int64		vMin,
//	const	Int64		vMax
//) {
//	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
//}
//
//inline Double getClip(
//	const	Double		vCur,
//	const	Double		vMin,
//	const	Double		vMax
//) {
//	return ( vCur < vMin ? vMin : vCur > vMax ? vMax : vCur );
//}

// ============================================================================
// Ceil / floor / ...
// ============================================================================

/// \brief Returns \f$ \lceil \frac{ x }{ y } \rceil \f$.
///
/// \ingroup BFC_Base

inline Uint32 ceilDiv(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( ( x + y - 1 ) / y );
}

/// \brief Returns \f$ \lceil \frac{ x }{ 2^y } \rceil \f$.
///
/// \ingroup BFC_Base

inline Uint32 ceilDivPow2(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( ( x + ( 1 << y ) - 1 ) >> y );
}

/// \brief Returns \f$ \lfloor \frac{ x }{ y } \rfloor \f$.
///
/// \ingroup BFC_Base

inline Uint32 floorDiv(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( x / y );
}

/// \brief Returns \f$ \lfloor \frac{ x }{ 2^y } \rfloor \f$.
///
/// \ingroup BFC_Base

inline Uint32 floorDivPow2(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( x >> y );
}

/// \brief Returns \f$ \left| v \right| \f$.
///
/// \ingroup BFC_Base

inline Uint16 abs(
	const	Int16		v
) {
	return ( Uint16 )( v < 0 ? -v : v );
}

/// \brief Returns \f$ \left| v \right| \f$.
///
/// \ingroup BFC_Base

inline Uint32 abs(
	const	Int32		v
) {
	return ( Uint32 )( v < 0 ? -v : v );
}

/// \brief Returns \f$ \left| v \right| \f$.
///
/// \ingroup BFC_Base

inline Uint64 abs(
	const	Int64		v
) {
	return ( Uint64 )( v < 0 ? -v : v );
}

/// \brief Returns \f$ \sqrt{ d } \f$.
///
/// \ingroup BFC_Base

inline Double sqrt(
	const	Double		d
);

// ============================================================================
// Random
// ============================================================================

/// \brief Returns the biggest integer that could be returned by rand().
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 getRandMax(
);

/// \brief Returns a pseudo-random integer between 0 and getRandMax()
///	(incl).
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 rand(
);

/// \brief Returns a pseudo-random integer between 0 and \a range
///	(incl).
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 rand(
	const	Uint32		range
);

/// \brief Returns a pseudo-random integer between 0 and \a range
///	(incl).
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint64 rand(
	const	Uint64		range
);

/// \brief Returns a pseudo-random boolean.
///
/// \ingroup BFC_Base

BFC_Light_DLL Bool randBool(
);

/// \brief Seeds a new pseudo-random sequence.
///
/// \ingroup BFC_Base

BFC_Light_DLL void srand(
	const	Uint32		seed
);

// ============================================================================

} // namespace Math
} // namespace BFC

// ============================================================================

#endif // _BFC_Math_Utils_H_

// ============================================================================

