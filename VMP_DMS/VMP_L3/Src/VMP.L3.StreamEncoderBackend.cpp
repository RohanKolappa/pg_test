// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.StreamEncoderBackend.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.StreamEncoderBackend.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, StreamEncoderBackend )

// ============================================================================

const DL::TypeCPtr & L3::StreamEncoderBackend::getClassType() {

	static DL::TypeCPtr i = new DL::Type(
		UUId( "c53acde4-bdc8-4941-884d-3ba73b0ef1b7" ),
		"VMP.L3.StreamEncoderBackend",
		L3::EncoderBackend::getClassType() );

	return i;

}

// ============================================================================

L3::StreamEncoderBackend::StreamEncoderBackend(
	const	BFC::DL::TypeCPtr &	pType ) :

	EncoderBackend( pType ) {

}

// ============================================================================

void L3::StreamEncoderBackend::attachIODevice(
		StreamEncoderODevicePtr	pODevice ) {

	if ( odevice ) {
		throw InternalError( "Already attached!" );
	}

	odevice = pODevice;

}

void L3::StreamEncoderBackend::detachIODevice() {

	if ( ! odevice ) {
		throw InternalError( "Not attached!" );
	}

	odevice.kill();

}

L3::StreamEncoderODevicePtr L3::StreamEncoderBackend::getIODevice() {

	return odevice;

}

// ============================================================================

Bool L3::StreamEncoderBackend::canEncode(
		SessionPtr		/*pSession*/ ) {

	return true;

}

// ============================================================================

void L3::StreamEncoderBackend::exitSession() {

}

// ============================================================================

void L3::StreamEncoderBackend::putHeader() {

}

void L3::StreamEncoderBackend::putTrailer() {

}

// ============================================================================

