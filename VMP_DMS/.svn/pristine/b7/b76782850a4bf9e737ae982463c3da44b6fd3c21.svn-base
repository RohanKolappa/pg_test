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
//	TBFC.IO.MemoryOutputStream.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.IO.MemoryOutputStream.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL_NS( IO, MemoryOutputStream )

// ============================================================================

IO::MemoryOutputStream::MemoryOutputStream() {

	setObjectName( "TBFC.IO.MemoryOutputStream" );

}

IO::MemoryOutputStream::~MemoryOutputStream() {

	if ( isConnected() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "~MemoryOutputStream(): can't auto-close!" );
		}
	}

}

// ============================================================================

void IO::MemoryOutputStream::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	buffer.kill();
	offset = 0;

	setAccess( WriteAccess | RandomAccess );

}

void IO::MemoryOutputStream::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	buffer.kill();

	resetAccess();

}

// ============================================================================

Buffer IO::MemoryOutputStream::getContent() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return buffer;

}

// ============================================================================

void IO::MemoryOutputStream::putBytes(
	const	Buffer&		input ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( offset > buffer.length() ) {
		buffer += Buffer::chars( offset - buffer.length(), '\0' );
	}

	if ( offset == buffer.length() ) {
		buffer += input;
	}
	else if ( offset + input.length() >= buffer.length() ) {
		buffer = buffer( 0, offset )
			+ input;
	}
	else {
		buffer = buffer( 0, offset )
			+ input
			+ buffer( offset + input.length() );
	}

	offset += input.length();

}

// ============================================================================

Uint64 IO::MemoryOutputStream::seek(
	const	Uint64		newOffset ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	offset = (Uint32)newOffset;

	return (Uint64)offset;

}

Uint64 IO::MemoryOutputStream::tell() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return (Uint64)offset;

}

Uint64 IO::MemoryOutputStream::length() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return (Uint64)buffer.length();

}

// ============================================================================





