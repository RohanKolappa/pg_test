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
//	BFC.NTLM.Challenge.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.System.h"

#include "BFC.NTLM.Challenge.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( NTLM, Challenge )

// ============================================================================

NTLM::Challenge::Challenge(
	const	Buffer &	pPacket ) {

	static const Uchar hdr1[ 16 ] = {
		'N', 'T', 'L', 'M',
		'S', 'S', 'P', 0x00,
		0x02, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	static const Uchar hdr2[ 8 ] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	if ( pPacket.getLength() < 40 ) {
		throw InvalidArgument( "Invalid type 2 message: length!" );
	}

	if ( pPacket( 0, 16 ) != Buffer( hdr1, 16 ) ) {
		throw InvalidArgument( "Invalid type 2 message: header!" );
	}

	if ( pPacket( 32, 8 ) != Buffer( hdr2, 8 ) ) {
		throw InvalidArgument( "Invalid type 2 message: trailer!" );
	}

	Uint32 len = System::Host2LE( *( const Uint32 * )( pPacket.getCstPtr() + 16 ) );

	if ( pPacket.getLength() != len ) {
		throw InvalidArgument( "Invalid type 2 message: != length!" );
	}

	nonce = pPacket( 24, 8 );

}

// ============================================================================

