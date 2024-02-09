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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
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
//	BFC.Base.UUId.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_UUId_H_
#define _BFC_Base_UUId_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.System.h"

#include "BFC.TL.CompArray.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Represents an immutable universally unique identifier (UUId).
///
/// A UUId represents a 128-bit value.
///
/// There exist different variants of these global identifiers:
/// -	1: specified in RFC 4122
/// -	2: "Leach-Salz".
/// The constructors allow the creation of any variant of UUID.
///
/// The layout of a variant 1 UUId is as follows. The fields are encoded as 16
/// octets, and with each field encoded with the Most Significant Byte first
/// (known as network byte order). Note that the field names, particularly for
/// multiplexed fields, follow historical practice.
/// \code
///   0                   1                   2                   3
///    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |                          time_low                             |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |       time_mid                |         time_hi_and_version   |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///   |                         node (2-5)                            |
///   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// \endcode
///
/// Variant 1 supports a number of versions:
/// -	1: time-based
/// -	2: DCE security, with embedded POSIX UIDs
/// -	3: name-based, using MD5
/// -	4: randomly or pseudo-randomly generated
/// -	5: name-based, using SHA1.
///
/// The layout of a variant 2 (Leach-Salz) UUID is as follows: the most
/// significant long consists of the following unsigned fields:
/// \code
///	0xFFFFFFFF00000000 time_low
///	0x00000000FFFF0000 time_mid
///	0x000000000000F000 version
///	0x0000000000000FFF time_hi
/// \endcode
///
/// The least significant long consists of the following unsigned fields:
/// \code
///	0xC000000000000000 variant
///	0x3FFF000000000000 clock_seq
///	0x0000FFFFFFFFFFFF node
/// \endcode
///
/// \ingroup BFC_Base

class BFC_Light_DLL UUId {

public :

	static UUId makeV1(
	);

	UUId(
	);

	UUId(
		const	Buffer &	pBuffer
	);

	/// \brief Constructs a new variant 1 / version 1 UUId.

	UUId(
		const	Uint32		time_low,		// 4 bytes
		const	Uint16		time_mid,		// 2 bytes
		const	Uint16		time_hi_and_version,	// 2 bytes
		const	Uint16		clock_seq,		// 2 bytes
		const	Buffer &	node			// 6 bytes
	);

	/// \brief Constructs a new variant 1 / version 1 UUId.

	UUId(
		const	Uint64		pTime,
		const	Uint16		pClck,
		const	Buffer &	pNode
	);

	/// \brief Constructs a new UUId.
	///
	/// The first 3 parameters are converted to Big Endian format before
	/// being copied byte per byte into the destination array.

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

	/// \brief Constructs a copy of \a other.

	UUId(
		const	UUId &		other
	);

	/// \brief Copies \a other.

	UUId & operator = (
		const	UUId &		other
	);

	/// \brief Reset value of UUId variable to the NULL value.
	///
	/// This method sets the value of this object to the NULL value.

	void clear(
	);

	/// \brief Generates a variant 1 / version 1 UUId.

	void generateV1(
	);

	/// \brief Creates a UUID using a "name" from a "name space".
	///
	/// \param pNSId
	///	UUID to serve as context, so identical names from different
	///	name spaces generate different UUIDs.
	///
	/// \param pName
	///	The name from which to generate a UUID.

	void generateV3(
		const	UUId &		pNSId,
		const	Buffer &	pName
	);

	Bool operator == (
		const	UUId &		o
	) const;

	Bool operator != (
		const	UUId &		o
	) const;

	Bool isNull(
	) const;

	/// \brief Initializes this object with a 16 bytes long buffer.

	void fromCompactBuffer(
		const	Buffer &	src
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
		const	Uchar *		ptr
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
		const	Buffer &	in
	);

	Buffer toASFFile(
	) const;

	Buffer toCompactBuffer(
	) const;

	/// \brief Returns an ASCII representation of this UUId.
	///
	/// The UUID string representation is as described by this BNF :
	/// \verbatim
	///	UUID                  = <time_low> "-" <time_mid> "-"
	///	                        <time_high_and_version> "-"
	///	                        <variant_and_sequence> "-"
	///	                        <node>
	///	time_low              = 4*<hexOctet>
	///	time_mid              = 2*<hexOctet>
	///	time_high_and_version = 2*<hexOctet>
	///	variant_and_sequence  = 2*<hexOctet>
	///	node                  = 6*<hexOctet>
	///	hexOctet              = <hexDigit><hexDigit>
	///	hexDigit              = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
	///	                      | "a" | "b" | "c" | "d" | "e" | "f"
	///	                      | "A" | "B" | "C" | "D" | "E" | "F"
	/// \endverbatim

