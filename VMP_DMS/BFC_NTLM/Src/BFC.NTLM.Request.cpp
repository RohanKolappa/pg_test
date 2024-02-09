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
//	BFC.NTLM.Request.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteOutputStream.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "BFC.NTLM.Request.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( NTLM, Request )

// ============================================================================

NTLM::Request::Request(
	const	Buffer &	pHost,
	const	Buffer &	pDomain ) {

	Uint32	domLen = pDomain.getLength();
	Uint32	hstLen = pHost.getLength();

	Uint32	totLen = 32 + hstLen + domLen;

	IO::MemoryOutputStreamPtr
				mStream = new IO::MemoryOutputStream( totLen );
	IO::ByteOutputStreamPtr	bStream = new IO::ByteOutputStream( mStream );

	bStream->open();

	Uint32	hstOff = 0x20;
	Uint32	domOff = hstOff + hstLen;

	static const Uchar hdr[ 16 ] = {
		'N', 'T', 'L', 'M',
		'S', 'S', 'P', 0x00,
		0x01, 0x00, 0x00, 0x00,
		0x03, 0xB2, 0x00, 0x00
	};

	bStream->putBytes( Buffer( hdr, 16 ) );
	bStream->putLEUint32( domLen | ( domLen << 16 ) );
	bStream->putLEUint32( domOff );
	bStream->putLEUint32( hstLen | ( hstLen << 16 ) );
	bStream->putLEUint32( hstOff );
	bStream->putBytes( pHost.toUpper() );
	bStream->putBytes( pDomain.toUpper() );
	bStream->flush();

	data = mStream->getContent();

}

// ============================================================================

