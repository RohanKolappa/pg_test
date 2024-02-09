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
//	BFC.Time.Delta.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Time_Delta_H_
#define _BFC_Time_Delta_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {
namespace Time {

// ============================================================================

/// \brief Represents the difference between 2 Clock objects.
///
/// \ingroup BFC_Time

class BFC_Light_DLL Delta {

public :

	Delta(
	) : d( 0 ) {
	}

	Delta(
		const	Int64		d_u	// in µsecs
	) : d( d_u ) {
	}

	Delta(
		const	Int32		d_s,	// in secs
		const	Int32		d_u	// in µsecs
	) : d(Int64(d_s) * Int64(1000000) + Int64(d_u)) {
	}

	Delta(
		const	Delta		& t
	) : d(t.d) {
	}

	~Delta(
	) {
	}

	Delta& operator = (
		const	Delta		& t
	) {
		if (this != &t) {d = t.d;}
		return *this;
	}

	Delta operator + (
		const	Delta		& t
	) const {
		return Delta(d + t.d);
	}

	Delta& operator += (
		const	Delta		& t
	) {
		d += t.d;
		return *this;
	}

	Delta operator - (
		const	Delta		& t
	) const {
		return Delta(d - t.d);
	}

	Delta& operator -= (
		const	Delta		& t
	) {
		d -= t.d;
		return *this;
	}

	Delta operator * (
		const	Double	f
	) const {
		return Delta(Int64(Double(d) * f));
	}

	Delta operator * (
		const	Int64	f
	) const {
		return Delta( d * f );
	}

	Delta operator * (
		const	Uint64	f
	) const {
		return Delta( d * ( Int64 )f );
	}

	Delta operator * (
		const	Int32	f
	) const {
		return Delta( d * ( Int64 )f );
	}

	Delta operator * (
		const	Uint32	f
	) const {
		return Delta( d * ( Int64 )( Uint64 )f );
	}

	Double operator / (
		const	Delta &		pOther
	) const {
		return ( toDouble() / pOther.toDouble() );
	}

	Delta operator / (
		const	Double	f
	) const {
		return Delta( ( Int64 )( ( Double )d / f ) );
	}

	Delta operator / (
		const	Int64	f
	) const {
		return Delta( d / f );
	}

	Delta operator / (
		const	Uint64	f
	) const {
		return Delta( d / ( Int64 )f );
	}

	Delta operator / (
		const	Int32	f
	) const {
		return Delta( d / ( Int64 )f );
	}

	Delta operator / (
		const	Uint32	f
	) const {
		return Delta( d / ( Int64 )( Uint64 )f );
	}

	operator Int64 (
	) const {
		return d;
	}

	Double toDouble(
	) const {
		return ( Double )d / ( Double )1000000.0;
	}

	/// \brief Returns a human readable string representing this object.
	///
	/// The format used is "[-]D.hh:mm:ss.uuuuuu", where:
	/// -	'D': days (0..)
	/// -	'h': hours (0..23)
	/// -	'm': minutes (0..59)
	/// -	's': seconds (0..59)
	/// -	'u': microseconds (0..999999)

	Buffer toBuffer(
	) const;

protected :

private :

	Int64		d;	///< Number of µs.

};

// ============================================================================

/// \brief One year (of 365 days).
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneYear;

/// \brief One year (of 366 days).
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneBYear;

/// \brief One day.
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneDay;

/// \brief One hour.
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneHour;

/// \brief One minute.
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneMinute;

/// \brief One second.
///
/// \ingroup BFC_Time

BFC_Light_DLL extern const BFC::Time::Delta OneSecond;

// ============================================================================

} // namespace Time
} // namespace BFC

// ============================================================================

#endif // _BFC_Time_Delta_H_

// ============================================================================

