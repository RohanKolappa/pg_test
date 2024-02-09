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
//	BFC.IO.ByteInputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Math.Utils.h"

#include "BFC.IO.ByteInputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, ByteInputStream )

// ============================================================================

IO::ByteInputStream::ByteInputStream(
		DevicePtr		pPeer ) :

	ProxyDevice( pPeer ) {

	setObjectName( "BFC.IO.ByteInputStream" );

}

// ============================================================================

void IO::ByteInputStream::open() {

//	msgDbg( "open(): --->" );

	ProxyDevice::open();

	// ignore potential write access on device
	setAccess( getAccessMode() & ~WriteAccess );

//	msgDbg( "open(): ----" );

	buffer = 0;
	cursor = 0;
	filled = 0;
	offset = 0;

//	msgDbg( "open(): <---" );

}

void IO::ByteInputStream::close() {

	cache.kill();

	buffer = 0;
	cursor = 0;
	filled = 0;
	offset = 0;

	ProxyDevice::close();

}

// ============================================================================

Buffer IO::ByteInputStream::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	ensureAvailable( 1 );

	Buffer res = cache( cursor );

	cursor = filled;

	return res;

}

// ============================================================================

Buffer IO::ByteInputStream::peekBytes(
	const	Uint32		toread,
	const	Bool		exact ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Buffer res;

	if ( ! toread ) {
		return res;
	}

	ensureAvailable( toread, exact );

	if ( ! cursor && toread == filled ) {
		res = cache;
	}
	else {
		res = cache( cursor, toread );
	}

	return res;

}

Buffer IO::ByteInputStream::getBytes(
	const	Uint32		toread,
	const	Bool		exact ) {

	Buffer res = peekBytes( toread, exact );

	cursor += res.length();

	return res;

}

// ============================================================================

Buffer IO::ByteInputStream::peekLine() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	ensureAvailable( 1 );

	Uint32	pos = 0;

	for (;;) {
		Uint32 len = filled - cursor;
		const Uchar * ptr = buffer + cursor;
		for ( Uint32 i = pos ; i < len ; i++ ) {
			if ( ptr[ i ] == '\n'
			  || ( ptr[ i ] == '\r'
			    && i + 1 < len
			    && ptr[ i + 1 ] == '\n' ) ) {
				return cache( cursor, i );
			}
		}
		pos = len - 1;
		ensureAvailable( len + 1 );
	}

}

Buffer IO::ByteInputStream::getLine() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	ensureAvailable( 1 );

	Uint32	pos = 0;

	for (;;) {
		Uint32 len = filled - cursor;
		const Uchar * ptr = buffer + cursor;
		for ( Uint32 i = pos ; i < len ; i++ ) {
			Uint32 toSkip = 0;
			if ( ptr[ i ] == '\n' ) {
				toSkip = 1;
			}
			else if ( ptr[ i ] == '\r'
			    && i + 1 < len
			    && ptr[ i + 1 ] == '\n' ) {
				toSkip = 2;
			}
			else {
				continue;
			}
			Buffer res = cache( cursor, i );
			cursor += i + toSkip;
			return res;
		}
		pos = len - 1;
		ensureAvailable( len + 1 );
	}

}

Buffer IO::ByteInputStream::getLine(
	const	Bool	concat,
	const	Bool	shellSyntax ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( atEOS() ) {
		throw EndOfStream();
	}

	Buffer res;

	for (;;) {

		Uint32	pos = res.getLength(); // start to search for comments here.

		// Add chars to 'res', up until some EOL char...

		for (;;) {
			Uchar c = getUchar();
			if ( c == '\n' ) {
				break;
			}
			if ( c == '\r' ) {
				if ( ! atEOS() && ( peekUchar() == '\n' ) ) {
					getUchar();
				}
				break;
			}
			res += c;
			if ( atEOS() ) {
				// Premature EOL!
				break;
			}
		}

		// If shellSyntax, find any comment, starting at 'pos'...

		if ( shellSyntax ) {
			Uint32 cmt = res.findFwd( '#', pos );
			if ( cmt < res.getLength() ) {
				// Found comment! Remove it and return!
				res = res( 0, cmt );
				break;
			}
		}

		// If not concatenating, return!

		if ( ! concat
		  || res.isEmpty()
		  || res.lastChar() != '\\' ) {
			break;
		}

		res.dropLastChar();

		if ( atEOS() ) {
			break;
		}

	}

	return res;

}

