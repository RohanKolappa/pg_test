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
//	TBFC.IO.BtpDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "TBFC.IO.BtpDevice.h"

// ============================================================================

using namespace TBFC;

// ============================================================================

TBFC_PTR_IMPL_NS( IO, BtpDevice )

// ============================================================================

IO::BtpDevice::BtpDevice(
		ByteInputStreamPtr	idevice ) {

	setObjectName( "TBFC.IO.BtpDevice" );

	this->idevice = idevice;

}

IO::BtpDevice::BtpDevice(
		ByteOutputStreamPtr	odevice ) {

	setObjectName( "TBFC.IO.BtpDevice" );

	this->odevice = odevice;


}

IO::BtpDevice::~BtpDevice() {

	if ( isConnected() ) {
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "~BtpDevice(): can't auto-close device!" );
		}
	}

}

// ============================================================================

void IO::BtpDevice::open() {

	if ( isConnected() ) {
		throw AlreadyConnected();
	}

	if ( idevice ) {
		if ( ! idevice->isConnected() ) {
			idevice->open();
		}
		setAccess( ReadAccess );
	}
	else if ( odevice ) {
		if ( ! odevice->isConnected() ) {
			odevice->open();
		}
		setAccess( WriteAccess );
	}
	else {
		throw CantOpen( "No peer device!" );
	}

	index = 0;

}

void IO::BtpDevice::close() {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( idevice ) {
		if ( idevice->isConnected() ) {
			idevice->close();
		}
		else {
			msgWrn( "close(): peer not open!" );
		}
	}
	else if ( odevice ) {
		if ( odevice->isConnected() ) {
			odevice->close();
		}
		else {
			msgWrn( "close(): peer not open!" );
		}
	}
	else {
		throw CantClose( "No peer device!" );
	}

	resetAccess();

}

// ============================================================================

Buffer IO::BtpDevice::getBytes() {

	// Sanity checks...

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasReadAccess() ) {
		throw NoReadAccess();
	}

	// Let's read a new BTP packet...

	// ... receive sizeof(int)

	Uchar c;

	try {
		c = idevice->getUchar();
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantGetBytes( e, "... while reading sizeof(int)" );
	}

	if ( c != (Uchar)sizeof( Uint32 ) ) {
		msgWrn( "Invalid sizeof( int )!" );
		msgWrn( "... expected: " + Buffer( (Uint32)sizeof( Uint32 )) );
		msgWrn( "... but rcvd: " + Buffer( (Uint32)c ) );
		try {
			close();
		}
		catch ( Exception& e ) {
			msgExc( e, "... and can't auto-close peer!" );
		}
		resetAccess();
		throw BrokenPipe();
	}

	// ... receive buffer.length()

	Uint32 l;

	try {
		l = idevice->getLEUint32();
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantGetBytes( e, "... while reading buffer length" );
	}


	Buffer target;

	if ( ! l ) {
		return target;
	}

	// ... receive buffer

	try {
		target = idevice->getBytes( l );
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantGetBytes( e, "... while reading buffer payload" );
	}

	index += (Uint64)target.length();

	return target;

}

void IO::BtpDevice::putBytes(
	const	Buffer&		input ) {

	// Sanity checks...

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	// Note it is perfectly correct to send an empty buffer...

	// ... send sizeof( int )

	try {
		odevice->putUchar( (Uchar)( sizeof( Uint32 ) ) );
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantPutBytes( e, "... while writing sizeof(int)" );
	}

	// ... send input.length()

	try {
		odevice->putLEUint32( input.length() );
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantPutBytes( e, "... while writing buffer length" );
	}

	// ... send input

	if ( input.isEmpty() ) {
		return;
	}

	try {
		odevice->putBytes( input );
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantPutBytes( e, "... while writing buffer payload" );
	}

	index += (Uint64)input.length();

}

void IO::BtpDevice::flush() {

	// Sanity checks...

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( ! hasWriteAccess() ) {
		throw NoWriteAccess();
	}

	try {
		odevice->flush();
	}
	catch ( Device::BrokenPipe& ) {
		resetAccess();
		throw;
	}
	catch ( Exception& e ) {
		throw CantPutBytes( e );
	}

}

// ============================================================================

Uint64 IO::BtpDevice::seek(
	const	Uint64		offset ) {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	if ( offset != tell() ) {
		throw NoRandomAccess();
	}

	return offset;

}

Uint64 IO::BtpDevice::tell() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return index;

}

Uint64 IO::BtpDevice::length() const {

	return Uint64( 0 );

}

Bool IO::BtpDevice::atEOS() const {

	if ( ! isConnected() ) {
		throw NotConnected();
	}

	return ( odevice ? odevice->atEOS() : idevice->atEOS() );

}

// ============================================================================





