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
//	BFC.Base.Endian.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_Endian_H_
#define _BFC_Base_Endian_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

/* Definitions for byte order, according to significance of bytes,
   from low addresses to high addresses.  The value is what you get by
   putting '4' in the most significant byte, '3' in the second most
   significant byte, '2' in the second least significant byte, and '1'
   in the least significant byte, and then writing down one digit for
   each byte, starting with the byte at the lowest address at the left,
   and proceeding to the byte with the highest address at the right.  */

#if defined( PLATFORM_CYGWIN )
#  include <sys/param.h>
#endif

#if defined( PLATFORM_LINUX )
#  include <byteswap.h>
#  include <endian.h>
#  ifndef BYTE_ORDER
#    define	BYTE_ORDER	__BYTE_ORDER
#    define	LITTLE_ENDIAN	__LITTLE_ENDIAN
#    define	BIG_ENDIAN	__BIG_ENDIAN
#  endif // BYTE_ORDER
#endif

#if defined( PLATFORM_DARWIN )
#include <libkern/OSByteOrder.h>
#define bswap_16 OSSwapInt16
#define bswap_32 OSSwapInt32
#define bswap_64 OSSwapInt64
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

// ============================================================================

#endif // _BFC_Base_Endian_H_

// ============================================================================

