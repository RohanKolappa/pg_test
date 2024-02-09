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
//	BFC.Base.StackTrace.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#if defined( PLATFORM_LINUX )
#include <cxxabi.h>
#include <dlfcn.h>
#include <stdlib.h>
#ifndef PLATFORM_EMBLINUX
#include <execinfo.h>
#endif
#endif
#if defined( PLATFORM_WIN32 )
#include <windows.h>
#include <dbgHelp.h>
#include <winbase.h>
#endif

#include "BFC.Base.StackTrace.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Buffer StackTrace::dump(
	const	Uint32		pToSkip ) {

	Buffer	msg;

#if defined( PLATFORM_LINUX ) && !defined( PLATFORM_EMBLINUX)

	void *	array[ 100 ];
	Dl_info	info[ 100 ];

	Uint32	size = ::backtrace( array, 100 );

	for ( Uint32 i = pToSkip ; i < size ; i++ ) {
		::dladdr( array[ i ], &info[ i ] );
	}

	Buffer	sbuf( size - pToSkip );

	for ( Uint32 i = pToSkip ; i < size ; i++ ) {
		Buffer	elfname( info[ i ].dli_fname );
		Buffer	smbname( info[ i ].dli_sname );
		int	status;
		char *	tmp = abi::__cxa_demangle( info[ i ].dli_sname, 0, 0, &status );
		Buffer	demname( tmp, true );
		::free( tmp );
		if ( elfname.getLength() > 40 ) {
			elfname = "..." + elfname( elfname.getLength() - 37 );
		}
		else {
			elfname = Buffer::spaces( 40 - elfname.getLength() )
				+ elfname;
		}
		msg	+= "--- ["
			+ Buffer( i + 1 - pToSkip, Buffer::Base10, sbuf.getLength() )
			+ "/"
			+ sbuf
			+ "]: "
			+ elfname
			+ " ["
			+ Buffer( ( void * )info[ i ].dli_saddr )
			+ "] "
			+ ( demname.hasContent() ? demname : smbname )
			+ "\n";
	}

#elif defined( PLATFORM_WIN32 )

	LPVOID		stkTrc[ 64 ];
	USHORT		nbrFrm = CaptureStackBackTrace( 0, 63, stkTrc, NULL );

	HANDLE		handle = GetCurrentProcess();

	SymInitialize( handle, NULL, TRUE );

	SYMBOL_INFO *	symbol = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );

	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	Buffer	sbuf( ( Uint32 )( nbrFrm - pToSkip ) );

	for ( USHORT i = pToSkip ; i < nbrFrm ; i++ ) {
		// Symbol name...
		SymFromAddr( handle, ( DWORD64 )( stkTrc[ i ] ), 0, symbol );
		Buffer	smbNme( symbol->Name, true );
		// DLL name...
		DWORD64 dllAdd = SymGetModuleBase64( handle, ( DWORD64 )stkTrc[ i ] );
		Buffer	dllNme;
		if ( dllAdd ) {
			char	dllBuf[ 1024 ];
			GetModuleFileName( ( HMODULE )dllAdd, dllBuf, 1024 );
			dllNme = Buffer( dllBuf, true );
		}
		if ( dllNme.getLength() > 40 ) {
			dllNme = "..." + dllNme( dllNme.getLength() - 37 );
		}
		else {
			dllNme = Buffer::spaces( 40 - dllNme.getLength() )
				+ dllNme;
		}
		// Concatenate...
		msg	+= "--- ["
			+ Buffer( ( Uint32 )( i + 1 - pToSkip ), Buffer::Base10, sbuf.getLength() )
			+ "/"
			+ sbuf
			+ "]: "
			+ dllNme
			+ " ["
			+ Buffer( ( void * )symbol->Address )
			+ "] "
			+ smbNme
			+ "\n";
	}

	free( symbol );

#endif

	return msg;

}

// ============================================================================

