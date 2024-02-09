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
//	BFC.Base.OID.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_OID_H_
#define _BFC_Base_OID_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Crypto

class BFC_Light_DLL OID {

public :

	/// \brief Creates a new OID.

	OID(
		const	Uint32		pVal0 = ( Uint32 )-1,
		const	Uint32		pVal1 = ( Uint32 )-1,
		const	Uint32		pVal2 = ( Uint32 )-1,
		const	Uint32		pVal3 = ( Uint32 )-1,
		const	Uint32		pVal4 = ( Uint32 )-1,
		const	Uint32		pVal5 = ( Uint32 )-1,
		const	Uint32		pVal6 = ( Uint32 )-1,
		const	Uint32		pVal7 = ( Uint32 )-1,
		const	Uint32		pVal8 = ( Uint32 )-1,
		const	Uint32		pVal9 = ( Uint32 )-1,
		const	Uint32		pVal10 = ( Uint32 )-1,
		const	Uint32		pVal11 = ( Uint32 )-1,
		const	Uint32		pVal12 = ( Uint32 )-1,
		const	Uint32		pVal13 = ( Uint32 )-1,
		const	Uint32		pVal14 = ( Uint32 )-1,
		const	Uint32		pVal15 = ( Uint32 )-1
	);

	/// \brief Creates a copy of \a pOther.

	OID(
		const	OID &		pOther
	);

	/// \brief Copy operator.

	OID & operator = (
		const	OID &		pOther
	);

	Bool operator == (
		const	OID &		pOther
	) const;

	Bool operator != (
		const	OID &		pOther
	) const {
		return ( ! ( *this == pOther ) );
	}

	static const Uint32 MaxSize = 32;

	Uint32 getSize(
	) const {
		return l;
	}

protected :

	void checkIndex(
		const	Uint32		pIndex
	) const {
		if ( pIndex >= l ) {
			throw OutOfRange();
		}
	}

public :

	Uint32 getElt(
		const	Uint32		pIndex
	) const {
		checkIndex( pIndex );
		return v[ pIndex ];
	}

	Uint32 operator [] (
		const	Uint32		pIndex
	) const {
		checkIndex( pIndex );
		return v[ pIndex ];
	}

	Uint32 & operator [] (
		const	Uint32		pIndex
	) {
		checkIndex( pIndex );
		return v[ pIndex ];
	}

	void setElt(
		const	Uint32		pIndex,
		const	Uint32		pValue
	) {
		if ( pIndex >= l ) {
			throw OutOfRange();
		}
		v[ pIndex ] = pValue;
	}

	void addElt(
		const	Uint32		pValue
	) {
		if ( l == MaxSize ) {
			throw InternalError();
		}
		v[ l++ ] = pValue;
	}

	void addOID(
		const	OID &		pOther
	) {
		Uint32 ol = pOther.l;
		if ( l + ol > MaxSize ) {
			throw InternalError();
		}
		for ( Uint32 i = 0 ; i < ol ; i++ ) {
			v[ l++ ] = pOther.v[ i ];
		}
	}

	void operator += (
		const	Uint32		pValue
	) {
		addElt( pValue );
	}

	void operator += (
		const	OID &		pOther
	) {
		addOID( pOther );
	}

protected :

private :

	Uint32		v[ MaxSize ];
	Uint32		l;

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_OID_H_

// ============================================================================

