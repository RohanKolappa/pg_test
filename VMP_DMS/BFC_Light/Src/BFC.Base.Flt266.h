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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//							Free Software Foundation
//							59 Temple Place, Suite 330
//							Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.Flt266.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Light_Flt266_H_
#define _BFC_Light_Flt266_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Floating point number, in fixed 26.6 format.
///
/// \ingroup BFC_Light

class BFC_Light_DLL Flt266 {

public :

	Flt266(
	) : val( 0 ) {
	}

	Flt266(
		const	Int32		i
	) : val( i << 6 ) {
	}

	Flt266 & operator = (
		const	Int32		i
	) {
		val = ( i << 6 );
		return *this;
	}

	static Flt266 fromReal(
		const	Double		r
	) {
		Flt266 f;
		f.val = (Int32)(r*Double(64));
		return f;
	}

	static Flt266 fromFixed(
		const	Int32		fixed
	) {
		Flt266 f;
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
		return ( ( Double )val ) / ( Double )64;
	}

	Int32 truncate(
	) const {
		return ( val >> 6 );
	}

	Flt266 round(
	) const {
		Flt266 f;
		f.val = ((val)+32) & -64;
		return f;
	}

	Flt266 floor(
	) const {
		Flt266 f;
		f.val = (val) & -64;
		return f;
	}

	Flt266 ceil(
	) const {
		Flt266 f;
		f.val = (val+63) & -64;
		return f;
	}

	Flt266 operator+(Int32 i) const { Flt266 f; f.val = (val + (i << 6)); return f; }
	Flt266 operator+(Uint32 i) const { Flt266 f; f.val = (val + (i << 6)); return f; }
	Flt266 operator+(const Flt266 &other) const { Flt266 f; f.val = (val + other.val); return f; }
	Flt266 &operator+=(Int32 i) { val += (i << 6); return *this; }
	Flt266 &operator+=(Uint32 i) { val += (i << 6); return *this; }
	Flt266 &operator+=(const Flt266 &other) { val += other.val; return *this; }
	Flt266 operator-(Int32 i) const { Flt266 f; f.val = (val - (i << 6)); return f; }
	Flt266 operator-(Uint32 i) const { Flt266 f; f.val = (val - (i << 6)); return f; }
	Flt266 operator-(const Flt266 &other) const { Flt266 f; f.val = (val - other.val); return f; }
	Flt266 &operator-=(Int32 i) { val -= (i << 6); return *this; }
	Flt266 &operator-=(Uint32 i) { val -= (i << 6); return *this; }
	Flt266 &operator-=(const Flt266 &other) { val -= other.val; return *this; }
	Flt266 operator-() const { Flt266 f; f.val = -val; return f; }

	Bool operator==(const Flt266 &other) const { return val == other.val; }
	Bool operator!=(const Flt266 &other) const { return val != other.val; }
	Bool operator<(const Flt266 &other) const { return val < other.val; }
	Bool operator>(const Flt266 &other) const { return val > other.val; }
	Bool operator<=(const Flt266 &other) const { return val <= other.val; }
	Bool operator>=(const Flt266 &other) const { return val >= other.val; }
	Bool operator!() const { return !val; }

	Flt266 &operator/=(Int32 x) { val /= x; return *this; }
	Flt266 &operator/=(const Flt266 &o) {
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
	Flt266 operator/(Int32 d) const { Flt266 f; f.val = val/d; return f; }
	Flt266 operator/(Flt266 b) const { Flt266 f = *this; return (f /= b); }
	Flt266 operator>>(Int32 d) const { Flt266 f = *this; f.val >>= d; return f; }
	Flt266 &operator*=(Int32 i) { val *= i; return *this; }
	Flt266 &operator*=(Uint32 i) { val *= i; return *this; }
	Flt266 &operator*=(const Flt266 &o) {
		Bool neg = false;
		Int64 a = val;
		Int64 b = o.val;
		if (a < 0) { a = -a; neg = true; }
		if (b < 0) { b = -b; neg = !neg; }

		Int32 res = (Int32)((a * b + 0x20L) >> 6);
		val = neg ? -res : res;
		return *this;
	}
	Flt266 operator*(Int32 i) const { Flt266 f = *this; return (f *= i); }
	Flt266 operator*(Uint32 i) const { Flt266 f = *this; return (f *= i); }
	Flt266 operator*(const Flt266 &o) const { Flt266 f = *this; return (f *= o); }

protected :

	Flt266(Double i) : val((Int32)(i*Double(64))) {}
	Flt266 &operator=(Double i) { val = (Int32)(i*Double(64)); return *this; }
	Flt266 operator+(Double i) const { Flt266 f; f.val = (val + (Int32)(i*Double(64))); return f; }
	Flt266 &operator+=(Double i) { val += (Int32)(i*64); return *this; }
	Flt266 operator-(Double i) const { Flt266 f; f.val = (val - (Int32)(i*Double(64))); return f; }
	Flt266 &operator-=(Double i) { val -= (Int32)(i*64); return *this; }
	Flt266 &operator/=(Double r) { val = (Int32)(val/r); return *this; }
	Flt266 operator/(Double d) const { Flt266 f; f.val = (Int32)(val/d); return f; }
	Flt266 &operator*=(Double d) { val = (Int32) (val*d); return *this; }
	Flt266 operator*(Double d) const { Flt266 f = *this; return (f *= d); }

private:

	Int32	val;

};

inline Int32 myRound(const Flt266 &f) { return f.toInt(); }

inline Flt266 operator*(Int32 i, const Flt266 &d) { return d*i; }
inline Flt266 operator+(Int32 i, const Flt266 &d) { return d+i; }
inline Flt266 operator-(Int32 i, const Flt266 &d) { return -(d-i); } 
inline Flt266 operator*(Uint32 i, const Flt266 &d) { return d*i; }
inline Flt266 operator+(Uint32 i, const Flt266 &d) { return d+i; }
inline Flt266 operator-(Uint32 i, const Flt266 &d) { return -(d-i); }
// inline Flt266 operator*(Double d, const Flt266 &d2) { return d2*d; }

inline Bool operator==(const Flt266 &f, Int32 i) { return f.value() == (i << 6); }
inline Bool operator==(Int32 i, const Flt266 &f) { return f.value() == (i << 6); }
inline Bool operator!=(const Flt266 &f, Int32 i) { return f.value() != (i << 6); }
inline Bool operator!=(Int32 i, const Flt266 &f) { return f.value() != (i << 6); }
inline Bool operator<=(const Flt266 &f, Int32 i) { return f.value() <= (i << 6); }
inline Bool operator<=(Int32 i, const Flt266 &f) { return (i << 6) <= f.value(); }
inline Bool operator>=(const Flt266 &f, Int32 i) { return f.value() >= (i << 6); }
inline Bool operator>=(Int32 i, const Flt266 &f) { return (i << 6) >= f.value(); }
inline Bool operator<(const Flt266 &f, Int32 i) { return f.value() < (i << 6); }
inline Bool operator<(Int32 i, const Flt266 &f) { return (i << 6) < f.value(); }
inline Bool operator>(const Flt266 &f, Int32 i) { return f.value() > (i << 6); }
inline Bool operator>(Int32 i, const Flt266 &f) { return (i << 6) > f.value(); }

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Light_Flt266_H_

// ============================================================================

