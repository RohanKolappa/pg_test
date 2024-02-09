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
//	BFC.Base.System.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_System_H_
#define _BFC_System_H_

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

// Find out platform endianness...

#ifdef PLATFORM_CYGWIN
#  include <sys/param.h>
#endif

#ifdef PLATFORM_LINUX
#  include <byteswap.h>
#  include <endian.h>
#  ifndef BYTE_ORDER
#    define	BYTE_ORDER	__BYTE_ORDER
#    define	LITTLE_ENDIAN	__LITTLE_ENDIAN
#    define	BIG_ENDIAN	__BIG_ENDIAN
#  endif // BYTE_ORDER
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

// ============================================================================

/// A set of static helper functions.

///
/// \ingroup Common_Base

class JBS_DLL_NONE System {

public :

	System();

	virtual ~System();

	//	Memory [de]allocation
	//	---------------------

	static Uchar *malloc(
		const	Uint32	size
	);

	static Uchar *realloc(
			Uchar	*ptr,
		const	Uint32	size
	);

	static void free(
			Uchar	*ptr
	);

	//	String manipulation
	//	-------------------

	static void memcpy(
			Uchar	*dest,
		const	Uchar	*src,
		const	Uint32	len
	);

	static void memset(
			Uchar	*dest,
		const	Uchar	c,
		const	Uint32	len
	);

	static void bzero(
			Uchar	*dest,
		const	Uint32	len
	);

	static Uint32 strlen(
		const	Uchar	*p
	);

	//	Byte-swapping
	//	-------------

	static Uint64 swap( const Uint64 v ) { return bswap_64( v ); }
	static Uint32 swap( const Uint32 v ) { return bswap_32( v ); }
	static Uint16 swap( const Uint16 v ) { return bswap_16( v ); }

#if BYTE_ORDER == LITTLE_ENDIAN

	static Uint64 hostToNetwork(const Uint64 v) {return bswap_64(v);}
	static Uint32 hostToNetwork(const Uint32 v) {return bswap_32(v);}
	static Uint16 hostToNetwork(const Uint16 v) {return bswap_16(v);}

	static Uint64 networkToHost(const Uint64 v) {return bswap_64(v);}
	static Uint32 networkToHost(const Uint32 v) {return bswap_32(v);}
	static Uint16 networkToHost(const Uint16 v) {return bswap_16(v);}

	static Uint64 BE2Host(const Uint64 v) {return bswap_64(v);}
	static Uint32 BE2Host(const Uint32 v) {return bswap_32(v);}
	static Uint16 BE2Host(const Uint16 v) {return bswap_16(v);}

	static Uint64 LE2Host(const Uint64 v) {return v;}
	static Uint32 LE2Host(const Uint32 v) {return v;}
	static Uint16 LE2Host(const Uint16 v) {return v;}

	static Uint64 Host2BE(const Uint64 v) {return bswap_64(v);}
	static Uint32 Host2BE(const Uint32 v) {return bswap_32(v);}
	static Uint16 Host2BE(const Uint16 v) {return bswap_16(v);}

	static Uint64 Host2LE(const Uint64 v) {return v;}
	static Uint32 Host2LE(const Uint32 v) {return v;}
	static Uint16 Host2LE(const Uint16 v) {return v;}

#else

	static Uint64 hostToNetwork(const Uint64 v) {return v;}
	static Uint32 hostToNetwork(const Uint32 v) {return v;}
	static Uint16 hostToNetwork(const Uint16 v) {return v;}

	static Uint64 networkToHost(const Uint64 v) {return v;}
	static Uint32 networkToHost(const Uint32 v) {return v;}
	static Uint16 networkToHost(const Uint16 v) {return v;}

	static Uint64 LE2Host(const Uint64 v) {return bswap_64(v);}
	static Uint32 LE2Host(const Uint32 v) {return bswap_32(v);}
	static Uint16 LE2Host(const Uint16 v) {return bswap_16(v);}

	static Uint64 BE2Host(const Uint64 v) {return v;}
	static Uint32 BE2Host(const Uint32 v) {return v;}
	static Uint16 BE2Host(const Uint16 v) {return v;}

	static Uint64 Host2LE(const Uint64 v) {return bswap_64(v);}
	static Uint32 Host2LE(const Uint32 v) {return bswap_32(v);}
	static Uint16 Host2LE(const Uint16 v) {return bswap_16(v);}

	static Uint64 Host2BE(const Uint64 v) {return v;}
	static Uint32 Host2BE(const Uint32 v) {return v;}
	static Uint16 Host2BE(const Uint16 v) {return v;}

#endif

	//	Timers
	//	------

	static void sleep(
		const	Uint32	secs
	);

	static void usleep(
		const	Uint64	usecs
	);

	static Uint32 getRandMax();

	/// \brief Returns a pseudo-random integer between 0 and getRandMax() (incl).

	static Uint32 rand();

	/// Returns a pseudo-random integer between 0 and range (incl).

	static Uint32 rand(
		const	Uint32	range
	);

	static Bool randBool();

	/*
	 *	void srand(Uint32 seed)
	 *
	 *	Sets its argument as the seed for a new sequence of
	 *	pseudo-random integers to be returned by rand().  These
	 *	sequences are repeatable by calling srand() with the same seed
	 *	value. If no seed value is provided, the rand() function is
	 *	automatically seeded with a value of 1.
	 */

	static void srand(
		const	Uint32	seed
	);

	static void shuffle(
	);

protected :

private :

	// helpers...

#if defined( PLATFORM_CYGWIN ) || defined( PLATFORM_WIN32 )

	// We have to implement our own version...

	static Uint64 bswap_64( const Uint64 v );
	static Uint32 bswap_32( const Uint32 v );
	static Uint16 bswap_16( const Uint16 v );

#endif

	// others...

	System(
		const System&
	);

	System& operator = (
		const System&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_System_H_

// ============================================================================


