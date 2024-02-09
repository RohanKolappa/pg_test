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
//	TBFC.Base.Utils.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_Utils_H_
#define _TBFC_Base_Utils_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.Buffer.h"

// ============================================================================

namespace TBFC {
namespace Utils {

// ============================================================================

/// \brief Updates \a __pos to point to the first non white space character in
///	\a __src, starting from the initial value of \a __pos.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void skipWhiteSpaces(
	const	Buffer		& __src,
		Uint32		& __pos
);

/// \brief Updates \a __pos to point to the first white space character in
///	\a __src, starting from the initial value of \a __pos.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void skipNonWhiteSpaces(
	const	Buffer		& __src,
		Uint32		& __pos
);

/// \brief Finds the closing '"' character, starting at \a __start + 1.
///
/// \a __src must contain a '"' character at position \a __start.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Uint32 findMatchingChar(
	const	Buffer		& __src,
	const	Uint32		__start
);

// ============================================================================

/// \brief Returns a copy of \a __src, where all control characters (ASCII code
///	< 0x20) have been replaced by their textual code.
///
/// Example: this method converts the string "\x07" into the string "<BEL>".
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Buffer toPrintable(
	const	Buffer		& __src
);

/// \brief Table containing the 16 hexadecimal digits.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL extern const Uchar hexCharTable[];

/// \brief Returns a dot separated sequence of 2 hex digits representing the
///	characters of \a __src.
///
/// Example: this method converts the string "123" into the string "31.32.33".
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Buffer toHex(
	const	Buffer		& __src
);

// ============================================================================

} // namespace Utils
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_Utils_H_

// ============================================================================

