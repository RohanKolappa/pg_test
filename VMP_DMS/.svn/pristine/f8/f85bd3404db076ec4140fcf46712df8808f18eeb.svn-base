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
//	TBFC.Base.System.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_System_H_
#define _TBFC_Base_System_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

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

#include "TBFC.Base.Types.h"

// ============================================================================

namespace TBFC {
namespace System {

// ============================================================================

#if defined( PLATFORM_CYGWIN ) || defined( PLATFORM_WIN32 )

/// \brief Returns a value with all bytes in the 64 bit argument swapped.
///
/// \ingroup TBFC_Base

inline Uint64 bswap_64(
	const	Uint64		v
) {
	return ( ( v & 0xFF00000000000000ull ) >> 56 )
	     | ( ( v & 0x00FF000000000000ull ) >> 40 )
	     | ( ( v & 0x0000FF0000000000ull ) >> 24 )
	     | ( ( v & 0x000000FF00000000ull ) >>  8 )
	     | ( ( v & 0x00000000FF000000ull ) <<  8 )
	     | ( ( v & 0x0000000000FF0000ull ) << 24 )
	     | ( ( v & 0x000000000000FF00ull ) << 40 )
	     | ( ( v & 0x00000000000000FFull ) << 56 );
}

/// \brief Returns a value with all bytes in the 32 bit argument swapped.
///
/// \ingroup TBFC_Base

inline Uint32 bswap_32(
	const	Uint32		v
) {
	return ( ( v & 0xFF000000 ) >> 24 )
	     | ( ( v & 0x00FF0000 ) >>  8 )
	     | ( ( v & 0x0000FF00 ) <<  8 )
	     | ( ( v & 0x000000FF ) << 24 );
}

/// \brief Returns a value with all bytes in the 16 bit argument swapped.
///
/// \ingroup TBFC_Base

inline Uint16 bswap_16(
	const	Uint16		v
) {
	return ( ( v & 0xFF00 ) >> 8 )
	     | ( ( v & 0x00FF ) << 8 );
}

#endif

// ============================================================================

/// \brief Allocates \a __len bytes and returns a pointer to the
///	allocated memory.
///
/// The memory is not cleared.
///
/// \param __len
///	The number of bytes to allocate.
///
/// \returns
///	A pointer to the allocated memory.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Uchar * malloc(
	const	Uint32		__len
);

/// \brief Changes the size of the memory block pointed to by \a __ptr
///	to \a __len bytes.
///
/// The contents will be unchanged to the minimum of the old and new
/// sizes.
///
/// Newly allocated memory will be uninitialized.
///
/// If \a __ptr is NULL, the call is equivalent to
/// "malloc( __len )". If \a __len is equal to zero, the call
/// is equivalent to "free( __ptr )".
///
/// Unless \a __ptr is NULL, it must have been returned by an earlier
/// call to malloc() or realloc().
//
/// \ingroup TBFC_Base

TBFC_Light_DLL Uchar * realloc(
		Uchar		* __ptr,
	const	Uint32		__len
);

/// \brief Frees the memory space pointed to by \a __ptr.
///
/// \a __ptr must have been returned by a previous call to malloc()
/// or realloc(). Otherwise, or if "free( __ptr )" has already been
/// called before, undefined behaviour occurs.
///
/// If \a __ptr is NULL, no operation is performed.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void free(
		Uchar		* __ptr
);

/// \brief Copies \a __len bytes from memory area \a __src to memory
///	area \a __dst.
///
/// The memory areas may not overlap! Use memmove() if the memory
/// areas do overlap.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void memcpy(
		Uchar		* __dst,
	const	Uchar		* __src,
	const	Uint32		__len
);

/// \brief Copies \a __len bytes from memory area \a __src to memory
///	area \a __dst.
///
/// The memory areas may overlap.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void memmove(
		Uchar		* __dst,
	const	Uchar		* __src,
	const	Uint32		__len
);

/// \brief Fills the first \a __len bytes of the memory area \a __dst
///	with the constant char \a __c.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void memset(
		Uchar		* __dst,
	const	Uchar		__c,
	const	Uint32		__len
);

