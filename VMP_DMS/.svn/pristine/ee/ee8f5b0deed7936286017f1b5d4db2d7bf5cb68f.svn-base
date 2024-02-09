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
//	BFC.Light.LibAsserts.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Light.LibAsserts.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Light, LibAsserts )

// ============================================================================

template < typename T >
static void testSizeof(
	const	Uint32		s,
	const	char *		n ) {

	if ( sizeof( T ) != s ) {
		throw InternalError( "Sizeof( " + Buffer( n ) + " ) == "
			+ Buffer( ( Uint32 )sizeof( T ) ) );
	}

}

// ============================================================================

Light::LibAsserts::LibAsserts() {

	// Global assertions...

	testSizeof< Uint8 >( 1, "Uint8" );
	testSizeof< Uint16 >( 2, "Uint16" );
	testSizeof< Uint32 >( 4, "Uint32" );
	testSizeof< Uint64 >( 8, "Uint64" );
	testSizeof< Float >( 4, "Float" );
	testSizeof< Double >( 8, "Double" );

}

// ============================================================================

Light::LibAssertsPtr Light::LibAsserts::instance() {

	static LibAssertsPtr i = new LibAsserts;

	return i;

}

// ============================================================================

static Light::LibAssertsPtr dummy = Light::LibAsserts::instance();

// ============================================================================

