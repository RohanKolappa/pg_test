// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
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
//	Sys.MT.ThreadData.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "Sys.MT.ThreadData.h"

// ============================================================================

using namespace Sys;

// ============================================================================

MT::ThreadData::ThreadData() {

#if ! defined( PLATFORM_WIN32 )

	::pthread_attr_init( & attr );
	::pthread_attr_setdetachstate( & attr, PTHREAD_CREATE_JOINABLE );
	::pthread_attr_setschedpolicy( & attr, SCHED_OTHER );

#endif

}

MT::ThreadData::~ThreadData() {

#if ! defined( PLATFORM_WIN32 )

	::pthread_attr_destroy( & attr );

#endif

}

// ============================================================================

