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
//	BFC.Msg.Event.cpp
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

#include "BFC.Msg.Event.h"

// ============================================================================

using namespace BFC;

// ============================================================================

const UUId & Msg::Event::classUUId() {

	static UUId id = Buffer( "c3deae5c-81ee-4b74-a757-23b476f7e6a9" );

	return id;

}

// ============================================================================

Buffer Msg::Event::toBuffer() const {

	return Buffer( "<anonymous event>" );

}

// ============================================================================

void Msg::Event::setSender(
		ObservableCPtr	pSender ) {

	pSender.setMaster( false );

	sender = pSender;

}

// ============================================================================

void Msg::Event::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	pOStream->putUUId( uuid );

}

void Msg::Event::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	uuid = pIStream->getUUId();

}

// ============================================================================

