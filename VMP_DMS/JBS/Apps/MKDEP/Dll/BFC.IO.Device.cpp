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
using namespace BFC::IO;

// ============================================================================

BFC_PTR_IMPL(Device)

// ============================================================================

Device::Device() {

	connected    = false;
	closeOnExit  = true;
	readAccess   = false;
	writeAccess  = false;
	randomAccess = false;

}

Device::~Device() {



}

// ============================================================================

Bool Device::operator == (
	const	Device&		o) const {

	return (this == &o);

}

Bool Device::operator != (
	const	Device&		o) const {

	return (this != &o);

}

// ============================================================================

Bool Device::isConnected() {

	return connected;

}

Bool Device::willCloseOnExit() const {

	return closeOnExit;

}

Bool Device::hasReadAccess() const {

	return readAccess;

}

Bool Device::hasWriteAccess() const {

	return writeAccess;

}

Bool Device::hasRandomAccess() const {

	return randomAccess;

}

// ============================================================================

Buffer Device::getBytes() {

	throw NotImplemented();

}

void Device::putBytes(
	const	Buffer&		) {

	throw NotImplemented();

}

void Device::flush() {



}

// ============================================================================

Buffer Device::dumpToBuffer() {

	// Sanity checks...

	if (!isConnected()) {
		throw NotConnected("In Device::dumpToBuffer()");
	}

	if (!hasReadAccess()) {
		throw NoReadAccess();
	}

	Buffer output;

	// Dump content...

	for (;;) {
		Buffer temp;
		try {
			temp = getBytes();
		}
		catch (EndOfStream&) {
			break;
		}
		catch (Exception& e) {
			throw CantGetBytes( e );
		}
		output += temp;
	}

	return output;

}

// ============================================================================

Uint32 Device::skip(
	const	Uint32		skipped) {

	return seek(tell() + skipped);

}

void Device::rewind() {

	(void)seek(0);

}

Bool Device::atEOS() const {

	return (tell() == length());

}

// ============================================================================

void Device::setConnected(Bool b) {

	connected = b;

}

// ============================================================================

