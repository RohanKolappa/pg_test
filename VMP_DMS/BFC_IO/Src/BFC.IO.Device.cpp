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
//	BFC.IO.Device.cpp
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#include "BFC.IO.Device.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, Device )

// ============================================================================

IO::Device::Device() {

	setObjectName( "BFC.IO.Device" );

	accessMode	= NoAccess;
	connected	= false;

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
	const	Device &	o ) const {

	return ( this == &o );

}

Bool IO::Device::operator != (
	const	Device &	o ) const {

	return ( this != &o );

}

// ============================================================================

Bool IO::Device::hasGetTimeout() const {

	return ( getAbsGetTimeout() != NoTimeout );

}

Time::Delta IO::Device::getRelGetTimeout() const {

	return ( getAbsGetTimeout() - Time::now() );

}

void IO::Device::setRelGetTimeout(
	const	Time::Delta &	pRelTime ) {

	setAbsGetTimeout( Time::now() + pRelTime );

}

void IO::Device::resetGetTimeout() {

	setAbsGetTimeout( NoTimeout );

}

Bool IO::Device::hasPutTimeout() const {

	return ( getAbsPutTimeout() != NoTimeout );

}

Time::Delta IO::Device::getRelPutTimeout() const {

	return ( getAbsPutTimeout() - Time::now() );

}

void IO::Device::setRelPutTimeout(
	const	Time::Delta &	pRelTime ) {

	setAbsPutTimeout( Time::now() + pRelTime );

}

void IO::Device::resetPutTimeout() {

	setAbsPutTimeout( NoTimeout );

}

const Time::Clock IO::Device::NoTimeout;

// ============================================================================

Buffer IO::Device::getBytes() {

	throw NotImplemented();

}

void IO::Device::putBytes(
	const	Buffer &	/* input */ ) {

	throw NotImplemented();

}

void IO::Device::flush() {

}

// ============================================================================

Buffer IO::Device::dumpToBuffer() {

	Buffer output;

	if ( length() ) {
		output.supposedMaxSize( ( Uint32 )length() );
	}

	for (;;) {
		Buffer temp;
		try {
			temp = getBytes();
		}
		catch ( EndOfStream & ) {
			break;
		}
		output += temp;
	}

	return output;

}

void IO::Device::dumpToDevice(
		DevicePtr	pDevice ) {

	for (;;) {
		Buffer temp;
		try {
			temp = getBytes();
		}
		catch ( EndOfStream & ) {
			break;
		}
		pDevice->putBytes( temp );
	}

}

// ============================================================================

Uint64 IO::Device::skip(
	const	Uint64		skipped ) {

	return seek( tell() + skipped );

}

void IO::Device::rewind() {

	( void )seek( Uint64( 0 ) );

}

Bool IO::Device::atEOS() const {

	return ( tell() == length() );

}

// ============================================================================

IO::Device & IO::operator << (
		IO::Device &	dev,
	const	Buffer &	buf ) {

	dev.putBytes( buf );

	return dev;

}

// ============================================================================

void IO::Device::safeClose() {

	try {
		if ( isConnected() ) {
			close();
		}
	}
	catch ( Exception & ) {
	}

}

// ============================================================================

