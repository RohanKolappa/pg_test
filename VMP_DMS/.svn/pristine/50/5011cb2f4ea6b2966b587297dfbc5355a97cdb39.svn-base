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
//	BFC.Base.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Utils_H_
#define _BFC_Base_Utils_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

// ============================================================================

namespace BFC {
namespace Utils {

// ============================================================================

/// \brief Updates \a pPos to point to the first non white space character in
///	\a pSrc, starting from the initial value of \a pPos.
///
/// \ingroup BFC_Base

BFC_Light_DLL void skipWhiteSpaces(
	const	Buffer &	pSrc,
		Uint32 &	pPos
);

/// \brief Updates \a pPos to point to the first white space character in
///	\a pSrc, starting from the initial value of \a pPos.
///
/// \ingroup BFC_Base

BFC_Light_DLL void skipNonWhiteSpaces(
	const	Buffer &	pSrc,
		Uint32 &	pPos
);

/// \brief Finds the closing '"' character, starting at \a pStart + 1.
///
/// \a pSrc must contain a '"' character at position \a pStart.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 findMatchingChar(
	const	Buffer &	pSrc,
	const	Uint32		pStart
);

// ============================================================================

/// \brief Returns a copy of \a pSrc, where all control characters (ASCII code
///	< 0x20) have been replaced by their textual code.
///
/// Example: this method converts the string "\x07" into the string "<BEL>".
///
/// \ingroup BFC_Base

BFC_Light_DLL Buffer toPrintable(
	const	Buffer &	pSrc
);

/// \brief Returns a copy of \a pSrc, where all control characters (ASCII code
///	< 0x20) have been replaced by \a pFill (default: a space).
///
/// If \a pSign is true, the sign bit is first set to 0.
///
/// Example: this method converts the string "\x07Hello" into the string " Hello".
///
/// \ingroup BFC_Base

BFC_Light_DLL Buffer keepPrintable( 
	const	Buffer &	pSrc,
	const	Uchar		pFill = ' ',
	const	Bool		pSign = false
);

BFC_Light_DLL Buffer trimNonPrintable( 
	const	Buffer &	pSrc
);

/// \brief Table containing the 16 hexadecimal digits.
///
/// \ingroup BFC_Base

BFC_Light_DLL extern const Uchar hexCharTable[];

/// \brief Returns true iff \a c is a valid hexadecimal digit.
///
/// \ingroup BFC_Base

BFC_Light_DLL Bool isHexChar(
	const	Uchar		c
);

/// \brief Returns a [dot-separated] sequence of 2 hex digits representing the
///	characters of \a src.
///
/// Example: this method converts the string "123" into the string "31.32.33".
///
/// \ingroup BFC_Base

BFC_Light_DLL Buffer toHex(
	const	Buffer &	src,
	const	Bool		addDot = true
);

/// \brief Converts the lowest 4 bits of \a c to an hex char.
///
/// Example: toHexL( 0xAB ) --> 'b'.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uchar toHexL(
	const	Uchar		c
);

/// \brief Converts the highest 4 bits of \a c to an hex char.
///
/// Example: toHexH( 0xAB ) --> 'a'.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uchar toHexH(
	const	Uchar		c
);

/// \brief Converts an hexadecimal character to its decimal integer value.
///
/// Example: fromHex( 'a' ) --> 10.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 fromHex(
	const	Uchar		c
);

/// \brief Converts 2 hexadecimal characters corresponding to the nibbles of
///	a byte to their decimal integer value.
///
/// Example: fromHex( 'a', 'b' ) --> 171.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 fromHex(
	const	Uchar		ch,
	const	Uchar		cl
);

/// \brief Converts a string containing an even number of hexadecimal chars
///	back to its canonical form.
///
/// Example: fromHex( "31.32.33" ) returns "123".
///
/// Example: fromHex( "313233", false ) returns "123".
///
/// \ingroup BFC_Base

BFC_Light_DLL Buffer fromHex(
	const	Buffer &	src,
	const	Bool		dot = true
);

// ============================================================================

} // namespace Utils
} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Utils_H_

// ============================================================================

