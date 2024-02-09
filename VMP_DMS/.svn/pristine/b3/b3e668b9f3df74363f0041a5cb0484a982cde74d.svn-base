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
//	BFC.Msg.ResizeEvent.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.EventRegisterer.h"
#include "BFC.Msg.ResizeEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, ResizeEvent )

// ============================================================================

const UUId & Msg::ResizeEvent::classUUId() {

	static UUId id = Buffer( "22f50c16-28ae-496c-a188-47e18f49bb35" );

	return id;

}

// ============================================================================

Buffer Msg::ResizeEvent::toBuffer() const {

	return "ResizeEvent( " + Buffer( newSize ) + " )";

}

// ============================================================================

void Msg::ResizeEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Event::doSerialize( pOStream );

	pOStream->putLEUint32( newSize );

}

void Msg::ResizeEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Event::unSerialize( pIStream );

	newSize = pIStream->getLEUint32();

}

// ============================================================================

static Msg::EventRegisterer< Msg::ResizeEvent > registerer;

// ============================================================================

