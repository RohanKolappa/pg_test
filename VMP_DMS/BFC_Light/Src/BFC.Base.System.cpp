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
//	BFC.Base.System.cpp
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
#include <string.h>
#else
#include <unistd.h>
#endif
#if defined(PLATFORM_DARWIN )
#include <strings.h>
#endif

#include "BFC.Base.System.h"

#include "BFC.Math.Utils.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Uchar * System::calloc(
	const	Uint32		pNbr,
	const	Uint32		pSze ) {

	return ( Uchar * )::calloc( pNbr, pSze );

}

Uchar * System::malloc(
	const	Uint32		pSze ) {

	return ( Uchar * )::malloc( pSze );

}

Uchar *System::realloc(
		Uchar *		pPtr,
	const	Uint32		pSze ) {

	return ( Uchar * )::realloc( pPtr, pSze );

}

void System::free(
		Uchar *		pPtr ) {

	::free( pPtr );

}

// ============================================================================

void System::memcpy(
		Uchar *		pDst,
	const	Uchar *		pSrc,
	const	Uint32		pLen ) {

	::memcpy(
		( void * )( pDst ),
		( const void * )( pSrc ),
		( size_t )pLen );

}

void System::memmove(
		Uchar *		pDst,
	const	Uchar *		pSrc,
	const	Uint32		pLen ) {

	::memmove(
		( void * )( pDst ),
		( const void * )( pSrc ),
		( size_t )pLen );

}

void System::memset(
		Uchar *		pDst,
	const	Uchar		pChr,
	const	Uint32		pLen ) {

	::memset(
		( void * )( pDst ),
		( int )( Uint32 )pChr,
		( size_t )pLen );

}

void System::bzero(
		Uchar *		pDst,
	const	Uint32		pLen ) {

	::memset(
		( void * )( pDst ),
		0,
		( size_t )pLen );

}

Uint32 System::strlen(
	const	Uchar *		str ) {

	return ::strlen( ( const char * )str );

}

Int32 System::strcmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2 ) {

	return ::strcmp( ( const char * )pS1, ( const char * )pS2 );

}

Int32 System::strncmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen ) {

	return ::strncmp( ( const char * )pS1, ( const char * )pS2, pLen );

}

Int32 System::strcasecmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2 ) {

#if defined( PLATFORM_WIN32 )
	return ::_stricmp( ( const char * )pS1, ( const char * )pS2 );
#else
	return ::strcasecmp( ( const char * )pS1, ( const char * )pS2 );
#endif

}

Int32 System::strncasecmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen ) {

#if defined( PLATFORM_WIN32 )
	return ::_strnicmp( ( const char * )pS1, ( const char * )pS2, pLen );
#else
	return ::strncasecmp( ( const char * )pS1, ( const char * )pS2, pLen );
#endif

}

Int32 System::memcmp(
	const	Uchar *		pS1,
	const	Uchar *		pS2,
	const	Uint32		pLen ) {

	return ::memcmp( ( const void * )pS1, ( const void * )pS2, pLen );

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
	::usleep( ( useconds_t )usecs );
#endif

}

void System::sleepRand(
	const	Uint32		pSecs ) {

	sleep( Math::rand( pSecs ) );

}

void System::usleepRand(
	const	Uint64		pUsecs ) {

	usleep( Math::rand( pUsecs ) );

}

// ============================================================================

