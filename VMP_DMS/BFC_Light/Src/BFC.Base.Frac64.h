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
//	BFC.Frac64.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Frac64_H_
#define _BFC_Frac64_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Light

class BFC_Light_DLL Frac64 {

public :

	/// \brief Creates a new Frac64.

	Frac64(
		const	Int64		pNum = 0,
		const	Int64		pDen = 1
	) :
		num( pNum ),
		den( pDen )
	{
		simplify();
	}

	/// \brief Creates a new Frac64.

	Frac64(
		const	Double		pVal
	);

	/// \brief Copy constructor.

	Frac64(
		const	Frac64 &	pOther
	) :
		num( pOther.num ),
		den( pOther.den ) {
	}

	/// \brief Copy operator.

	Frac64 & operator = (
		const	Frac64 &	pOther
	) {
		num = pOther.num;
		den = pOther.den;
		return *this;
	}

	Bool operator == (
		const	Frac64 &	pOther
	) const {
		return ( num == pOther.num
		      && den == pOther.den );
	}

	Bool operator != (
		const	Frac64 &	pOther
	) const {
		return ( num != pOther.num
		      || den != pOther.den );
	}

//	operator Bool (
//	) const {
//		return ( num != 0 );
//	}

	Bool operator ! (
	) const {
		return ( num == 0 );
	}

	Int64 getNum(
	) const {
		return num;
	}

	void setNum(
		const	Int64		pNum
	) {
		num = pNum;
	}

	Int64 getDen(
	) const {
		return den;
	}

	void setDen(
		const	Int64		pDen
	) {
		den = pDen;
	}

	Frac64 ceil(
	) const {
		return Frac64( ( num + den - 1 ) / den );
	}

	Int64 toInt64(
	) const {
		if ( ! den ) {
			throwDivideByZero();
		}
		return ( num / den );
	}

	Double toDouble(
	) const {
		if ( ! den ) {
			throwDivideByZero();
		}
		return ( ( Double )num / ( Double )den );
	}

	void simplify(
	);

	Buffer toBuffer(
		const	Bool		pForceDen = true
	) const {
		return ( pForceDen || den != 1
			? Buffer( num ) + "/" + Buffer( den )
			: Buffer( num ) );
	}

	static void throwDivideByZero(
	);

protected :

	/// \brief Computes the GCD using Euclid's algorithm.
	///
	/// The Euclidean algorithm uses the division algorithm in combination
	/// with the observation that the gcd of two numbers also divides their
	/// difference. For example: divide 48 by 18 to get a quotient of 2 and
	/// a remainder of 12. Then divide 18 by 12 to get a quotient of 1 and
	/// a remainder of 6. Then divide 12 by 6 to get a remainder of 0,
	/// which means that 6 is the gcd.

	static Uint64 gcd(
			Uint64		a,
			Uint64		b
	);

private :

	Int64		num;
	Int64		den;

};

// ============================================================================

inline Frac64 operator + (
	const	Frac64 &	pFrac1,
	const	Frac64 &	pFrac2
) {

	Frac64 res(
		pFrac1.getNum() * pFrac2.getDen() + pFrac1.getDen() * pFrac2.getNum(),
		pFrac1.getDen() * pFrac2.getDen() );

	res.simplify();

	return res;

}

inline Frac64 operator - (
	const	Frac64 &	pFrac1,
	const	Frac64 &	pFrac2
) {

	Frac64 res(
		pFrac1.getNum() * pFrac2.getDen() - pFrac1.getDen() * pFrac2.getNum(),
		pFrac1.getDen() * pFrac2.getDen() );

	res.simplify();

	return res;

}

inline Frac64 operator * (
	const	Frac64 &	pFrac1,
	const	Frac64 &	pFrac2
) {

	Frac64 res(
		pFrac1.getNum() * pFrac2.getNum(),
		pFrac1.getDen() * pFrac2.getDen() );

	res.simplify();

	return res;

}

inline Frac64 operator / (
	const	Frac64 &	pFrac1,
	const	Frac64 &	pFrac2
) {

	Frac64 res(
		pFrac1.getNum() * pFrac2.getDen(),
		pFrac1.getDen() * pFrac2.getNum() );

	if ( ! res.getDen() ) {
		Frac64::throwDivideByZero();
	}

	res.simplify();

	return res;

}

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Frac64_H_

// ============================================================================

