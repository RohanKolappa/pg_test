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
//	BFC.IO.MemoryInputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.MemoryInputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, MemoryInputStream )

// ============================================================================

IO::MemoryInputStream::MemoryInputStream(
	const	Buffer&		content ) {

	setObjectName( "BFC.IO.MemoryInputStream" );

	buffer = content;

}

// ============================================================================

void IO::MemoryInputStream::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	address = buffer.getBufferAddr();
	size    = buffer.length();
	cursor  = 0;

	setAccess( ReadAccess | RandomAccess );

}

void IO::MemoryInputStream::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	address = 0;
	size    = 0;
	cursor  = 0;

	resetAccess();

}

// ============================================================================

void IO::MemoryInputStream::addContent(
	const	Buffer&		contentToAdd ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	buffer += contentToAdd;
	address = buffer.getBufferAddr();
	size    = buffer.length();

}

// ============================================================================

Buffer IO::MemoryInputStream::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( cursor == size ) {
		throw EndOfStream();
	}

	Buffer res;

	if ( ! cursor ) {
		res = buffer;
	}
	else {
		res = buffer( cursor );
	}

	cursor = size;

	return res;

}

// ============================================================================

Uint64 IO::MemoryInputStream::seek(
	const	Uint64		offset ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( offset > (Uint64)size ) {
		throw CantSeek( "Overflow!" );
	}

	cursor = (Uint32)offset;

	return offset;

}

Uint64 IO::MemoryInputStream::tell() const {

	return (Uint64)cursor;

}

Uint64 IO::MemoryInputStream::length() const {

	return (Uint64)size;

}

// ============================================================================





