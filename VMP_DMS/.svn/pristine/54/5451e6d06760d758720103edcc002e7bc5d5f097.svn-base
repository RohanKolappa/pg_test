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
//	BFC.IO.BtpDevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.BtpDevice.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( IO, BtpDevice )

// ============================================================================

IO::BtpDevice::BtpDevice(
		ByteInputStreamPtr	idevice ) :

	ProxyDevice( idevice ) {

	setObjectName( "BFC.IO.BtpDevice" );

	this->idevice = idevice;

}

IO::BtpDevice::BtpDevice(
		ByteOutputStreamPtr	odevice ) :

	ProxyDevice( odevice ) {

	setObjectName( "BFC.IO.BtpDevice" );

	this->odevice = odevice;


}

// ============================================================================

void IO::BtpDevice::open() {

	ProxyDevice::open();

	index = 0;

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
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}

	if ( c != (Uchar)sizeof( Uint32 ) ) {
		msgWrn( "Invalid sizeof( int )!" );
		msgWrn( "... expected: " + Buffer( (Uint32)sizeof( Uint32 )) );
		msgWrn( "... but rcvd: " + Buffer( (Uint32)c ) );
		safeClose();
		throw BrokenPipe();
	}

	// ... receive buffer.length()

	Uint32 l;

	try {
		l = idevice->getLEUint32();
	}
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}
	catch ( WouldBlock & ) {
		safeClose();
		throw BrokenPipe();
	}

	Buffer target;

	if ( ! l ) {
		return target;
	}

	// ... receive buffer

	try {
		target = idevice->getBytes( l );
	}
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}
	catch ( WouldBlock & ) {
		safeClose();
		throw BrokenPipe();
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
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}

	// ... send input.length()

	try {
		odevice->putLEUint32( input.length() );
	}
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}
	catch ( WouldBlock & ) {
		safeClose();
		throw BrokenPipe();
	}

	// ... send input

	if ( input.isEmpty() ) {
		return;
	}

	try {
		odevice->putBytes( input );
	}
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
	}
	catch ( WouldBlock & ) {
		safeClose();
		throw BrokenPipe();
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
	catch ( BrokenPipe & ) {
		resetAccess();
		throw;
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