	Buffer toBuffer(
	) const;

	operator Buffer (
	) const {
		return toBuffer();
	}

	const Uchar * getMemPtr(
	) const {
		return d;
	}

	/// \brief Makes a UUId from \a pTime, \a pClck, and \a pNode.

	void buildV1(
		const	Uint64		pTime,
		const	Uint16		pClck,
		const	Buffer &	pNode
	);

	/// \brief Makes a UUID from a (pseudo)random 128 bit number.

	void buildV3(
		const	Buffer &	hash
	);

protected :

	static Uchar hexVal(
		const	Uchar		c
	);

	static void hexStrToNum(
		const	Uchar *		i,
			Uchar *		o
	) {
		*o = ( Uchar )( ( hexVal( i[0] ) << 4 ) | hexVal( i[1] ) );
	}

	static const Uchar * hexTable;

	static void hexNumToStr(
		const	Uchar *		i,
			Uchar *		o
	) {
		o[0] = hexTable[ *i >> 4 ];
		o[1] = hexTable[ *i & 0x0F ];
	}

	static void skipDash(
		const	Uchar *		i
	);

	static void putDash(
			Uchar *		o
	) {
		o[0] = '-';
	}

	/// \brief Returns the current time, as 100ns ticks since Oct 15, 1582.

	static Uint64 getSystemTime(
	);

	/// \brief Gets time as 60 bit 100ns ticks since whenever.
	///
	/// Compensates for the fact that real clock resolution is less than
	/// 100ns.

	static Uint64 getCurrentTime(
	);

	class MD5 {

	public :

		MD5(
		);

		void init(
		);
	
		void process(
			const	Buffer &	pData
		);
	
		void done(
				Buffer &	out
		);

	protected :

		void compress(
			const	Uchar *		pData
		);

		static Uint32 F( const Uint32 x, const Uint32 y, const Uint32 z) { return (z ^ (x & (y ^ z))); }
		static Uint32 G( const Uint32 x, const Uint32 y, const Uint32 z) { return (y ^ (z & (y ^ x))); }
		static Uint32 H( const Uint32 x, const Uint32 y, const Uint32 z) { return (x^y^z); }
		static Uint32 I( const Uint32 x, const Uint32 y, const Uint32 z) { return (y^(x|(~z))); }
	
		static void FF( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += F(b,c,d) + M + t; a = ROL(a, s) + b; }
		static void GG( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += G(b,c,d) + M + t; a = ROL(a, s) + b; }
		static void HH( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += H(b,c,d) + M + t; a = ROL(a, s) + b; }
		static void II( Uint32 & a, const Uint32 b, const Uint32 c, const Uint32 d, const Uint32 M, const Uint32 s, const Uint32 t ) { a += I(b,c,d) + M + t; a = ROL(a, s) + b; }

		static void STORE32L( const Uint32 x, Uchar * y ) { *( Uint32 * )y = System::Host2LE( x ); }
		static void STORE64L( const Uint64 x, Uchar * y ) { *( Uint64 * )y = System::Host2LE( x ); }

		static Uint32 LOAD32L( const Uchar * y ) { return System::Host2LE( *( const Uint32 * )y ); }

		static Uint32 ROL( const Uint32 x, const Uint32 y ) {
			return ( ( x << (      ( y & 31 ) ) )
			       | ( x >> ( 32 - ( y & 31 ) ) ) );
		}

	private :

		Buffer	msgBuf;
		Uint32	state[4];
		Uint64	msgLen;

	};

private :

	// From: http://tools.ietf.org/html/draft-leach-uuids-guids-01
	//	"A UUID is encoded as a 128-bit object, as follows: the fields
	//	are encoded as 16 octets, with the sizes and order of the
	//	fields defined in section 3.1, and with each field encoded with
	//	the Most Significant Byte first (also known as network byte
	//	order)."
	//
	//  0                   1                   2                   3
	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |                          time_low                             |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |       time_mid                |         time_hi_and_version   |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |                         node (2-5)                            |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Uchar		d[ 16 ];	///< 16 bytes.
	static const Uchar
			n[ 16 ];	///< Null UUId.

};

// ============================================================================

typedef CompArray< UUId > UUIdArray;

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_UUId_H_

// ============================================================================

