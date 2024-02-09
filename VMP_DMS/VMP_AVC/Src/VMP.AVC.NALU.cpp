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
//	VMP.AVC.NALU.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.AVC.NALU.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( AVC, NALU )

// ============================================================================

const Buffer & AVC::NALU::getTypeName(
	const	Uchar		pType ) {

	static const Buffer tbl[] = {

		"",		// 0x00
		"Slice",	// 0x01
		"DPA",		// 0x02
		"DPB",		// 0x03
		"DPC",		// 0x04
		"IDR",		// 0x05
		"SEI",		// 0x06
		"SPS",		// 0x07
		"PPS",		// 0x08
		"AUD",		// 0x09
		"EOSeq",	// 0x0A
		"EOStream",	// 0x0B
		"Fill",		// 0x0C
		"SPSExt",	// 0x0D
		"Prefix",	// 0x0E
		"SubSPS",	// 0x0F
		"",		// 0x10
		"",		// 0x11
		"",		// 0x12
		"AuxSlice",	// 0x13
		"SliceExt"	// 0x14

	};

	if ( pType == 0x00
	  || ( pType >= 0x10 && pType <= 0x12 )
	  || pType >= 0x15 ) {
		throw InvalidArgument();
	}

	return tbl[ pType ];

}

// ============================================================================

Buffer AVC::NALU::getRBSP() const {

	if ( data.isEmpty() ) {
		throw InternalError();
	}

	const Uchar *	src = data.getCstPtr() + 1;
	Uint32		len = data.getLength() - 1;
	Buffer		res;

	res.resize( len );

	Uchar *		dst = res.getVarPtr();
	Uint32		cnt = 0;
	Uint32		pos = 0;

	for ( Uint32 i = 0 ; i < len ; i++ ) {

		// In NAL unit, 0x000000, 0x000001 or 0x000002 shall not occur
		// at any byte-aligned position.

		Uchar chr = src[ i ];

		if ( cnt == 2 ) {
// FIXME: The following test is there only to support the "Antrica" test case,
// FIXME: which uses SPS's containing long sequences of 0x00 at the end of the
// FIXME: NALU.
			if ( chr == 0x00 ) {
				Bool ok = true;
				for ( Uint32 j = i ; j < len ; j++ ) {
					if ( src[ j ] ) {
						ok = false;
						break;
					}
				}
				if ( ok ) {
					pos -= 2;
					break;
				}
			}
			if ( chr < 0x03 ) {
				throw InternalError( "Invalid 0x00000x sequence in NAL unit!" );
			}
			if ( chr == 0x03 ) {
				// Check the 4th byte after 0x000003, except
				// when cabac_zero_word is used, in which case
				// the last three bytes of this NAL unit must
				// be 0x000003.
				if ( i < len - 1 && src[ i + 1 ] > 0x03 ) {
					throw InternalError( "invalid byte after 0x000002 sequence in NAL unit!" );
				}
				// If cabac_zero_word is used, the final byte
				// of this NAL unit (0x03) is discarded, and the last
				// two bytes of RBSP must be 0x0000.
				if ( i == len - 1 ) {
					break;
				}
				i++;
				chr = src[ i ];
				cnt = 0;
			}
		}

		dst[ pos++ ] = chr;

		if ( chr == 0x00 ) {
			cnt++;
		}
		else {
			cnt = 0;
		}

	}

	res.resize( pos );

	return res;

}

// ============================================================================

