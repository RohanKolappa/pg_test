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
//	VMP.L3.StreamEncoderODevice.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "VMP.L3.StreamEncoderODevice.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( L3, StreamEncoderODevice )

// ============================================================================

L3::StreamEncoderODevice::StreamEncoderODevice() {

}

// ============================================================================

void L3::StreamEncoderODevice::open() {

//	msgDbg( "open()" );

	if ( isOpen() ) {
		throw AlreadyConnected();
	}

	setAccess( IO::Device::WriteAccess | IO::Device::RandomAccess );

	curr = 0;
	fifo.kill();

}

void L3::StreamEncoderODevice::close() {

//	msgDbg( "close()" );

	if ( ! isOpen() ) {
		throw NotConnected();
	}

	resetAccess();

}

// ============================================================================

Bool L3::StreamEncoderODevice::hasFrame() const {

	return ( ! fifo.isEmpty() );

}

FramePtr L3::StreamEncoderODevice::getFrame() {

	FramePtr res = fifo.top();
	fifo.pop();
	return res;

}

// ============================================================================

void L3::StreamEncoderODevice::putBytes(
	const	Buffer &	pOutputBuffer ) {

	BytesFramePtr frame = new BytesFrame;

	frame->setData( pOutputBuffer );
	frame->setOffset( curr );

	fifo.push( frame );

	curr += pOutputBuffer.getLength();

}

Uint64 L3::StreamEncoderODevice::seek(
	const	Uint64		pOffset ) {

	return ( ( curr = pOffset ) );

}

Uint64 L3::StreamEncoderODevice::tell() const {

	return curr;

}

Uint64 L3::StreamEncoderODevice::length() const {

	return 0;

}

void L3::StreamEncoderODevice::putFrame(
		BytesFramePtr	pOutputFrame ) {

	fifo.push( pOutputFrame );

	if ( pOutputFrame->hasOffset() ) {
		curr = pOutputFrame->getOffset();
	}

	curr += pOutputFrame->getData().getLength();

}

// ============================================================================

