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
//	BFC.Base64.Encoder.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Base64.Encoder.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Base64, Encoder )

// ============================================================================

Base64::Encoder::Encoder() {



}

Base64::Encoder::~Encoder() {



}

// ============================================================================

const Uchar Base64::Encoder::encode_table[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" /* 0x00 -- 0x19 */
	"abcdefghijklmnopqrstuvwxyz" /* 0x1A -- 0x33 */
	"0123456789+/";              /* 0x34 -- 0x3F */

// ============================================================================

Buffer Base64::Encoder::encode(
	const	Buffer &	iBuf ) const {

	Buffer	oBuf;

	if ( iBuf.isEmpty() ) {
		return oBuf;
	}

	Uint32	iLen = iBuf.getLength();

	oBuf.resize( ( ( iLen + 2 ) / 3 ) * 4 );

	const Uchar *	iPtr = iBuf.getCstPtr();
	Uchar *		oPtr = oBuf.getVarPtr();

	while ( iLen ) {
		if ( iLen >= 3 ) {
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0xFC ) >> 2 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0x03 ) << 4 )
			                      | ( ( iPtr[ 1 ] & 0xF0 ) >> 4 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 1 ] & 0x0F ) << 2 )
			                      | ( ( iPtr[ 2 ] & 0xC0 ) >> 6 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 2 ] & 0x3F )      ) ];
			iLen -= 3;
			iPtr += 3;
		}
		else if (iLen == 2) {
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0xFC ) >> 2 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0x03 ) << 4 )
			                      | ( ( iPtr[ 1 ] & 0xF0 ) >> 4 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 1 ] & 0x0F ) << 2 ) ];
			*oPtr++ = '=';
			break;
		}
		else { // iLen == 1
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0xFC ) >> 2 ) ];
			*oPtr++ = encode_table[ ( ( iPtr[ 0 ] & 0x03 ) << 4 ) ];
			*oPtr++ = '=';
			*oPtr++ = '=';
			break;
		}
	}

	return oBuf;

}

// ============================================================================

