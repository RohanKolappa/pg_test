// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "Sys::MT".
// 
// "Sys::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "Sys::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "Sys::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	Sys.MT.ThreadCreator.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <errno.h>
#include <iostream>
#include <string.h>
#if defined( PLATFORM_WIN32 )
#include <process.h>
#endif

#include "Sys.MT.ThreadCreator.h"
#include "Sys.MT.ThreadData.h"

// ============================================================================

using namespace std;
using namespace Sys;

// ============================================================================

MT::ThreadCreator::ThreadCreator() {

//	cerr << "Sys::MT::ThreadCreator()" << endl;

	threadCreator = this;

}

MT::ThreadCreator::~ThreadCreator() {

	if ( threadCreator == this ) {
		threadCreator = 0;
	}

}

// ============================================================================

MT::ThreadCreator * MT::ThreadCreator::instance() {

	return threadCreator;

}

// ============================================================================

#if defined( PLATFORM_WIN32 )

bool MT::ThreadCreator::create(
		ThreadData *	localData,
		void		( * launcher )( void * ),
		void *		arg ) {

	return  ( ( localData->id = ( HANDLE )::_beginthread( launcher, 0, arg ) )
			!= ( HANDLE )-1 );

}

#else

bool MT::ThreadCreator::create(
		ThreadData *	localData,
		void *		( * launcher )( void * ),
		void *		arg ) {

	return ( ::pthread_create(
			& localData->id,
			& localData->attr,
			launcher,
			arg ) == 0 );

}

#endif

// ============================================================================

MT::ThreadCreator * MT::ThreadCreator::threadCreator = 0;

// ============================================================================

static MT::ThreadCreator threadCreator;

// ============================================================================

