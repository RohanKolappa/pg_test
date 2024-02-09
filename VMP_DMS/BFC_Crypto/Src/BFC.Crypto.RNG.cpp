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
//	BFC.Crypto.RNG.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.Crypto.RNG.h"
#include "BFC.Crypto.RNGAnsi.h"
#include "BFC.Crypto.RNGUnix.h"
#include "BFC.Crypto.RNGWin32.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Crypto, RNG )

// ============================================================================

Crypto::RNGPtr Crypto::RNG::createNew() {

	Crypto::RNGPtr res;

#if defined( PLATFORM_WIN32 )
	res = new RNGWin32;
	if ( res->isValid() ) {
		return res;
	}
#endif

#if defined( PLATFORM_LINUX )
	res = new RNGUnix;
	if ( res->isValid() ) {
		return res;
	}
#endif

	res = new RNGAnsi;
	if ( res->isValid() ) {
		return res;
	}

	throw InternalError( "No RNG available!" );

}

// ============================================================================

Buffer Crypto::RNG::getBytes(
	const	Uint32		pBytes ) {

	Buffer res;

	res.resize( pBytes );

	getBytes( res );

	return res;

}

// ============================================================================

