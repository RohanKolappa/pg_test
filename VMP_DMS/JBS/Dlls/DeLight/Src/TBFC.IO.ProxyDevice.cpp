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
//	TBFC.IO.ProxyDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.IO.ProxyDevice.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL_NS( IO, ProxyDevice )

// ============================================================================

IO::ProxyDevice::ProxyDevice(
		DevicePtr	device ) {

	setObjectName( "TBFC.IO.ProxyDevice" );

	this->device = device;

}

IO::ProxyDevice::~ProxyDevice() {

	if ( isConnected() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "~ProxyDevice(): can't auto-close proxy device!" );
		}
	}

}

// ============================================================================

void IO::ProxyDevice::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! device ) {
		throw CantOpen( "No peer device!" );
	}

	if ( device->isConnected() ) {
		msgWrn( "open(): peer already open!" );
	}
	else {
		device->open();
	}

	setAccess( device->getAccessMode() );

}

void IO::ProxyDevice::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	try {
		if ( hasWriteAccess() ) {
			flush();
		}
	}
	catch ( Exception & e ) {
		msgExc( e, "close(): can't flush peer device!" );
	}

	if ( device->isConnected() ) {
		try {
			device->close();
		}
		catch ( Exception & e ) {
			msgExc( e, "close(): can't close peer device!" );
		}
	}
	else {
		msgWrn( "close(): peer not connected!" );
	}

	resetAccess();

}

// ============================================================================

void IO::ProxyDevice::flush() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	try {
		device->flush();
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantFlush( e );
	}

}

// ============================================================================

