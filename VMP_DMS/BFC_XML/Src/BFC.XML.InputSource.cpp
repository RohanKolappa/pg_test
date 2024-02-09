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
//	BFC.XML.InputSource.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.IO.MemoryInputStream.h"

#include "BFC.XML.InputSource.h"

// ============================================================================

using namespace BFC;
using namespace BFC::XML;

// ============================================================================

BFC_PTR_IMPL( InputSource )

// ============================================================================

InputSource::InputSource(
	const	Buffer		& pBuffer ) {

	setObjectName( "BFC.XML.InputSource" );

	inputDevice = new IO::MemoryInputStream( pBuffer );

	try {
		inputDevice->open();
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't open device!" );
	}

	bufPtr = 0;
	bufLen = 0;
	bufPos = 0;

	safeFetch();

}

InputSource::InputSource(
		IO::DevicePtr	pDevice ) {

	setObjectName( "BFC.XML.InputSource" );

	inputDevice = pDevice;

	try {
		if ( ! inputDevice->isConnected() ) {
			inputDevice->open();
		}
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't open device!" );
	}

	bufPtr = 0;
	bufLen = 0;
	bufPos = 0;

	safeFetch();

}

InputSource::~InputSource() {



}

// ============================================================================

const Buffer & InputSource::getData() const {

	return buffer;

}

// ============================================================================

Uchar InputSource::getNext() {

	if ( bufPos == bufLen && bufPtr ) {
		buffer.kill();
		bufPtr = 0;
		bufPos = 0;
		bufLen = 0;
	}

	if ( ! bufLen ) {
		safeFetch();
		if ( ! bufLen ) {
			throw EndOfStream();
		}
	}

	return bufPtr[ bufPos++ ];

}

// ============================================================================

void InputSource::safeFetch() {

	if ( bufLen ) {
		return;
	}

	try {
		buffer = inputDevice->getBytes();
		bufPtr = buffer.getCstPtr();
		bufPos = 0;
		bufLen = buffer.getLength();
	}
	catch ( IO::EndOfStream & ) {
	}
	catch ( ThreadStopped & ) {
		throw;
	}
	catch ( Exception & e ) {
		msgExc( e, "Can't fetch data!" );
	}

}

// ============================================================================

