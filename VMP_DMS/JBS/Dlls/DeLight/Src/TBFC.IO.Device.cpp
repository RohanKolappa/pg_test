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
//	TBFC.IO.Device.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "TBFC.IO.Device.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL_NS( IO, Device )

// ============================================================================

IO::Device::Device() {

	setObjectName( "TBFC.IO.Device" );

	accessMode	= NoAccess;
	connected	= false;

}

IO::Device::~Device() {

	if ( isConnected() ) {
		msgWrn( "~Device(): still connected :-(" );
	}

}

// ============================================================================

void IO::Device::setAccess(
	const	Uint32		m ) {

	connected = true;
	accessMode = m;

}

void IO::Device::resetAccess() {

	connected = false;
	accessMode = NoAccess;

}

Uint32 IO::Device::getAccessMode() const {

	return accessMode;

}

// ============================================================================

Bool IO::Device::hasReadAccess() const {

	return ( ( accessMode & ReadAccess ) != 0 );

}

Bool IO::Device::hasWriteAccess() const {

	return ( ( accessMode & WriteAccess ) != 0 );

}

Bool IO::Device::hasRandomAccess() const {

	return ( ( accessMode & RandomAccess ) != 0 );

}

// ============================================================================

Bool IO::Device::operator == (
	const	Device&		o ) const {

	return ( this == &o );

}

Bool IO::Device::operator != (
	const	Device&		o ) const {

	return ( this != &o );

}

// ============================================================================

Buffer IO::Device::getBytes() {

	throw NotImplemented();

}

void IO::Device::putBytes(
	const	Buffer&		/* input */ ) {

	throw NotImplemented();

}

void IO::Device::flush() {



}

// ============================================================================

Buffer IO::Device::dumpToBuffer() {

	Buffer output;

	if ( length() ) {
		output.supposedMaxSize( (Uint32)length() );
	}

	for (;;) {
		Buffer temp;
		try {
			temp = getBytes();
		}
		catch ( EndOfStream& ) {
			break;
		}
		output += temp;
	}

	return output;

}

// ============================================================================

Uint64 IO::Device::skip(
	const	Uint64		skipped ) {

	return seek( tell() + skipped );

}

void IO::Device::rewind() {

	(void)seek( Uint64( 0 ) );

}

Bool IO::Device::atEOS() const {

	return ( tell() == length() );

}

// ============================================================================

IO::Device & IO::operator << (
		IO::Device	& dev,
	const	Buffer		& buf ) {

	dev.putBytes( buf );

	return dev;

}

// ============================================================================

