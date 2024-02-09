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
//	BFC.System.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "BFC.Math.RNG.h"
#include "BFC.Base.System.h"
#include "BFC.Base.Time.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_CYGWIN ) || defined( PLATFORM_WIN32 )

// Based on Linux version (see /usr/include/bits/byteswap.h).

Uint64 System::bswap_64( const Uint64 v ) {

	return ( ( v & 0xFF00000000000000ull ) >> 56 )
	     | ( ( v & 0x00FF000000000000ull ) >> 40 )
	     | ( ( v & 0x0000FF0000000000ull ) >> 24 )
	     | ( ( v & 0x000000FF00000000ull ) >>  8 )
	     | ( ( v & 0x00000000FF000000ull ) <<  8 )
	     | ( ( v & 0x0000000000FF0000ull ) << 24 )
	     | ( ( v & 0x000000000000FF00ull ) << 40 )
	     | ( ( v & 0x00000000000000FFull ) << 56 );

}

Uint32 System::bswap_32( const Uint32 v ) {

	return ( ( v & 0xFF000000 ) >> 24 )
	     | ( ( v & 0x00FF0000 ) >>  8 )
	     | ( ( v & 0x0000FF00 ) <<  8 )
	     | ( ( v & 0x000000FF ) << 24 );

}

Uint16 System::bswap_16( const Uint16 v ) {

	return ( ( ( v        ) >> 8 ) & 0xff )
	     | ( ( ( v & 0xff ) << 8 )        );

}

#endif // !defined(PLATFORM_CYGWIN)

// ============================================================================

System::System() {



}

System::~System() {



}

// ============================================================================

Uchar *System::malloc(
	const	Uint32		size) {

	return (Uchar *)::malloc(size);

}

Uchar *System::realloc(
		Uchar		*ptr,
	const	Uint32		size) {

	return (Uchar *)::realloc(ptr, size);

}

void System::free(
		Uchar		*ptr) {

	::free(ptr);

}

// ============================================================================

void System::memcpy(
		Uchar		*dest,
	const	Uchar		*src,
	const	Uint32		len) {

	::memcpy((void *)dest, (const void *)src, (size_t)len);

}

void System::memset(
		Uchar		*dest,
	const	Uchar		c,
	const	Uint32		len) {

	::memset((void *)dest, (int)(Uint32)c, (size_t)len);

}

void System::bzero(
		Uchar		*dest,
	const	Uint32		len) {

	::memset((void *)dest, 0, (size_t)len);

}

Uint32 System::strlen(
	const	Uchar		*str ) {

	return ::strlen( ( const char * )str );

}

// ============================================================================

void System::sleep(
	const	Uint32	secs) {

#if defined(PLATFORM_WIN32)
//	_sleep(secs * 1000);
	Sleep(secs * 1000);
#else
	::sleep(secs);
#endif

}

void System::usleep(
	const	Uint64	usecs) {

#if defined(PLATFORM_WIN32)
//	_sleep((Uint32)(usecs) / 1000); // in millisecs for WIN32
	// ^^^^ deprecated in MSVC8 :-(
	Sleep((Uint32)(usecs) / 1000); // in millisecs for WIN32
#else
	::usleep(usecs);
#endif

}

// ============================================================================

#if 0

Uint32 System::getRandMax() {

	return (Uint32)RAND_MAX;

}

Uint32 System::rand() {

	return (Uint32)::rand();

}

Uint32 System::rand(
	const	Uint32	range) {

	return (Uint32)( (Uint64)(::rand())
	                   * (Uint64)(range)
	                   / (Uint64)(RAND_MAX));

}

Bool System::randBool() {

	return (::rand() <= (RAND_MAX >> 1));

}

void System::srand(const Uint32 seed) {

	::srand((unsigned int)seed);

}

void System::shuffle() {

	::srand((unsigned int)Time::now().getSecondsSinceEpoch());

}

#else

static RNG rng;

Uint32 System::getRandMax() {

	return 0xFFFFFFFF;

}

Uint32 System::rand() {

	return rng.rand( 0xFFFFFFFF );

}

Uint32 System::rand(
	const	Uint32		range ) {

	return rng.rand( range );

}

Bool System::randBool() {

	return ( rand() <= 0x7FFFFFFF );

}

void System::srand( const Uint32 seed ) {

	rng.randomInit( seed );

}

void System::shuffle() {

	rng.randomInit( Time::now().getSecondsSinceEpoch() );

}

#endif

// ============================================================================

