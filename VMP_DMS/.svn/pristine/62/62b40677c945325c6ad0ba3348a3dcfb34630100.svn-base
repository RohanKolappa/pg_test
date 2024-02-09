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
//	BFC.Crypto.Functions.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _BFC_Crypto_Functions_H_
#define _BFC_Crypto_Functions_H_

// ============================================================================

#include "BFC.Crypto.DLL.h"

// ============================================================================

#include "BFC.Base.System.h"

// ============================================================================

namespace BFC {
namespace Crypto {

// ============================================================================

inline void STORE32L(
	const	Uint32		x,
		Uchar *		y
) {
	*( Uint32 * )y = System::Host2LE( x );
}

inline Uint32 LOAD32L(
	const	Uchar *		y
) {
	return System::Host2LE( *( const Uint32 * )y );
}

inline void STORE64L(
	const	Uint64		x,
		Uchar *		y
) {
	*( Uint64 * )y = System::Host2LE( x );
}

inline Uint64 LOAD64L(
	const	Uchar *		y
) {
	return System::Host2LE( *( const Uint64 * )y );
}

inline void STORE32H(
	const	Uint32		x,
		Uchar *		y
) {
	*( Uint32 * )y = System::Host2BE( x );
}

inline Uint32 LOAD32H(
	const	Uchar *		y
) {
	return System::Host2BE( *( const Uint32 * )y );
}

inline void STORE64H(
	const	Uint64		x,
		Uchar *		y
) {
	*( Uint64 * )y = System::Host2BE( x );
}

inline Uint64 LOAD64H(
	const	Uchar *		y
) {
	return System::Host2BE( *( const Uint64 * )y );
}

// ============================================================================

#if defined( PLATFORM_WIN32 )
#define CONST64( n ) n ## ui64
#else
#define CONST64( n ) n ## ULL
#endif

// ============================================================================

inline Uint32 BSWAP(
	const	Uint32		x
) {
	return System::swap( x );
}

inline Uint32 ROL(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( ( x << (      ( y & 31 ) ) )
	       | ( x >> ( 32 - ( y & 31 ) ) ) );
}

inline Uint32 ROR(
	const	Uint32		x,
	const	Uint32		y
) {
	return ( ( x >> (      ( y & 31 ) ) )
	       | ( x << ( 32 - ( y & 31 ) ) ) );
}

inline Uint64 ROL64(
	const	Uint64		x,
	const	Uint32		y
) {
	return ( ( x << (      ( y & 63 ) ) )
	       | ( x >> ( 64 - ( y & 63 ) ) ) );
}

inline Uint64 ROR64(
	const	Uint64		x,
	const	Uint32		y
) {
	return ( ( x >> (      ( y & 63 ) ) )
	       | ( x << ( 64 - ( y & 63 ) ) ) );
}

// ============================================================================

inline Uint32 getByte(
	const	Uint32		x,
	const	Uint32		n
) {
	return ( ( x >> ( n << 3 ) ) & 0xFF );
}

inline Uint32 getByte(
	const	Uint64		x,
	const	Uint32		n
) {
	return ( ( x >> ( n << 3 ) ) & 0xFF );
}

// ============================================================================

} // namespace Crypto
} // namespace BFC

// ============================================================================

#endif // _BFC_Crypto_Functions_H_

// ============================================================================

