// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "Sys::DL".
//
// "Sys::DL" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "Sys::DL" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "Sys::DL"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	Sys.DL.DllLoader.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "Sys.DL.DllLoader.h"
#include "Sys.DL.DllLoader.Data.h"

// ============================================================================

using namespace Sys;

// ============================================================================

DL::DllLoader::DllLoader() {

	dllLoader = this;

}

DL::DllLoader::~DllLoader() {

	if ( dllLoader == this ) {
		dllLoader = 0;
	}

}

// ============================================================================

DL::DllLoader * DL::DllLoader::instance() {

	return dllLoader;

}

// ============================================================================

DL::DllLoader::Data * DL::DllLoader::open(
	const	char *		filename,
	const	bool		pLoadLazy,
	const	bool		pLoadLocal ) {

#if defined( PLATFORM_WIN32 )

	HMODULE res = LoadLibraryA(
			filename );

#else

	::dlerror();

	void * res = ::dlopen(
			filename,
			( pLoadLazy ? RTLD_LAZY : RTLD_NOW )
			| ( pLoadLocal ? RTLD_LOCAL : RTLD_GLOBAL ) );

#endif

	if ( ! res ) {
		return 0;
	}

	Data * data = new Data;

	data->handle = res;

	return data;

}

bool DL::DllLoader::close(
		Data *&		pData ) {

	if ( ! pData ) {
		return false;
	}

	int errorCode = 0;

#ifdef PLATFORM_WIN32

	//errorCode = ???
	FreeLibrary( pData->handle );

#else

	::dlerror();
	errorCode = ::dlclose( pData->handle );

#endif

	delete pData;
	pData = 0;

	return errorCode == 0;

}

char * DL::DllLoader::error() {

#if defined( PLATFORM_WIN32 )

//	return "unknown error";

	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	static char	buf[ 1024 ];

	const char * src = ( const char * )( LPCTSTR )lpMsgBuf;
	const int len = strlen( src );
	memcpy( buf, src, len >= 1023 ? 1023 : len );
	buf[ 1023 ] = 0;

	LocalFree(lpMsgBuf);
	return buf;

#else
	return ::dlerror();
#endif

}

void * DL::DllLoader::sym(
	const	Data *		pData,
	const	char *		pName ) {

	if ( ! pData ) {
		return 0;
	}

#if defined( PLATFORM_WIN32 )
	return GetProcAddress( pData->handle, pName );
#else
	return ::dlsym( pData->handle, pName );
#endif

}

// ============================================================================

DL::DllLoader * DL::DllLoader::dllLoader = 0;

// ============================================================================

static DL::DllLoader dllLoader;

// ============================================================================

