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
//	BFC.Base.Uint24.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Base_Uint24_H_
#define _BFC_Base_Uint24_H_

// ============================================================================

#include "BFC.Base.Endian.h"
#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief Emulation of a 3-bytes (24 bits) unsigned integer.
///
/// \ingroup BFC_Base

class Uint24 {

public:

	Uint24(
	) :
		b0( 0 ),
		b1( 0 ),
		b2( 0 )
	{
	}

	Uint24(
		const	Uint32		a
	) :
#if ( BYTE_ORDER == LITTLE_ENDIAN )
		b0( ( Uchar )( a       ) ),
		b1( ( Uchar )( a >>  8 ) ),
		b2( ( Uchar )( a >> 16 ) )
#else
		b0( ( Uchar )( a >> 16 ) ),
		b1( ( Uchar )( a >>  8 ) ),
		b2( ( Uchar )( a       ) )
#endif
	{
	}

	Uint24(
		const	Uchar		pb0,
		const	Uchar		pb1,
		const	Uchar		pb2
	) :
		b0( pb0 ),
		b1( pb1 ),
		b2( pb2 )
	{
	}

	Uint24(
		const	Uint24 &	pOther
	) :
		b0( pOther.b0 ),
		b1( pOther.b1 ),
		b2( pOther.b2 )
	{
	}

	Uint24 & operator = (
		const	Uint24 &	pOther
	) {
		b0 = pOther.b0;
		b1 = pOther.b1;
		b2 = pOther.b2;
		return * this;
	}

	operator Uint32 (
	) const {
#if ( BYTE_ORDER == LITTLE_ENDIAN )
		return ( ( Uint32 )b0       )
		     | ( ( Uint32 )b1 <<  8 )
		     | ( ( Uint32 )b2 << 16 );
#else
		return ( ( Uint32 )b0 << 16 )
		     | ( ( Uint32 )b1 <<  8 )
		     | ( ( Uint32 )b2       );
#endif
	}

	Uchar getByte0() const { return b0; }
	Uchar getByte1() const { return b1; }
	Uchar getByte2() const { return b2; }

private :

	Uchar	b0; // LSB if little-endian
	Uchar	b1;
	Uchar	b2; // MSB if little-endian

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_Uint24_H_

// ============================================================================

