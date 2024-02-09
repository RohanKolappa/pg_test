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
//	BFC.Msg.EventSerializer.cpp
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
#include "BFC.IO.MemoryInputStream.h"
#include "BFC.IO.MemoryOutputStream.h"

#include "BFC.Msg.EventRepository.h"
#include "BFC.Msg.EventSerializer.h"

// ============================================================================

using namespace BFC;

// ============================================================================

void Msg::EventSerializer::doSerialize(
		IO::ByteOutputStreamPtr	pOStream,
		EventCPtr		pEvent ) {

	pEvent->doSerialize( pOStream );
	pOStream->flush();

}

Msg::EventPtr Msg::EventSerializer::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	EventPtr res = EventRepository::instance()->getNewEvent( pIStream->peekUUId() );
	res->unSerialize( pIStream );
	return res;

}

// ============================================================================

Buffer Msg::EventSerializer::doSerialize(
		EventCPtr		pEvent ) {

	IO::MemoryOutputStreamPtr	oMStream = new IO::MemoryOutputStream;
	IO::ByteOutputStreamPtr		oBStream = new IO::ByteOutputStream( oMStream );

	oBStream->open();
	doSerialize( oBStream, pEvent );
	oBStream->flush();

	Buffer payload = oMStream->getContent();

	oBStream->close();

	return payload;

}

Msg::EventPtr Msg::EventSerializer::unSerialize(
	const	Buffer &		pBuffer ) {

	IO::MemoryInputStreamPtr	iMStream = new IO::MemoryInputStream( pBuffer );
	IO::ByteInputStreamPtr		iBStream = new IO::ByteInputStream( iMStream );

	iBStream->open();
	EventPtr event = unSerialize( iBStream );
	iBStream->close();

	return event;

}

// ============================================================================

