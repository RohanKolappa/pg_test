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
//	BFC.DL.Event.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.DL.Event.h"
#include "BFC.DL.Factory.h"
#include "BFC.DL.Type.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.EventRegisterer.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::DL::RepositoryAddedEvent
// ============================================================================

BFC_PTR_IMPL_NS( DL, RepositoryAddedEvent )

// ============================================================================

const UUId & DL::RepositoryAddedEvent::classUUId() {

	static UUId id = Buffer( "13218221-4a76-4aba-b978-5735f4364664" );

	return id;

}

// ============================================================================

Buffer DL::RepositoryAddedEvent::toBuffer() const {

	return "RepositoryAddedEvent()";

}

// ============================================================================

void DL::RepositoryAddedEvent::doSerialize(
		IO::ByteOutputStreamPtr		/*pOStream*/ ) const {

	throw NotImplemented();

}

void DL::RepositoryAddedEvent::unSerialize(
		IO::ByteInputStreamPtr		/*pIStream*/ ) {

	throw NotImplemented();

}

// ============================================================================

static Msg::EventRegisterer< DL::RepositoryAddedEvent > repositoryAddedRegisterer;

// ============================================================================
// BFC::DL::RepositoryDeletedEvent
// ============================================================================

BFC_PTR_IMPL_NS( DL, RepositoryDeletedEvent )

// ============================================================================

const UUId & DL::RepositoryDeletedEvent::classUUId() {

	static UUId id = Buffer( "9c41d599-4eb0-4e4e-bc52-9119e0be3cb8" );

	return id;

}

// ============================================================================

Buffer DL::RepositoryDeletedEvent::toBuffer() const {

	return "RepositoryDeletedEvent()";

}

// ============================================================================

void DL::RepositoryDeletedEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Msg::Event::doSerialize( pOStream );

	pOStream->putUUId( uuid );

}

void DL::RepositoryDeletedEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Msg::Event::unSerialize( pIStream );

	uuid = pIStream->getUUId();

}

// ============================================================================

static Msg::EventRegisterer< DL::RepositoryDeletedEvent > repositoryDeletedRegisterer;

// ============================================================================
// BFC::DL::FactoryAddedEvent
// ============================================================================

BFC_PTR_IMPL_NS( DL, FactoryAddedEvent )

// ============================================================================

const UUId & DL::FactoryAddedEvent::classUUId() {

	static UUId id = Buffer( "6395e185-653b-433a-9e2e-5268bec64dd4" );

	return id;

}

// ============================================================================

DL::FactoryAddedEvent::FactoryAddedEvent() :

	Msg::Event	( classUUId() ) {

}

DL::FactoryAddedEvent::FactoryAddedEvent(
		FactoryPtr	pFactory ) :

	Msg::Event	( classUUId() ),
	fact		( pFactory ),
	desc		( pFactory->getType()->getData() ) {

}

// ============================================================================

Buffer DL::FactoryAddedEvent::toBuffer() const {

	return "FactoryAddedEvent()";

}

// ============================================================================

void DL::FactoryAddedEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Msg::Event::doSerialize( pOStream );

//	pOStream->putUUId( uuid );
//	pOStream->putLEUint32( name.getLength() );
//	pOStream->putBytes( name );

}

void DL::FactoryAddedEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Msg::Event::unSerialize( pIStream );

//	uuid = pIStream->getUUId();
//	name = pIStream->getBytes( pIStream->getLEUint32() );

}

// ============================================================================

static Msg::EventRegisterer< DL::FactoryAddedEvent > factoryAddedRegisterer;

// ============================================================================
// BFC::DL::FactoryDeletedEvent
// ============================================================================

BFC_PTR_IMPL_NS( DL, FactoryDeletedEvent )

// ============================================================================

const UUId & DL::FactoryDeletedEvent::classUUId() {

	static UUId id = Buffer( "5d0c92db-463e-4b17-9806-54445bd45fa8" );

	return id;

}

// ============================================================================

DL::FactoryDeletedEvent::FactoryDeletedEvent() :

	Msg::Event	( classUUId() ) {

}

DL::FactoryDeletedEvent::FactoryDeletedEvent(
		FactoryPtr	pFactory ) :

	Msg::Event	( classUUId() ),
	fact		( pFactory ),
	desc		( pFactory->getType()->getData() ) {

}

// ============================================================================

Buffer DL::FactoryDeletedEvent::toBuffer() const {

	return "FactoryDeletedEvent()";

}

// ============================================================================

void DL::FactoryDeletedEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Msg::Event::doSerialize( pOStream );

//	pOStream->putUUId( uuid );
//	pOStream->putLEUint32( name.getLength() );
//	pOStream->putBytes( name );

}

void DL::FactoryDeletedEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Msg::Event::unSerialize( pIStream );

//	uuid = pIStream->getUUId();
//	name = pIStream->getBytes( pIStream->getLEUint32() );

}

// ============================================================================

static Msg::EventRegisterer< DL::FactoryDeletedEvent > factoryDeletedRegisterer;

// ============================================================================

