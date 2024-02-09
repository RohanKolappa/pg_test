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
//	BFC.Base.BusyCS.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include <stdio.h>

#include "BFC.Base.BusyCS.h"

// ============================================================================

using namespace BFC;

// ============================================================================

#if defined( PLATFORM_LINUX ) && !defined( PLATFORM_EMBLINUX)

void BusyCS::enter() {

	for (;;) {

		Uint32	ret;
	
		__asm __volatile(
			"lock; cmpxchgl %2, %1"
				: "=a" ( ret ), "=m" ( cnt )
				: "r" ( 0 ), "m" ( cnt ), "0" ( 1 ) );

		if ( ret == 1 ) {
			break;
		}

		asm ("rep; nop");

	}

}

void BusyCS::leave() {

	Uint32	ret;

	__asm __volatile(
		"lock; cmpxchgl %2, %1"
			: "=a" ( ret ), "=m" ( cnt )
			: "r" ( 1 ), "m" ( cnt ), "0" ( 0 ) );

	if ( ret ) { // Keeps ICC happy :-)
	}

}

#endif

// ============================================================================

