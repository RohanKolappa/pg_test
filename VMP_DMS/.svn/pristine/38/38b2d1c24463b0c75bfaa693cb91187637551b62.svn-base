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
//	BFC.Base.Fix266.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Fix266_H_
#define _BFC_Base_Fix266_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Base

class BFC_Light_DLL Fix266 {

public :

	Fix266(
	) : val( 0 ) {
	}

	Fix266(
		const	Int32		i
	) : val( i << 6 ) {
	}

	Fix266(
		const	Double		d
	) : val( ( Int32 )( d * 64.0 ) ) {
	}

	Fix266(
		const	Fix266 &	o
	) : val( o.val ) {
	}

	Fix266 & operator = (
		const	Int32		i
	) {
		val = ( i << 6 );
		return *this;
	}

	static Fix266 fromReal(
		const	Double		r
	) {
		return Fix266( r );
	}

	static Fix266 fromFixed(
		const	Int32		fixed
	) {
		Fix266 f;
		f.val = fixed;
		return f;
	}

	Int32 value(
	) const {
		return val;
	}

	void setValue(
		const	Int32		value
	) {
		val = value;
	}

	Int32 toInt(
	) const {
		return ( ( ( val + 32 ) & -64 ) >> 6 );
	}

	Double toReal(
	) const {
		return ( ( Double )val ) / 64.0;
	}

	Int32 truncate(
	) const {
		return ( val >> 6 );
	}

	Fix266 round() const { Fix266 f; f.val = ((val)+32) & -64; return f; }
	Fix266 floor() const { Fix266 f; f.val = (val) & -64; return f; }
	Fix266 ceil() const { Fix266 f; f.val = (val+63) & -64; return f; }

	Fix266 operator+(Int32 i) const { Fix266 f; f.val = (val + (i<<6)); return f; }
	Fix266 operator+(Uint32 i) const { Fix266 f; f.val = (val + (i<<6)); return f; }
	Fix266 operator+(const Fix266 &other) const { Fix266 f; f.val = (val + other.val); return f; }
	Fix266 &operator+=(Int32 i) { val += (i<<6); return *this; }
	Fix266 &operator+=(Uint32 i) { val += (i<<6); return *this; }
	Fix266 &operator+=(const Fix266 &other) { val += other.val; return *this; }
	Fix266 operator-(Int32 i) const { Fix266 f; f.val = (val - (i<<6)); return f; }
	Fix266 operator-(Uint32 i) const { Fix266 f; f.val = (val - (i<<6)); return f; }
	Fix266 operator-(const Fix266 &other) const { Fix266 f; f.val = (val - other.val); return f; }
	Fix266 &operator-=(Int32 i) { val -= (i<<6); return *this; }
	Fix266 &operator-=(Uint32 i) { val -= (i<<6); return *this; }
	Fix266 &operator-=(const Fix266 &other) { val -= other.val; return *this; }
	Fix266 operator-() const { Fix266 f; f.val = -val; return f; }

	Bool operator==(const Fix266 &other) const { return val == other.val; }
	Bool operator!=(const Fix266 &other) const { return val != other.val; }
	Bool operator<(const Fix266 &other) const { return val < other.val; }
	Bool operator>(const Fix266 &other) const { return val > other.val; }
	Bool operator<=(const Fix266 &other) const { return val <= other.val; }
	Bool operator>=(const Fix266 &other) const { return val >= other.val; }
	Bool operator!() const { return !val; }

	Fix266 &operator/=(Int32 x) { val /= x; return *this; }
	Fix266 &operator/=(const Fix266 &o) {
		if (o.val == 0) {
			val = 0x7FFFFFFFL;
		} else {
			Bool neg = false;
			Int64 a = val;
			Int64 b = o.val;
			if (a < 0) { a = -a; neg = true; }
			if (b < 0) { b = -b; neg = !neg; }

			Int32 res = (Int32)(((a << 6) + (b >> 1)) / b);

			val = (neg ? -res : res);
		}
		return *this;
	}
	Fix266 operator/(Int32 d) const { Fix266 f; f.val = val/d; return f; }
	Fix266 operator/(Fix266 b) const { Fix266 f = *this; return (f /= b); }
	Fix266 operator>>(Int32 d) const { Fix266 f = *this; f.val >>= d; return f; }
	Fix266 &operator*=(Int32 i) { val *= i; return *this; }
	Fix266 &operator*=(Uint32 i) { val *= i; return *this; }
	Fix266 &operator*=(const Fix266 &o) {
		Bool neg = false;
		Int64 a = val;
		Int64 b = o.val;
		if (a < 0) { a = -a; neg = true; }
		if (b < 0) { b = -b; neg = !neg; }

		Int32 res = (Int32)((a * b + 0x20L) >> 6);
		val = neg ? -res : res;
		return *this;
	}
	Fix266 operator*(Int32 i) const { Fix266 f = *this; return (f *= i); }
	Fix266 operator*(Uint32 i) const { Fix266 f = *this; return (f *= i); }
	Fix266 operator*(const Fix266 &o) const { Fix266 f = *this; return (f *= o); }

protected :

private :

	Int32	val;

};

// ============================================================================

inline Int32 myRound( const Fix266 &f ) { return f.toInt(); }

inline Fix266 operator*(Int32 i, const Fix266 &d) { return d*i; }
inline Fix266 operator+(Int32 i, const Fix266 &d) { return d+i; }
inline Fix266 operator-(Int32 i, const Fix266 &d) { return -(d-i); } 
inline Fix266 operator*(Uint32 i, const Fix266 &d) { return d*i; }
inline Fix266 operator+(Uint32 i, const Fix266 &d) { return d+i; }
inline Fix266 operator-(Uint32 i, const Fix266 &d) { return -(d-i); }
// inline Fix266 operator*(Double d, const Fix266 &d2) { return d2*d; }

inline Bool operator==(const Fix266 &f, Int32 i) { return f.value() == (i<<6); }
inline Bool operator==(Int32 i, const Fix266 &f) { return f.value() == (i<<6); }
inline Bool operator!=(const Fix266 &f, Int32 i) { return f.value() != (i<<6); }
inline Bool operator!=(Int32 i, const Fix266 &f) { return f.value() != (i<<6); }
inline Bool operator<=(const Fix266 &f, Int32 i) { return f.value() <= (i<<6); }
inline Bool operator<=(Int32 i, const Fix266 &f) { return (i<<6) <= f.value(); }
inline Bool operator>=(const Fix266 &f, Int32 i) { return f.value() >= (i<<6); }
inline Bool operator>=(Int32 i, const Fix266 &f) { return (i<<6) >= f.value(); }
inline Bool operator<(const Fix266 &f, Int32 i) { return f.value() < (i<<6); }
inline Bool operator<(Int32 i, const Fix266 &f) { return (i<<6) < f.value(); }
inline Bool operator>(const Fix266 &f, Int32 i) { return f.value() > (i<<6); }
inline Bool operator>(Int32 i, const Fix266 &f) { return (i<<6) > f.value(); }

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Fix266_H_

// ============================================================================

