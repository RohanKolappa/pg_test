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
//	TBFC.Time.Clock.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Time_Clock_H_
#define _TBFC_Time_Clock_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"
#include "TBFC.Base.Exception.h"

#include "TBFC.Time.Delta.h"

// ============================================================================

namespace TBFC {
namespace Time {

// ============================================================================

/// \brief Records time stamps with a maximum resolution of 1 탎 (1e-6 sec).
///
/// Time stamps are recorded as a number of 탎 elapsed since the Epoch
/// (00:00:00 UTC, January 1, 1970).
///
/// A Clock object has always a positive or null value.
///
/// \ingroup TBFC_Time

class TBFC_Light_DLL Clock {

public :

	/// \brief Creates a new Clock, representing the Epoch.

	Clock(
	) : u( 0 ) {
	}

	/// \brief Creates a new Clock, representing the point in time situated
	///	\a usecs 탎 after the Epoch.

	explicit Clock(
		const	Uint64		usecs
	) : u( usecs ) {
	}

	/// \brief Creates a new Clock, representing the point in time situated
	///	\a secs seconds and \a usecs 탎 after the Epoch.

	explicit Clock(
		const	Uint32		secs,
		const	Uint32		usecs
	) : u( ( Uint64 )secs * ( Uint64 )1000000 + ( Uint64 )usecs ) {
	}

	Clock(
		const	Uint32		ye,
		const	Uint32		mo,
		const	Uint32		da,
		const	Uint32		ho,
		const	Uint32		mi,
		const	Uint32		se
	);

	/// \brief Creates a new Clock, representing the same point in time
	///	as \a other.

	Clock(
		const	Clock		& other
	) : u( other.u ) {
	}

	/// \brief Destroys this object.

	~Clock(
	) {
	}

	/// \brief Waits until the system clock reaches the point in time
	///	represented by this object.

	void waitUntilReached(
	) const;

	/// \brief Copies \a other.

	Clock & operator = (
		const	Clock		& other
	) {
		u = other.u;
		return * this;
	}

	/// \brief Returns true iff this object is equal to \a other.

	Bool operator == (
		const	Clock		& other
	) const {
		return ( u == other.u );
	}

	/// \brief Returns true iff this object is different than \a other.

	Bool operator != (
		const	Clock		& other
	) const {
		return ( u != other.u );
	}

	/// \brief Returns true iff this object is less than \a other.

	Bool operator <  (
		const	Clock		& other
	) const {
		return ( u <  other.u );
	}

	/// \brief Returns true iff this object is less than or equal to \a other.

	Bool operator <= (
		const	Clock		& other
	) const {
		return ( u <= other.u );
	}

	/// \brief Returns true iff this object is greater than \a other.

	Bool operator >  (
		const	Clock		& other
	) const {
		return ( u >  other.u );
	}

	/// \brief Returns true iff this object is greater than or equal to \a other.

	Bool operator >= (
		const	Clock		& other
	) const {
		return ( u >= other.u );
	}

	/// \brief Returns the Clock equals to this object plus \a delta.

	Clock operator + (
		const	Delta		& delta
	) const {
		return Clock( u + ( Uint64 )( Int64 )delta );
	}

	/// \brief Returns the Clock equals to this object minus \a delta.

	Clock operator - (
		const	Delta		& delta
	) const {
		return Clock( u - ( Uint64 )( Int64 )delta );
	}

	/// \brief Adds the amount of time given by \a delta, which can be
	///	positive or negative, to this object.
	///
	/// \warning No underflow/overflow check is performed!

	void operator += (
		const	Delta		& delta
	) {
		u += ( Uint64 )( Int64 )delta;
	}

	/// \brief Substracts the amount of time given by \a delta, which can
	///	be positive or negative, from this object.
	///
	/// \warning No underflow/overflow check is performed!

	void operator -= (
		const	Delta		& delta
	) {
		u -= ( Uint64 )( Int64 )delta;
	}

	/// \brief Returns the difference between this object and \a other.

	Delta operator - (
		const	Clock		& other
	) const {
		return Delta( ( Int64 )u - ( Int64 )other.u );
	}

	/// \brief Converts this object to a Uint64, given the number
	///	of microseconds elapsed since the Epoch represented by this
	///	object.

	operator Uint64 (
	) const {
		return u;
	}

	/// \brief Initializes this object by parsing \a buf, which must use
	///	the same format as the one used by strings returned by
	///	toBuffer().

	void buildFrom(
		const	Buffer		& buf
	);

	/// \brief Initializes this object by parsing \a __buffer,
	///	which must match pattern \a __pattern.
	///
	/// \param __buffer
	///	A string representing a date/time.
	///
	/// \param __pattern
	///	A pattern describing the content of \a __buffer. This pattern
	///	may contain the following characters:
	///	-	's' : seconds -    0..59
	///	-	'm' : minutes -    0..59
	///	-	'h' : hours   -    0..23
	///	-	'D' : day     -    1..31
	///	-	'M' : month   -    1..12
	///	-	'Y' : year    - 1970..???.
	///	Any other character contained in the pattern must be matched
	///	literally in \a __buffer.
	///
	/// Examples:
	/// -	buffer "2010-12-12" matches pattern "Y-M-D"
	/// -	buffer "2010/12/12/0/0/0" matches pattern "Y/M/D/h/m/s"
	/// -	buffer "01-12-1980   12.00" matches pattern "D-M-Y   h.m"
	///
	/// All fields not present in the pattern are set to 0.

	void buildFrom(
		const	Buffer		& __buffer,
		const	Buffer		& __pattern
	);

	/// \brief Returns a human readable string representing this object.
	///
	/// The format used is "D.h:m:s.u", where:
	/// -	'D': days (0..)
	/// -	'h': hours (0..23)
	/// -	'm': minutes (0..59)
	/// -	's': seconds (0..59)
	/// -	'u': microseconds (0..999999)

	Buffer toBuffer(
	) const;

	/// \brief Returns a buffer matching "YYYY-MM-DD".

	Buffer dateToBuffer(
	) const;

	/// \brief Returns a buffer matching "hh:mm:ss".

	Buffer timeToBuffer(
	) const;

	/// \brief Returns a buffer matching "YYYY-MM-DD_hh:mm:ss".

	Buffer dateTimeToBuffer(
	) const;

	BASE_CLASS_GEN_EXCEPTION( "TBFC.Time.Clock" );

	CLASS_EXCEPTION( InvalidArgument, "Invalid argument" );

protected :

private :

	Uint64		u;	////< Number of 탎.

};

// ============================================================================

/// \brief Returns the current date/time.
///
/// \ingroup TBFC_Time

TBFC_Light_DLL TBFC::Time::Clock now(	/// Give full namespace to help Doxygen...
);

// ============================================================================

} // namespace Time
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Time_Clock_H_

// ============================================================================

