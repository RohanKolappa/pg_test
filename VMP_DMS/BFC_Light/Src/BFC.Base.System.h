// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	BFC.Base.System.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_System_H_
#define _BFC_Base_System_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Endian.h"
#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {
namespace System {

// ============================================================================

#if defined( PLATFORM_CYGWIN ) || defined( PLATFORM_WIN32 )

/// \brief Returns a value with all bytes in the 64 bit argument swapped.
///
/// \ingroup BFC_Base

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
/// \ingroup BFC_Base

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
/// \ingroup BFC_Base

inline Uint16 bswap_16(
	const	Uint16		v
) {
	return ( ( v & 0xFF00 ) >> 8 )
	     | ( ( v & 0x00FF ) << 8 );
}

#endif

// ============================================================================

inline Uint16 cst_bswap_16(
	const	Uint16		v
) {
#if defined( __bswap_constant_16 )
	return ( Uint16 )__bswap_constant_16( v );
#else
	return bswap_16( v );
#endif
}

inline Uint32 cst_bswap_32(
	const	Uint32		v
) {
#if defined( __bswap_constant_32 )
	return ( Uint32 )__bswap_constant_32( v );
#else
	return bswap_32( v );
#endif
}

inline Uint64 cst_bswap_64(
	const	Uint64		v
) {
#if defined( __bswap_constant_64 )
	return ( Uint64 )__bswap_constant_64( v );
#else
	return bswap_64( v );
#endif
}

// ============================================================================

/// \brief Allocates memory for an array of \a pNbr elements of \a pSze bytes
///	each.
///
/// The memory is set to zero.
///
/// \returns
///	A pointer to the allocated memory.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uchar * calloc(
	const	Uint32		pNbr,
	const	Uint32		pSze
);

/// \brief Allocates \a pLen bytes and returns a pointer to the
///	allocated memory.
///
/// The memory is not cleared.
///
/// \param pLen
///	The number of bytes to allocate.
///
/// \returns
///	A pointer to the allocated memory.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uchar * malloc(
	const	Uint32		pLen
);

/// \brief Changes the size of the memory block pointed to by \a pPtr
///	to \a pLen bytes.
///
/// The contents will be unchanged to the minimum of the old and new
/// sizes.
///
/// Newly allocated memory will be uninitialized.
///
/// If \a pPtr is NULL, the call is equivalent to
/// "malloc( pLen )". If \a pLen is equal to zero, the call
/// is equivalent to "free( pPtr )".
///
/// Unless \a pPtr is NULL, it must have been returned by an earlier
/// call to malloc() or realloc().
//
/// \ingroup BFC_Base

BFC_Light_DLL Uchar * realloc(
		Uchar *		pPtr,
	const	Uint32		pLen
);

/// \brief Frees the memory space pointed to by \a pPtr.
///
/// \a pPtr must have been returned by a previous call to malloc()
/// or realloc(). Otherwise, or if "free( pPtr )" has already been
/// called before, undefined behaviour occurs.
///
/// If \a pPtr is NULL, no operation is performed.
///
/// \ingroup BFC_Base

BFC_Light_DLL void free(
		Uchar *		pPtr
);

// ============================================================================

/// \brief Copies \a pLen bytes from memory area \a pSrc to memory
///	area \a pDst.
///
/// The memory areas may not overlap! Use memmove() if the memory
/// areas do overlap.
///
/// \ingroup BFC_Base

BFC_Light_DLL void memcpy(
		Uchar *		pDst,
	const	Uchar *		pSrc,
	const	Uint32		pLen
);

/// \brief Copies \a pLen bytes from memory area \a pSrc to memory
///	area \a pDst.
///
/// The memory areas may overlap.
///
/// \ingroup BFC_Base

BFC_Light_DLL void memmove(
		Uchar *		pDst,
	const	Uchar *		pSrc,
	const	Uint32		pLen
);

/// \brief Fills the first \a pLen bytes of the memory area \a pDst
///	with the constant char \a pChr.
///
/// \ingroup BFC_Base

BFC_Light_DLL void memset(
		Uchar *		pDst,
	const	Uchar		pChr,
	const	Uint32		pLen
);

/// \brief Sets the first \a pLen bytes of the memory area \a pDst
///	to zero.
///
/// \ingroup BFC_Base

BFC_Light_DLL void bzero(
		Uchar *		pDst,
	const	Uint32		pLen
);

/// \brief Calculates the length of the string \a pPtr, not including
///	the terminating '\\0' character.
///
/// \ingroup BFC_Base

BFC_Light_DLL Uint32 strlen(
	const	Uchar *		pPtr
);

/// \brief Compares the two strings \a pS1 and \a pS2, and returns
///	an integer less than, equal to, or greater than zero if
///	\a pS1 is found, respectively, to be less than, to match, or
///	be greater than \a pS2.
///
/// \ingroup BFC_Base

BFC_Light_DLL Int32 strcmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2
);