Buffer IO::ByteInputStream::getAnsiString() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Buffer res;

	for (;;) {
		// if nothing in cache, fill it!
		ensureAvailable(1);
		// find pos of newline...
		Uint32 p = cursor; // remember start of search...
		while (cursor < filled) {
			if (buffer[cursor] == '\0') {
				// got it! purge and return!
				res += cache( p, cursor - p );
				cursor++;
				return res;
			}
			cursor++;
		}
		// dump whole cache into res, and loop!
		res += cache( p );
	}

}

// ============================================================================

Uint32 IO::ByteInputStream::getBEUint(
		Uint32		length ) {

	if ( length < 1 || length > 4 ) {
		throw InvalidArgument();
	}

	ensureAvailable( length );

	Uint32	val = 0;

	while ( length-- ) {
		val <<= 8;
		val |= buffer[ cursor++ ];
	}

	return val;

}

// ============================================================================

Double IO::ByteInputStream::getDouble() {

	Uint32 l = ( Uint32 )getUchar();
	Buffer b = getBytes( l );

	if ( b == "inf" ) {
		return DoubleMax;
	}

	if ( b == "-inf" ) {
		return - DoubleMax;
	}

	return b.toDouble();

}

// ============================================================================

Uint64 IO::ByteInputStream::seek(
	const	Uint64		newOffset ) {

//	msgDbg( "seek(): ---> " + Buffer( newOffset ) );

	if ( newOffset >= offset && newOffset < offset + Uint64( filled ) ) {
		// cache hit!
		cursor = Uint32( newOffset - offset );
	}
	else {
		try {
			getPeer()->seek( newOffset );
		}
		catch ( BrokenPipe & ) {
			resetAccess();
			throw;
		}
		cache.kill();
		buffer = 0;
		cursor = 0;
		filled = 0;
		offset = newOffset;
	}

	return offset;

}

Uint64 IO::ByteInputStream::skip(
	const	Uint64		pToSkip ) {

	if ( hasRandomAccess() ) {
		return Device::skip( pToSkip );
	}
	else {
		Uint64 blkSze = getBlockSize();
		if ( ! blkSze ) {
			blkSze = 4096;
		}
		Uint64 toSkip = pToSkip;
		while ( toSkip ) {
			toSkip -= getBytes( ( Uint32 ) Math::getMin(
				toSkip, blkSze ) ).getLength();
		}
		return tell();
	}

}

Uint64 IO::ByteInputStream::tell() const {

	return offset + Uint64( cursor );

}

Uint64 IO::ByteInputStream::length() const {

	return getPeer()->length();

}

// ============================================================================

void IO::ByteInputStream::refill(
	const	Uint32		atLeast,
	const	Bool		pExact ) {

//	msgDbg( "ensureAvailable(): atLeast: "
//		+ Buffer( atLeast )
//		+ ", off: "
//		+ Buffer( offset )
//		+ ", cur: "
//		+ Buffer( cursor )
//		+ ", fill: "
//		+ Buffer( filled )
//		+ ", cache length: "
//		+ Buffer( cache.getLength() ) );

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! atLeast ) {
		throw InternalError();
	}

	if ( filled > cursor ) {
		cache = cache( cursor );
		buffer = cache.getCstPtr();
		filled = cache.getLength();
	}
	else {
		cache.kill();
		filled = 0;
		buffer = 0;
	}

	offset += cursor;
	cursor = 0;

	while ( cache.getLength() < atLeast ) {
		Uint32 needed = atLeast - cache.getLength();
		if ( needed > getBlockSize() ) {
			setBlockSize( needed );
		}
		Buffer buf;
		try {
			buf = getPeer()->getBytes();
		}
		catch ( EndOfStream & ) {
			if ( ! pExact ) {
				break;
			}
			throw;
		}
		catch ( BrokenPipe & ) {
			resetAccess();
			throw;
		}
		cache += buf;
		filled = cache.getLength();
		buffer = cache.getCstPtr();
	}

	if ( cache.isEmpty() ) {
		throw EndOfStream();
	}

}

// ============================================================================

