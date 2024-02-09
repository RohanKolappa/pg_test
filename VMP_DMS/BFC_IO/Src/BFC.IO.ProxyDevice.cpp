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
//	BFC.IO.ProxyDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ProxyDevice.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, ProxyDevice )

// ============================================================================

IO::ProxyDevice::ProxyDevice(
		DevicePtr	pPeer ) :

	peer	( pPeer ) {

	setObjectName( "BFC.IO.ProxyDevice" );

}

IO::ProxyDevice::~ProxyDevice() {

	if ( isConnected() ) {
		closePeer();
	}

}

// ============================================================================

void IO::ProxyDevice::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( ! peer ) {
		throw CantOpen( "No peer device!" );
	}

	if ( peer->isConnected() ) {
		// KAE: when using streams, we constantly get this msg, but it isn't an error, so disable this
		//msgWrn( "open(): peer already open!" );
		clse = false;
	}
	else {
		peer->open();
		clse = true;
	}

	setAccess( peer->getAccessMode() );

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

	closePeer();

}

// ============================================================================

void IO::ProxyDevice::setBlockSize(
	const	Uint32		pSize ) {

	peer->setBlockSize( pSize );

}

Uint32 IO::ProxyDevice::getBlockSize() const {

	return peer->getBlockSize();

}

// ============================================================================

const Time::Clock & IO::ProxyDevice::getAbsGetTimeout() const {

	return ( getPeer()->getAbsGetTimeout() );

}

void IO::ProxyDevice::setAbsGetTimeout(
	const	Time::Clock &	pAbsTime ) {

	getPeer()->setAbsGetTimeout( pAbsTime );

}

const Time::Clock & IO::ProxyDevice::getAbsPutTimeout() const {

	return ( getPeer()->getAbsPutTimeout() );

}

void IO::ProxyDevice::setAbsPutTimeout(
	const	Time::Clock &	pAbsTime ) {

	getPeer()->setAbsPutTimeout( pAbsTime );

}

// ============================================================================

void IO::ProxyDevice::flush() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	try {
		peer->flush();
	}
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}

}

// ============================================================================

void IO::ProxyDevice::closePeer() {

	if ( clse ) {
		if ( peer->isConnected() ) {
			try {
				peer->close();
			}
			catch ( Exception & e ) {
				msgExc( e, "closePeer(): can't close peer device!" );
			}
		}
		else {
			msgWrn( "closePeer(): peer not connected!" );
		}
	}

	resetAccess();

}

// ============================================================================

