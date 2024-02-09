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
//	BFC.Msg.ValueEvent.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.EventRegisterer.h"
#include "BFC.Msg.ValueEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================
// BFC::Msg::ValueEvent
// ============================================================================

BFC_PTR_IMPL_NS( Msg, ValueEvent )

// ============================================================================

const UUId & Msg::ValueEvent::classUUId() {

	static UUId id = Buffer( "15dcb601-a308-4134-96c3-a1ccfedef8f1" );

	return id;

}

// ============================================================================

Buffer Msg::ValueEvent::toBuffer() const {

	Buffer	b;

	switch ( mType ) {
	case TypeBoolean :
		b = "boolean: " + Buffer( mBoolean );
		break;
	case TypeInteger :
		b = "integer: " + Buffer( mInteger );
		break;
	case TypeFloating :
		b = "floating: " + Buffer( mDouble );
		break;
	case TypeBuffer :
		b = "buffer: " + mBuffer;
		break;
	}

	return "ValueEvent( " + b + " )";

}

// ============================================================================

void Msg::ValueEvent::throwException() const {

	throw BadCast();

}

// ============================================================================

void Msg::ValueEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	Event::doSerialize( pOStream );

	pOStream->putLEUint32( ( Uint32 )mType );

	switch ( mType ) {
	case TypeBoolean :
		pOStream->putLEUint32( mBoolean ? 1 : 0 );
		break;
	case TypeInteger :
		pOStream->putLEInt64( mInteger );
		break;
	case TypeFloating :
//		msgDbg( "doSerialize(): f: " + Buffer( mDouble ) );
		pOStream->putDouble( mDouble );
		break;
	case TypeBuffer :
		pOStream->putLEUint32( mBuffer.getLength() );
		pOStream->putBytes( mBuffer );
		break;
	default :
		throw InternalError();
	}

}

void Msg::ValueEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	Event::unSerialize( pIStream );

	mType = ( Type )pIStream->getLEUint32();

	if ( mType > TypeBuffer ) {
		throw InternalError();
	}

	switch ( mType ) {
	case TypeBoolean :
		mBoolean = ( pIStream->getLEUint32() == 1 );
		break;
	case TypeInteger :
		mInteger = pIStream->getLEInt64();
		break;
	case TypeFloating :
		mDouble = pIStream->getDouble();
//		msgDbg( "unSerialize(): f: " + Buffer( mDouble ) );
		break;
	case TypeBuffer : {
		Uint32 l = pIStream->getLEUint32();
		mBuffer = pIStream->getBytes( l );
		break;
		}
	}

}

// ============================================================================

static Msg::EventRegisterer< Msg::ValueEvent > valueEventRegisterer;

// ============================================================================
// BFC::Msg::BoundedValueEvent
// ============================================================================

BFC_PTR_IMPL_NS( Msg, BoundedValueEvent )

// ============================================================================

const UUId & Msg::BoundedValueEvent::classUUId() {

	static UUId id = Buffer( "6e524d5c-a879-4f62-a1c6-9f266b60f89a" );

	return id;

}

// ============================================================================

Buffer Msg::BoundedValueEvent::toBuffer() const {

	Buffer	b;

	switch ( getType() ) {
	case TypeInteger :
		b = "i: " + Buffer( getInt64() );
		break;
	case TypeFloating :
		b = "f: " + Buffer( getDouble() );
		break;
	default :
		throw InternalError();
	}

	return "BoundedValueEvent( " + b + " )";

}

// ============================================================================

void Msg::BoundedValueEvent::doSerialize(
		IO::ByteOutputStreamPtr		pOStream ) const {

	ValueEvent::doSerialize( pOStream );

	switch ( getType() ) {
	case TypeInteger :
		pOStream->putLEInt64( mMinInteger );
		pOStream->putLEInt64( mMaxInteger );
		break;
	case TypeFloating :
//		msgDbg( "doSerialize(): f: " + Buffer( mMinDouble ) + ", "
//			+ Buffer( mMaxDouble ) );
		pOStream->putDouble( mMinDouble );
		pOStream->putDouble( mMaxDouble );
		break;
	default :
		throw InternalError();
	}

}

void Msg::BoundedValueEvent::unSerialize(
		IO::ByteInputStreamPtr		pIStream ) {

	ValueEvent::unSerialize( pIStream );

	switch ( getType() ) {
	case TypeInteger :
		mMinInteger = pIStream->getLEInt64();
		mMaxInteger = pIStream->getLEInt64();
		break;
	case TypeFloating :
		mMinDouble = pIStream->getDouble();
		mMaxDouble = pIStream->getDouble();
//		msgDbg( "unSerialize(): f: " + Buffer( mMinDouble ) + ", "
//			+ Buffer( mMaxDouble ) );
		break;
	default :
		throw InternalError();
	}

}

// ============================================================================

static Msg::EventRegisterer< Msg::BoundedValueEvent > boundedValueEventRegisterer;

// ============================================================================


