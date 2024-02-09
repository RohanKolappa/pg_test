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
//	BFC.Msg.ChildEvent.cpp
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

#include "BFC.Msg.ChildEvent.h"
#include "BFC.Msg.EventRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, ChildEvent )

// ============================================================================

const UUId & Msg::ChildEvent::classUUId() {

	static UUId id = Buffer( "01e156c7-069c-4a97-a56d-0b5c0c3d6ca5" );

	return id;

}

// ============================================================================

Buffer Msg::ChildEvent::toBuffer() const {

	return "ChildEvent( " + child + ", " + event->toBuffer() + " )";

}

// ============================================================================

void Msg::ChildEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Event::doSerialize( pOStream );

	pOStream->putLEUint32( child.getLength() );
	pOStream->putBytes( child );

	event->doSerialize( pOStream );

}

void Msg::ChildEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Event::unSerialize( pIStream );

	Uint32 l = pIStream->getLEUint32();
	child = pIStream->getBytes( l );

	EventPtr res = EventRepository::instance()->getNewEvent( pIStream->peekUUId() );
	res->unSerialize( pIStream );

	event = res;

}

// ============================================================================

static Msg::EventRegisterer< Msg::ChildEvent > registerer;

// ============================================================================

