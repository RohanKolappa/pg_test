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
//	BFC.Net.MACAddress.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"
#include "BFC.Math.Utils.h"
#include "BFC.Net.MACAddress.h"

// ============================================================================

using namespace BFC;
using namespace BFC::Net;

// ============================================================================

MACAddress::MACAddress() {

	isOk = false;

}

MACAddress::MACAddress(
	const	MACAddress&	other) {

	if ( ( isOk = other.isOk ) ) {
		addr = other.addr;
	}

}

MACAddress::MACAddress(
	const	Buffer&		address) {

	isOk = false;
	addr.kill();

	if ( address.isEmpty() ) {
		throw EmptyArgument();
	}

	if ( address.length() > 255 ) {
		throw ArgumentOverflow();
	}

	BufferArray values;

	try {
		values = BufferParser::tokenize( address, ":." );
	}
	catch ( Exception& ) {
		throw BadArgument( address );
	}

	if ( values.size() != 6 ) {
		throw MisformedAddress( address );
	}

	Uint32	pos = 0,
		val,
		i;

	for ( i = 0 ; i < 6 ; i++ ) {
		pos = 0;
		val = values[ i ].getUintAt( pos );
		if ( ! pos || pos != values[ i ].length() ) {
			throw InvalidElement(/*Buffer("Elt. ")
			                     + i
			                     + " in `"
			                     + address
			                     + "'"*/);
		}
		if ( val > 255 ) {
			throw ElementOverflow(/*Buffer("Elt. ")
			                     + i
			                     + " in `"
			                     + address
			                     + "'"*/);
		}
		addr += (Uchar)val;
	}

	isOk = true;

}

MACAddress::~MACAddress() {



}

// ============================================================================

MACAddress& MACAddress::operator = (
	const	MACAddress&	other ) {

	if ( this != &other && ( ( isOk = other.isOk ) ) ) {
		addr = other.addr;
	}

	return *this;

}

// ============================================================================

void MACAddress::buildFromRawBuffer(
	const	Buffer&		rawBuffer) {

	if ( rawBuffer.length() != 6 ) {
		throw BadArgument();
	}

	addr = rawBuffer;
	isOk = true;

}

// ============================================================================

Buffer MACAddress::getDot() const {

	if ( ! isOk ) {
		return Buffer();
	}

	Buffer res;

	for ( Uint32 i = 0 ; i < 6 ; i++ ) {
		if ( i ) {
			res += ':';
		}
		res += (Uint32)addr[ i ];
	}

	return res;

}

// ============================================================================

Bool MACAddress::isNull() const {

	return ( isOk
		&& addr[ 0 ] == 0x0
		&& addr[ 1 ] == 0x0
		&& addr[ 2 ] == 0x0
		&& addr[ 3 ] == 0x0
		&& addr[ 4 ] == 0x0
		&& addr[ 5 ] == 0x0 );

}

// ============================================================================



