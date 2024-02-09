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
//	BFC.Base.WChar.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_WChar_H_
#define _BFC_Base_WChar_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Represents a wide character.
///
/// This class provides several methods for determining a character's category
/// (lowercase letter, digit, etc.) and for converting characters from
/// uppercase to lowercase and vice versa.
///
/// \ingroup BFC_Base

class BFC_Light_DLL WChar {

public :

	/// \brief Creates a new WChar.

	WChar(
	) :
		c	( 0 )
	{
	}

	/// \brief Creates a new WChar.

	WChar(
		const	Uchar		pValue
	) :
		c	( pValue )
	{
	}

	/// \brief Creates a new WChar.

	WChar(
		const	Uint32		pValue
	) :
		c	( pValue )
	{
	}

	/// \brief Creates a new WChar.

	WChar(
		const	int		pValue
	) :
		c	( ( Uint32 )pValue )
	{
	}

	/// \brief Copy constructor.

	WChar(
		const	WChar &		pOther
	) :
		c	( pOther.c )
	{
	}

	/// \brief Copy operator.

	WChar & operator = (
		const	WChar &		pOther
	) {
		c = pOther.c;
		return *this;
	}

//	Bool operator == (
//		const	WChar &		pOther
//	) const {
//		return ( c == pOther.c );
//	}
//
//	Bool operator != (
//		const	WChar &		pOther
//	) const {
//		return ( c != pOther.c );
//	}
//
//	Bool operator <= (
//		const	WChar &		pOther
//	) const {
//		return ( c <= pOther.c );
//	}
//
//	Bool operator < (
//		const	WChar &		pOther
//	) const {
//		return ( c < pOther.c );
//	}
//
//	Bool operator >= (
//		const	WChar &		pOther
//	) const {
//		return ( c >= pOther.c );
//	}
//
//	Bool operator > (
//		const	WChar &		pOther
//	) const {
//		return ( c > pOther.c );
//	}

	Uint32 getValue(
	) const {
		return c;
	}

	operator Uint32 (
	) const {
		return c;
	}

	Bool isDigit(
	) const {
		return ( c >= 0x30
		      && c <= 0x39 );
	}

protected :

private :

	Uint32		c;

};

// ============================================================================

const WChar chNull                  = 0x0000;
const WChar chHTab                  = 0x0009;
const WChar chLF                    = 0x000A;
const WChar chVTab                  = 0x000B;
const WChar chFF                    = 0x000C;
const WChar chCR                    = 0x000D;
const WChar chAmpersand             = 0x0026;
const WChar chAsterisk              = 0x002A;
const WChar chAt                    = 0x0040;
const WChar chBackSlash             = 0x005C;
const WChar chBang                  = 0x0021;
const WChar chCaret                 = 0x005E;
const WChar chCloseAngle            = 0x003E;
const WChar chCloseCurly            = 0x007D;
const WChar chCloseParen            = 0x0029;
const WChar chCloseSquare           = 0x005D;
const WChar chColon                 = 0x003A;
const WChar chComma                 = 0x002C;
const WChar chDash                  = 0x002D;
const WChar chDollarSign            = 0x0024;
const WChar chDoubleQuote           = 0x0022;
const WChar chEqual                 = 0x003D;
const WChar chForwardSlash          = 0x002F;
const WChar chGrave                 = 0x0060;
const WChar chNEL                   = 0x0085;
const WChar chOpenAngle             = 0x003C;
const WChar chOpenCurly             = 0x007B;
const WChar chOpenParen             = 0x0028;
const WChar chOpenSquare            = 0x005B;
const WChar chPercent               = 0x0025;
const WChar chPeriod                = 0x002E;
const WChar chPipe                  = 0x007C;
const WChar chPlus                  = 0x002B;
const WChar chPound                 = 0x0023;
const WChar chQuestion              = 0x003F;
const WChar chSingleQuote           = 0x0027;
const WChar chSpace                 = 0x0020;
const WChar chSemiColon             = 0x003B;
const WChar chTilde                 = 0x007E;
const WChar chUnderscore            = 0x005F;

const WChar chSwappedUnicodeMarker  = 0xFFFE;
const WChar chUnicodeMarker         = 0xFEFF;

const WChar chDigit_0               = 0x0030;
const WChar chDigit_1               = 0x0031;
const WChar chDigit_2               = 0x0032;
const WChar chDigit_3               = 0x0033;
const WChar chDigit_4               = 0x0034;
const WChar chDigit_5               = 0x0035;
const WChar chDigit_6               = 0x0036;
const WChar chDigit_7               = 0x0037;
const WChar chDigit_8               = 0x0038;
const WChar chDigit_9               = 0x0039;

const WChar chLatin_A               = 0x0041;
const WChar chLatin_B               = 0x0042;
const WChar chLatin_C               = 0x0043;
const WChar chLatin_D               = 0x0044;
const WChar chLatin_E               = 0x0045;
const WChar chLatin_F               = 0x0046;
const WChar chLatin_G               = 0x0047;
const WChar chLatin_H               = 0x0048;
const WChar chLatin_I               = 0x0049;
const WChar chLatin_J               = 0x004A;
const WChar chLatin_K               = 0x004B;
const WChar chLatin_L               = 0x004C;
const WChar chLatin_M               = 0x004D;
const WChar chLatin_N               = 0x004E;
const WChar chLatin_O               = 0x004F;
const WChar chLatin_P               = 0x0050;
const WChar chLatin_Q               = 0x0051;
const WChar chLatin_R               = 0x0052;
const WChar chLatin_S               = 0x0053;
const WChar chLatin_T               = 0x0054;
const WChar chLatin_U               = 0x0055;
const WChar chLatin_V               = 0x0056;
const WChar chLatin_W               = 0x0057;
const WChar chLatin_X               = 0x0058;
const WChar chLatin_Y               = 0x0059;
const WChar chLatin_Z               = 0x005A;

const WChar chLatin_a               = 0x0061;
const WChar chLatin_b               = 0x0062;
const WChar chLatin_c               = 0x0063;
const WChar chLatin_d               = 0x0064;
const WChar chLatin_e               = 0x0065;
const WChar chLatin_f               = 0x0066;
const WChar chLatin_g               = 0x0067;
const WChar chLatin_h               = 0x0068;
const WChar chLatin_i               = 0x0069;
const WChar chLatin_j               = 0x006A;
const WChar chLatin_k               = 0x006B;
const WChar chLatin_l               = 0x006C;
const WChar chLatin_m               = 0x006D;
const WChar chLatin_n               = 0x006E;
const WChar chLatin_o               = 0x006F;
const WChar chLatin_p               = 0x0070;
const WChar chLatin_q               = 0x0071;
const WChar chLatin_r               = 0x0072;
const WChar chLatin_s               = 0x0073;
const WChar chLatin_t               = 0x0074;
const WChar chLatin_u               = 0x0075;
const WChar chLatin_v               = 0x0076;
const WChar chLatin_w               = 0x0077;
const WChar chLatin_x               = 0x0078;
const WChar chLatin_y               = 0x0079;
const WChar chLatin_z               = 0x007A;

const WChar chYenSign               = 0x00A5;
const WChar chWonSign               = 0x20A9;

const WChar chLineSeparator         = 0x2028;
const WChar chParagraphSeparator    = 0x2029;

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_WChar_H_

// ============================================================================

