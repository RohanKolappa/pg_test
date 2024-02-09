// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::IO".
// 
// "BFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.IO.ByteOutputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, ByteOutputStream )

// ============================================================================

IO::ByteOutputStream::ByteOutputStream(
		DevicePtr	pPeer ) :

	ProxyDevice( pPeer ) {

	setObjectName( "BFC.IO.ByteOutputStream" );

}

// ============================================================================

void IO::ByteOutputStream::open() {

	ProxyDevice::open();

	cursor = 0;

}

// ============================================================================

void IO::ByteOutputStream::putBytes(
	const	Buffer		& b ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	const	Uint32	len = b.getLength();

	if ( cursor + ( Uint64 )len > ( Uint64 )BlockSize ) {
		flush();
		if ( len > BlockSize ) {
			try {
				getPeer()->putBytes( b );
			}
			catch ( BrokenPipe & ) {
				resetAccess();
				throw;
			}
			return;
		}
	}

	System::memcpy( buffer + cursor, b.getBufferAddr(), len );
	cursor += len;

}

void IO::ByteOutputStream::putAnsiString(
	const	Buffer&		buf) {

	putBytes(buf);
	putUchar('\0');

}

void IO::ByteOutputStream::flush() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( cursor ) {
		try {
			getPeer()->putBytes(
				Buffer( buffer, (Uint32)cursor, true ) );
		}
		catch ( BrokenPipe & ) {
			resetAccess();
			throw;
		}
		cursor = 0;
		getPeer()->flush();
	}

}

// ============================================================================

void IO::ByteOutputStream::putDouble(
	const	Double		d ) {

	Buffer b;

	if ( d == DoubleMax ) {
		b = "inf";
	}
	else if ( d == - DoubleMax ) {
		b = "-inf";
	}
	else {
		b = Buffer( d );
	}

	putUchar( ( Uchar )b.getLength() );
	putBytes( b );

}

// ============================================================================

Uint64 IO::ByteOutputStream::seek(
	const	Uint64		offset) {

	flush();

	return getPeer()->seek(offset);

}

Uint64 IO::ByteOutputStream::tell() const {

	return getPeer()->tell() + cursor;

}

Uint64 IO::ByteOutputStream::length() const {

	return getPeer()->length();

}

// ============================================================================

void IO::ByteOutputStream::ensureAvailable(
	const	Uint32		l) {

	if ( cursor + Uint64( l ) > Uint64( BlockSize ) ) {
		flush();
	}

}

// ============================================================================

