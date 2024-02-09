// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::IO".
// 
// "TBFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.IO.ByteInputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.IO.ByteInputStream.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL_NS( IO, ByteInputStream )

// ============================================================================

IO::ByteInputStream::ByteInputStream(
		DevicePtr		device )
	: ProxyDevice( device ) {

	setObjectName( "TBFC.IO.ByteInputStream" );

}

IO::ByteInputStream::~ByteInputStream() {

	if ( isConnected() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "~ByteInputStream(): can't auto-close device!" );
		}
	}

}

// ============================================================================

void IO::ByteInputStream::open() {

	ProxyDevice::open();

	buffer = 0;
	cursor = 0;
	filled = 0;
	offset = 0;

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
	const	Uint32		toread ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Buffer res;

	if ( ! toread ) {
		return res;
	}

	ensureAvailable( toread );

	if ( ! cursor && toread == filled ) {
		res = cache;
	}
	else {
		res = cache( cursor, toread );
	}

	return res;

}

Buffer IO::ByteInputStream::getBytes(
	const	Uint32		toread ) {

	Buffer res = peekBytes( toread );

	cursor += res.length();

	return res;

}

// ============================================================================

Buffer IO::ByteInputStream::getLine(
	const	Bool	concat ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	Buffer res;

	if ( concat ) {

	for (;;) {
		ensureAvailable( 1 );
		Uint32 startLine = cursor;
		while ( cursor < filled && buffer[ cursor++ ] != '\n' ) {
		}
		if ( cursor >= filled ) {
			res += cache( startLine, filled - startLine );
			continue;
		}
		// Find '#', if any...
		Uint32 stopLine = startLine;
		while ( stopLine < cursor && buffer[ stopLine ] != '#') {
			stopLine++;
		}
		if ( stopLine < cursor ) {
			// We found a comment! Skip trailing white spaces...
			while ( stopLine > startLine
			     && ( buffer[ stopLine - 1 ] == ' '
			       || buffer[ stopLine - 1 ] == '\t' ) ) {
				stopLine--;
			}
			// Skip leading white spaces too...
			while ( startLine < stopLine
			     && ( buffer[ startLine ] == ' '
			       || buffer[ startLine ] == '\t' ) ) {
				startLine++;
			}
			if ( stopLine == startLine ) {
				// We have to entirely skip this line!
				continue;
			}
			// Here, we are sure that there is some payload!
			res += cache( startLine, stopLine - startLine );
			return res;
		}
		// Here, we are sure that there is no comment on this line!
		// Go backware, skipping the '\n' and '\r' if present...
		stopLine--; // now on the '\n'...
		if ( stopLine > startLine && buffer[ stopLine - 1 ] == '\r' ) {
			stopLine--;
		}
		if ( stopLine > startLine && buffer[ stopLine - 1 ] == '\\' ) {
			// backslash-terminated line!
			stopLine--; // now on the '\\'...
			// Skip trailing white spaces...
			while ( stopLine > startLine
			     && ( buffer[ stopLine - 1 ] == ' '
			       || buffer[ stopLine - 1 ] == '\t' ) ) {
				stopLine--;
			}
			// Skip leading white spaces too...
			while ( startLine < stopLine
			     && ( buffer[ startLine ] == ' '
			       || buffer[ startLine ] == '\t' ) ) {
				startLine++;
			}
			if ( stopLine == startLine ) {
				// We have to entirely skip this line!
				continue;
			}
			res += cache( startLine, stopLine - startLine );
			res += ' '; // add a white space to replace the '\\'...
			continue;
		}
		if ( stopLine > startLine ) {
			res += cache( startLine, stopLine - startLine );
		}
		if ( ! res.isEmpty() ) {
			return res;
		}
	}

	}
	else {

	for (;;) {
		ensureAvailable( 1 );
		Uint32 p = cursor;
		while ( cursor < filled ) {
			if ( buffer[ cursor++ ] == '\n' ) {
				Uint32 l = cursor - p - 1;
				if ( l ) {
					res += cache( p, l );
				}
				if ( res.length()
				  && res.lastChar() == '\r' ) {
					res.dropLastChar();
				}
				return res;
			}
		}
		res += cache( p );
	}

	}

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

	if (!length || length > 4) {
		throw InvalidArgument();
	}

	ensureAvailable(length);
	Uint32	v;
	Uchar	*dst = (Uchar *)&v;
	const Uchar *src = buffer + cursor + length - 1;
	cursor += length;
	while (length--)
		*dst++ = *src--;

	return v;

}

// ============================================================================

Double IO::ByteInputStream::getDouble() {

	Uint32 l = (Uint32)getUchar();
	Buffer b = getBytes( l );

	return b.toDouble();

}

// ============================================================================

Uint64 IO::ByteInputStream::seek(
	const	Uint64		newOffset) {

	if (!device) {
		throw NotConnected();
	}

	if ( newOffset >= offset && newOffset < offset + Uint64( filled ) ) {
		// cache hit!
		cursor = Uint32( newOffset - offset );
	}
	else {
		cache.kill();
		buffer = 0;
		cursor = 0;
		filled = 0;
		offset = newOffset;
		device->seek(offset);
	}

	return offset;

}

Uint64 IO::ByteInputStream::tell() const {

	if (!device) {
		throw NotConnected();
	}

	return offset + Uint64( cursor );

}

Uint64 IO::ByteInputStream::length() const {

	if (!device) {
		throw NotConnected();
	}

	return device->length();

}

// ============================================================================

void IO::ByteInputStream::ensureAvailable(
	const	Uint32		atLeast) {

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

	if ( cursor + atLeast <= filled ) {
		return;
	}

	if ( filled > cursor ) {
		cache = cache( cursor );
	}
	else {
		cache.kill();
	}

	offset += cursor;

	if ( ! isConnected() ) {
		throw NotConnected();
	}

//	cache.supposedMaxSize( atLeast + 4096 );

	while ( cache.length() < atLeast ) {
//		Uint32 oldLength = cache.getLength();
		try {
			cache += device->getBytes();
		}
		catch ( BrokenPipe& ) {
			resetAccess();
			throw;
		}
//		msgDbg( "ensureAvailable(): cache growing: "
//			+ Buffer( oldLength )
//			+ " ---> "
//			+ Buffer( cache.length() ) );
	}

	cursor = 0;
	filled = cache.length();
	buffer = cache.getBufferAddr();

}

// ============================================================================

