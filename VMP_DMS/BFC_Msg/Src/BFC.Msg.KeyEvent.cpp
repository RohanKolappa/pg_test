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
//	BFC.Msg.KeyEvent.cpp
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
#include "BFC.Msg.KeyEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Msg, KeyEvent )

// ============================================================================

const UUId & Msg::KeyEvent::classUUId() {

	static UUId id = Buffer( "6ae0c264-5dad-40ae-9014-29f0aa470837" );

	return id;

}

// ============================================================================

void Msg::KeyEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Event::doSerialize( pOStream );

	pOStream->putLEInt32( id );

}

void Msg::KeyEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Event::unSerialize( pIStream );

	id = pIStream->getLEInt32();

}

// ============================================================================

static Msg::EventRegisterer< Msg::KeyEvent > registerer;

// ============================================================================

