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
//	BFC.IO.NullDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.NullDevice.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, NullDevice )

// ============================================================================

IO::NullDevice::NullDevice() {

	setObjectName( "BFC.IO.NullDevice" );

}

// ============================================================================

void IO::NullDevice::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	setAccess( IO::Device::WriteAccess | IO::Device::RandomAccess );

}

void IO::NullDevice::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	resetAccess();

}

Buffer IO::NullDevice::getBytes() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	throw NoReadAccess();

}

void IO::NullDevice::putBytes(
	const	Buffer &	/* input */ ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

}

// ============================================================================

Uint64 IO::NullDevice::seek(
	const	Uint64		/* offset */ ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return ( Uint64 )0;

}

Uint64 IO::NullDevice::tell() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return ( Uint64 )0;

}

Uint64 IO::NullDevice::length() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return ( Uint64 )0;

}

// ============================================================================

