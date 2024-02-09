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
//	TBFC.Base.FourCC.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_FourCC_H_
#define _TBFC_Base_FourCC_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Represents a FourCC (used in AVI files a.o.).
///
/// This class is used to encode "FourCC" (4 character codes), in an very
/// efficient and easy to use way.
///
/// The properties of a FourCC object are:
/// - value: an array of 4 characters. Character 0 is stored first in memory.
///	Character 3 is stored last.
/// - mask: a mask of 4 characters, used to indicate which bits are to
///	be considered as signicant when performing comparisons.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL FourCC {

public :

	/// \brief Constructs an empty FourCC (value == 0, mask == 0).

	FourCC(
	) : v( 0 ), m( 0 ) {
	}

	/// \brief Constructs a copy of \a other.

	FourCC(
		const	FourCC&		other
	) : v( other.v ), m( other.m ) {
	}

	/// \brief Constructs the FourCC corresponding to the string \a string.
	///
	///
	/// \note All characters are considered to be significant (mask is
	///	0xFFFFFFFF).
	///
	/// \param string
	///	A string of at least 4 characters.
	///	Only the first 4 characters are used if more are given.
	///
	/// \note
	///	Passing an empty string, or a string whose length is less than
	///	4 bytes, will silently produce an empty FourCC (see default
	///	constructor).
	///
	/// Example: FourCC( "DIVX" ) would construct a FourCC, whose first
	/// byte (index 0) is 'D' (0x44), whose second byte (index 1) is 'I'
	/// (0x49), and so on.

	FourCC(
		const	Buffer&		string
	);

	FourCC(
		const	Char		*value,
		const	Char		*mask
	);

	FourCC(
		const	Uint32		__v,
		const	Uint32		__m = 0xFFFFFFFF
	) : v( __v ), m( __m ) {
	}

	~FourCC(
	) {
	}

	FourCC& operator = (
		const	FourCC&		o
	) {
		v = o.v;
		m = o.m;
		return *this;
	}

	Bool operator == (
		const	FourCC&		f
	) const {
		return ( ( v & m & f.m ) == ( f.v & m & f.m ) );
	}

	Bool operator != (
		const	FourCC&		f
	) const {
		return ( ( v & m & f.m ) != ( f.v & m & f.m ) );
	}

	/// \brief Returns a host-dependent integer representation.

	operator Uint32(
	) const {
		return v;
	}

	Buffer toBuffer(
		const	Bool		b = false
	) const;

	Uchar operator [] (
		const	Uint32		i
	) const {
		return *(((Uchar *)&v) + i);
	}

	Uchar& operator [] (const Uint32 i) {
		return *(((Uchar *)&v) + i);
	}

protected :

	static Uint32 length(
		const	Char		*
	);

private :

	Uint32		v;	// 4cc[0] is at &v, 4cc[1] at &v + 1, ...
	Uint32		m;

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_FourCC_H_

// ============================================================================