/// \brief Sets the first \a __len bytes of the memory area \a __dst
///	to zero.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void bzero(
		Uchar		* __dst,
	const	Uint32		__len
);

/// \brief Calculates the length of the string \a __ptr, not including
///	the terminating '\\0' character.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Uint32 strlen(
	const	Uchar		* __ptr
);

/// \brief Compares the two strings \a __s1 and \a __s2, and returns
///	an integer less than, equal to, or greater than zero if
///	\a __s1 is found, respectively, to be less than, to match, or
///	be greater than \a __s2.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Int32 strcmp(
	const	Uchar		* __s1,
	const	Uchar		* __s2
);

/// \brief Similar to strcmp(), but compares the first (at most)
///	\a __len characters of \a __s1 and \a __s2.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL Int32 strncmp(
	const	Uchar		* __s1,
	const	Uchar		* __s2,
	const	Uint32		__len
);

// ============================================================================

/// \brief Swaps the bytes of \a v.
///
/// \ingroup TBFC_Base

inline Uint64 swap(
	const	Uint64		v
) {
	return bswap_64( v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup TBFC_Base

inline Uint32 swap(
	const	Uint32		v
) {
	return bswap_32( v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup TBFC_Base

inline Uint16 swap(
	const	Uint16		v
) {
	return bswap_16( v );
}

// ============================================================================

#if BYTE_ORDER == LITTLE_ENDIAN
#define SWAP_IF_LE( v ) swap( v )
#define SWAP_IF_BE( v ) v
#else
#define SWAP_IF_LE( v ) v
#define SWAP_IF_BE( v ) swap( v )
#endif

// ============================================================================

/// \brief Converts \a v from host-endian to network-endian.
///
/// \ingroup TBFC_Base

inline Uint64 hostToNetwork(
	const	Uint64		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from network-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint64 networkToHost(
	const	Uint64		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from big-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint64 BE2Host(
	const	Uint64		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from little-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint64 LE2Host(
	const	Uint64		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to big-endian.
///
/// \ingroup TBFC_Base

inline Uint64 Host2BE(
	const	Uint64		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from host-endian to little-endian.
///
/// \ingroup TBFC_Base

inline Uint64 Host2LE(
	const	Uint64		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to network-endian.
///
/// \ingroup TBFC_Base

inline Uint32 hostToNetwork(
	const	Uint32		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from network-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint32 networkToHost(
	const	Uint32		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from big-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint32 BE2Host(
	const	Uint32		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from little-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint32 LE2Host(
	const	Uint32		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to big-endian.
///
/// \ingroup TBFC_Base

inline Uint32 Host2BE(
	const	Uint32		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from host-endian to little-endian.
///
/// \ingroup TBFC_Base

inline Uint32 Host2LE(
	const	Uint32		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to network-endian.
///
/// \ingroup TBFC_Base

inline Uint16 hostToNetwork(
	const	Uint16		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from network-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint16 networkToHost(
	const	Uint16		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from big-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint16 BE2Host(
	const	Uint16		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from little-endian to host-endian.
///
/// \ingroup TBFC_Base

inline Uint16 LE2Host(
	const	Uint16		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to big-endian.
///
/// \ingroup TBFC_Base

inline Uint16 Host2BE(
	const	Uint16		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from host-endian to little-endian.
///
/// \ingroup TBFC_Base

inline Uint16 Host2LE(
	const	Uint16		v
) {
	return SWAP_IF_BE( v );
}

// ============================================================================

/// \brief Makes the current process/thread sleep until \a __secs
///	seconds have elapsed.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void sleep(
	const	Uint32		__secs
);

/// \brief Suspends execution of the calling process/thread for (at
///	least) \a __usecs microseconds.
///
/// The sleep may be lengthened slightly by any system activity or by
/// the time spent processing the call or by the granularity of system
/// timers.
///
/// \ingroup TBFC_Base

TBFC_Light_DLL void usleep(
	const	Uint64		__usecs
);

// ============================================================================

} // namespace System
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_System_H_

// ============================================================================

