// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.Event.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Event.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.EventRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Dyn::ItemsDeleted
// ============================================================================

BFC_PTR_IMPL_NS( Dyn, ItemsDeleted )

// ============================================================================

const UUId & Dyn::ItemsDeleted::classUUId() {

	static UUId id = Buffer( "deac1765-2e53-4ebb-b79b-57922aead8d8" );

	return id;

}

// ============================================================================

Buffer Dyn::ItemsDeleted::toBuffer() const {

	return "ItemsDeleted()";

}

// ============================================================================

static Msg::EventRegisterer< Dyn::ItemsDeleted > itemsDeletedRegisterer;

// ============================================================================
// BFC::Dyn::ItemAppended
// ============================================================================

BFC_PTR_IMPL_NS( Dyn, ItemAppended )

// ============================================================================

const UUId & Dyn::ItemAppended::classUUId() {

	static UUId id = Buffer( "c6fe5167-c8a4-4f24-8f93-dfe6621369da" );

	return id;

}

// ============================================================================

Buffer Dyn::ItemAppended::toBuffer() const {

	return "ItemAppended( \"" + itemName + "\" )";

}

// ============================================================================

void Dyn::ItemAppended::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Event::doSerialize( pOStream );

	pOStream->putLEUint32( itemName.getLength() );
	pOStream->putBytes( itemName );

}

void Dyn::ItemAppended::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Event::unSerialize( pIStream );

	Uint32 l = pIStream->getLEUint32();
	itemName = pIStream->getBytes( l );

}

// ============================================================================

static Msg::EventRegisterer< Dyn::ItemAppended > itemAppendedRegisterer;

// ============================================================================

