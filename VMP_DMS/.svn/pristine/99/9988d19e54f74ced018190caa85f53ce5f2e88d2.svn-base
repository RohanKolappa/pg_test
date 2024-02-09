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
//	BFC.Crypto.RNGUnix.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#if defined( PLATFORM_LINUX )

// ============================================================================

#include "BFC.IO.File.h"

#include "BFC.Crypto.RNGUnix.h"

// ============================================================================

using namespace BFC;

// ============================================================================

Crypto::RNGUnix::RNGUnix() {

	IO::FilePtr tmp;

	try {
		if ( ! tmp ) {
			tmp = new IO::File( "/dev/urandom", IO::File::Read );
			tmp->open();
		}
	}
	catch ( Exception & ) {
		tmp.kill();
	}

	try {
		if ( ! tmp ) {
			tmp = new IO::File( "/dev/random", IO::File::Read );
			tmp->open();
		}
	}
	catch ( Exception & ) {
		tmp.kill();
	}

	if ( tmp ) {
		f = new IO::ByteInputStream( tmp );
		f->open();
		setValid();
	}

}

// ============================================================================

void Crypto::RNGUnix::getBytes(
		Buffer &	pBuffer ) {

	pBuffer = f->getBytes( pBuffer.getLength() );

}

// ============================================================================

#endif // PLATFORM_LINUX

// ============================================================================