/// \brief Similar to strcmp(), but compares the first (at most)
///	\a pLen characters of \a pS1 and \a pS2.
///
/// \ingroup BFC_Base

BFC_Light_DLL Int32 strncmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen
);

/// \brief Compares the two strings \a pS1 and \a pS2, ignoring the case of the
///	characters, and returns an integer less than, equal to, or greater than
///	zero if \a pS1 is found, respectively, to be less than, to match, or be
///	greater than \a pS2.
///
/// \ingroup BFC_Base

BFC_Light_DLL Int32 strcasecmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2
);

/// \brief Similar to strcasecmp(), except it only compares the first \a pLen
///	characters of \a pS1 and \a pS2.
///
/// \ingroup BFC_Base

BFC_Light_DLL Int32 strncasecmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen
);

/// \brief Compares the first \a pLen bytes of the memory areas \a pS1 and
///	\a pS2, and returns an integer less than, equal to, or greater than
///	zero if \a pS1 is found, respectively, to be less than, to match, or be
///	greater than \a pS2.
///
/// \ingroup BFC_Base

BFC_Light_DLL Int32 memcmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen
);

// ============================================================================

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Uint64 swap(
	const	Uint64		v
) {
	return cst_bswap_64( v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Uint32 swap(
	const	Uint32		v
) {
	return cst_bswap_32( v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Uint16 swap(
	const	Uint16		v
) {
	return cst_bswap_16( v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Int64 swap(
	const	Int64		v
) {
	return ( Int64 )cst_bswap_64( ( Uint64 )v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Int32 swap(
	const	Int32		v
) {
	return ( Int32 )cst_bswap_32( ( Uint32 )v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Int16 swap(
	const	Int16		v
) {
	return ( Int16 )cst_bswap_16( ( Uint16 )v );
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Double swap(
	const	Double		v
) {
	union {
		Uint64	u;
		Double	d;
	} us, ud;
	us.d = v;
	ud.u = cst_bswap_64( us.u );
	return ud.d;
}

/// \brief Swaps the bytes of \a v.
///
/// \ingroup BFC_Base

inline Float swap(
	const	Float		v
) {
	union {
		Uint32	u;
		Float	d;
	} us, ud;
	us.d = v;
	ud.u = cst_bswap_32( us.u );
	return ud.d;
}

// ============================================================================

#if BYTE_ORDER == LITTLE_ENDIAN
#define SWAP_IF_LE( v ) BFC::System::swap( v )
#define SWAP_IF_BE( v ) ( v )
#else
#define SWAP_IF_LE( v ) ( v )
#define SWAP_IF_BE( v ) BFC::System::swap( v )
#endif

// ============================================================================

/// \brief Returns true if the executing processor is little-endian, false
///	otherwise.

inline Bool isLE(
) {
#if BYTE_ORDER == LITTLE_ENDIAN
	return true;
#else
	return false;
#endif
}

/// \brief Returns true if the executing processor is big-endian, false
///	otherwise.

inline Bool isBE(
) {
#if BYTE_ORDER == LITTLE_ENDIAN
	return false;
#else
	return true;
#endif
}

// ============================================================================

/// \brief Converts \a v from big-endian to host-endian.
///
/// \ingroup BFC_Base

template < typename T >
inline T BE2Host(
	const	T		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from little-endian to host-endian.
///
/// \ingroup BFC_Base

template < typename T >
inline T LE2Host(
	const	T		v
) {
	return SWAP_IF_BE( v );
}

/// \brief Converts \a v from host-endian to big-endian.
///
/// \ingroup BFC_Base

template < typename T >
inline T Host2BE(
	const	T		v
) {
	return SWAP_IF_LE( v );
}

/// \brief Converts \a v from host-endian to little-endian.
///
/// \ingroup BFC_Base

template < typename T >
inline T Host2LE(
	const	T		v
) {
	return SWAP_IF_BE( v );
}

// ============================================================================

/// \brief Makes the current process/thread sleep until \a pSecs
///	seconds have elapsed.
///
/// \ingroup BFC_Base

BFC_Light_DLL void sleep(
	const	Uint32		pSecs
);

/// \brief Makes the current process/thread sleep until \a pUSecs
///	microseconds have elapsed.
///
/// \ingroup BFC_Base

BFC_Light_DLL void usleep(
	const	Uint64		pUsecs
);

/// \brief Makes the current process/thread sleep until at most \a pSecs
///	seconds have elapsed.
///
/// \ingroup BFC_Base

BFC_Light_DLL void sleepRand(
	const	Uint32		pSecs
);

/// \brief Makes the current process/thread sleep until at most \a pUSecs
///	microseconds have elapsed.
///
/// \ingroup BFC_Base

BFC_Light_DLL void usleepRand(
	const	Uint64		pUsecs
);

// ============================================================================

} // namespace System
} // namespace BFC

// ============================================================================

#endif // _BFC_Base_System_H_

// ============================================================================

