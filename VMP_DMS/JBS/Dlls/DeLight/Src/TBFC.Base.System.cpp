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
//	TBFC.Base.System.cpp
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

#if defined( PLATFORM_WIN32 )
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "TBFC.Base.System.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

Uchar * System::malloc(
	const	Uint32		size ) {

	return ( Uchar * )::malloc( size );

}

Uchar *System::realloc(
		Uchar		* ptr,
	const	Uint32		size ) {

	return ( Uchar * )::realloc( ptr, size );

}

void System::free(
		Uchar		* ptr ) {

	::free( ptr );

}

// ============================================================================

void System::memcpy(
		Uchar		* dest,
	const	Uchar		* src,
	const	Uint32		len ) {

	::memcpy(
		( void * )( dest ),
		( const void * )( src ),
		( size_t )len );

}

void System::memmove(
		Uchar		* dest,
	const	Uchar		* src,
	const	Uint32		len ) {

	::memmove(
		( void * )( dest ),
		( const void * )( src ),
		( size_t )len );

}

void System::memset(
		Uchar		* dest,
	const	Uchar		c,
	const	Uint32		len ) {

	::memset(
		( void * )( dest ),
		( int )( Uint32 )c,
		( size_t )len );

}

void System::bzero(
		Uchar		* dest,
	const	Uint32		len ) {

	::memset(
		( void * )( dest ),
		0,
		( size_t )len );

}

Uint32 System::strlen(
	const	Uchar		* str ) {

	return ::strlen( ( const char * )str );

}

Int32 System::strcmp(
	const	Uchar		* __s1,
	const	Uchar		* __s2 ) {

	return ::strcmp( ( const char * )__s1, ( const char * )__s2 );

}

Int32 System::strncmp(
	const	Uchar		* __s1,
	const	Uchar		* __s2,
	const	Uint32		__len ) {

	return ::strncmp( ( const char * )__s1, ( const char * )__s2, __len );

}

// ============================================================================

void System::sleep(
	const	Uint32		secs ) {

#if defined( PLATFORM_WIN32 )
//	_sleep(secs * 1000);
	Sleep( secs * 1000 );
#else
	::sleep( secs );
#endif

}

void System::usleep(
	const	Uint64		usecs ) {

#if defined( PLATFORM_WIN32 )
//	_sleep((Uint32)(usecs) / 1000); // in millisecs for WIN32
	// ^^^^ deprecated in MSVC8 :-(
	Sleep( ( Uint32 )( usecs ) / 1000 ); // in millisecs for WIN32
#else
	::usleep( usecs );
#endif

}

// ============================================================================

