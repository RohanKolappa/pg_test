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
//	TBFC.Base.UUId.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_UUId_H_
#define _TBFC_Base_UUId_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"

#include "TBFC.TL.CompArray.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief Represents an immutable universally unique identifier (UUId).
///
/// A UUId represents a 128-bit value.
///
/// There exist different variants of these global identifiers. The methods of
/// this class are for manipulating the Leach-Salz variant, although the
/// constructors allow the creation of any variant of UUId (described below).
///
/// The layout of a variant 2 (Leach-Salz) UUId is as follows:
/// - the most significant long consists of the following unsigned fields:
///	- <tt>0xFFFFFFFF00000000</tt> time_low
///	- <tt>0x00000000FFFF0000</tt> time_mid
///	- <tt>0x000000000000F000</tt> version
///	- <tt>0x0000000000000FFF</tt> time_hi
/// - the least significant long consists of the following unsigned fields:
///	- <tt>0xC000000000000000</tt> variant
///	- <tt>0x3FFF000000000000</tt> clock_seq
///	- <tt>0x0000FFFFFFFFFFFF</tt> node
///
/// The variant field contains a value which identifies the layout of the UUId.
/// The bit layout described above is valid only for a UUId with a variant
/// value of 2, which indicates the Leach-Salz variant.
///
/// The version field holds a value that describes the type of this UUId. There
/// are four different basic types of UUIds: time-based, DCE security,
/// name-based, and randomly generated UUIds. These types have a version value
/// of 1, 2, 3 and 4, respectively.
///
/// For more information including algorithms used to create UUIds, see the
/// Internet-Draft UUIds and GUIDs or the standards body definition at
/// ISO/IEC 11578:1996.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL UUId {

public :

	UUId(
	);

	UUId(
		const	Char *		s
	);

	UUId(
		const	Uint32		clock_low,		// 4 bytes
		const	Uint16		time_mid,		// 2 bytes
		const	Uint16		time_hi_and_version,	// 2 bytes
		const	Uint16		clock_seq,		// 2 bytes
		const	Buffer &	node			// 6 bytes
	);

	UUId(
		const	Uint32		f1,
		const	Uint16		f2,
		const	Uint16		f3,
		const	Uchar		f40,
		const	Uchar		f41,
		const	Uchar		f42,
		const	Uchar		f43,
		const	Uchar		f44,
		const	Uchar		f45,
		const	Uchar		f46,
		const	Uchar		f47
	);

	UUId(
		const	UUId &		other
	);

	virtual ~UUId(
	);

	UUId& operator = (
		const	UUId &		o
	) {
//		if ( this != &o ) {
			is[0] = o.is[0];
			is[1] = o.is[1];
			is[2] = o.is[2];
			is[3] = o.is[3];
//		}
		return *this;
	}

	/// \brief Reset value of UUId variable to the NULL value.
	///
	/// This method sets the value of this object to the NULL value.

	void clear(
	) {
		is[0] = is[1] = is[2] = is[3] = 0;
	}

	Bool operator == (
		const	UUId&		o
	) const {
		return ( is[0] == o.is[0]
		      && is[1] == o.is[1]
		      && is[2] == o.is[2]
		      && is[3] == o.is[3] );
	}

	Bool operator != (
		const	UUId&		o
	) const {
		return ( is[0] != o.is[0]
		      || is[1] != o.is[1]
		      || is[2] != o.is[2]
		      || is[3] != o.is[3] );
	}

	Bool isNull(
	) const {
		return ( ! is[0] && ! is[1] && ! is[2] && ! is[3] );
	}

	/// \brief Initializes this object with a 16 bytes long buffer.

	void fromCompactBuffer(
		const	Buffer		& src
	);

	/// \brief Converts an input UUId string into the internal format.
	///
	/// This method converts the UUId string given by \a in into the
	/// internal format. The input UUId is a string of the form
	/// "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb", as produced by the
	/// toBuffer() method.

	void fromBuffer(
		const	Buffer &	in
	);

	void fromBuffer(
		const	Uchar		*ptr
	);

	/// \brief Converts a 16 bytes long string using the ASF format.
	///
	/// This method converts the UUId string given by \a in into the
	/// internal format. The input UUId must be exactly 16 bytes long,
	/// using the following convention. If the UUId to encode is given
	/// by "<d1>..<d4>-<d5>..<d6>-<d7>..<d8>-<d9>..<d10>-<d11>..<d16>",
	/// then \a in should contain:
	/// "<d4>..<d1><d6><d5><d8><d7><d9>..<d16>" (don't ask why).

	void fromASFFile(
		const	Buffer&		in
	);

	Buffer toCompactBuffer(
	) const;

	/// \brief Returns an ASCII representation of this UUId.
	///
	/// The UUID string representation is as described by this BNF :
	/// \verbatim
	///	UUID                   = <time_low> "-" <time_mid> "-"
	///	                         <time_high_and_version> "-"
	///	                         <variant_and_sequence> "-"
	///	                         <node>
	///	time_low               = 4*<hexOctet>
	///	time_mid               = 2*<hexOctet>
	///	time_high_and_version  = 2*<hexOctet>
	///	variant_and_sequence   = 2*<hexOctet>
	///	node                   = 6*<hexOctet>
	///	hexOctet               = <hexDigit><hexDigit>
	///	hexDigit               =
        ///		"0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
        ///		| "a" | "b" | "c" | "d" | "e" | "f"
        ///		| "A" | "B" | "C" | "D" | "E" | "F"
	/// \endverbatim

	Buffer toBuffer(
	) const;

	const Uchar * getMemPtr(
	) const {
		return ( const Uchar * )is;
	}

protected :

	// helpers...

	static Uchar hexVal(
		const	Uchar		c
	);

	static void hexStrToNum(
		const	Uchar		*i,
			Uchar		*o
	) {
		*o = ( hexVal( i[0] ) << 4 ) | hexVal( i[1] );
	}

	static const Uchar *hexTable;

	static void hexNumToStr(
		const	Uchar		*i,
			Uchar		*o
	) {
		o[0] = hexTable[ *i >> 4 ];
		o[1] = hexTable[ *i & 0x0F ];
	}

	static void skipDash(
		const	Uchar		*i
	);

	static void putDash(
			Uchar		*o
	) {
		o[0] = '-';
	}

private :

	// internal state...

	Uint32		is[ 4 ];	///< 16 bytes.

};

// ============================================================================

typedef CompArray< UUId > UUIdArray;

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_UUId_H_

// ============================================================================

