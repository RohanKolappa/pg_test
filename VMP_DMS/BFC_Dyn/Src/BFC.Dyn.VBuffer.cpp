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
//	BFC.Dyn.VBuffer.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "BFC.Dyn.TBuffer.h"
#include "BFC.Dyn.VBuffer.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VBuffer )

// ============================================================================

Dyn::VBuffer::VBuffer() :

	Var	( TBuffer::instance() ) {

	addCallback( this, &VBuffer::setValueCB );

}

Dyn::VBuffer::VBuffer(
	const	Buffer &	pBuffer ) :

	Var	( TBuffer::instance() ),
	v	( pBuffer ) {

	addCallback( this, &VBuffer::setValueCB );

}

Dyn::VBuffer::VBuffer(
	const	Buffer &	pVarName,
	const	Buffer &	pBuffer ) :

	Var	( TBuffer::instance(), pVarName ),
	v	( pBuffer ) {

	addCallback( this, &VBuffer::setValueCB );

}

Dyn::VBuffer::VBuffer(
	const	TBufferPtr &	pTBuffer ) :

	Var	( pTBuffer ) {

	addCallback( this, &VBuffer::setValueCB );

}

Dyn::VBuffer::VBuffer(
	const	VBuffer &	pOther ) :

	SObject	(),
	Cloneable(),
	Serializable(),
//	Controllable(),
	Invokable(),
	Observable(),
	Observer(),
//	IVar(),
	Var	( pOther ),
	v	( pOther.v ) {

	addCallback( this, &VBuffer::setValueCB );

}

// ============================================================================

Dyn::VBuffer::~VBuffer() {

	delCallbacks();

}

// ============================================================================

Dyn::VBuffer & Dyn::VBuffer::operator = (
	const	VBuffer &	pOther ) {

	Var::operator = ( pOther );

	privSetValue( pOther.v );

	return *this;

}

// ============================================================================

void Dyn::VBuffer::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

	if ( pTag.isEmpty() || pTag == "val" ) {
		setValue( pVal );
	}
	else {
		Var::set( pTag, pVal );
	}

}

Buffer Dyn::VBuffer::get(
	const	Buffer &	pTag ) const {

	Buffer res;

	if ( pTag.isEmpty() || pTag == "val" ) {
		res = getValue();
	}
	else {
		res = Var::get( pTag );
	}

	return res;

}

// ============================================================================

SPtr Dyn::VBuffer::clone() const {

	return new VBuffer( *this );

}

// ============================================================================

void Dyn::VBuffer::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putLEUint32( v.getLength() );
	pOStream->putBytes( v );

}

void Dyn::VBuffer::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	Uint32 l = pIStream->getLEUint32();
	v = pIStream->getBytes( l );

}

// ============================================================================

Buffer Dyn::VBuffer::toBuffer() const {

	return v;

}

void Dyn::VBuffer::fromBuffer(
	const	Buffer &	pBuffer ) {

	setValue( pBuffer );

}

// ============================================================================

void Dyn::VBuffer::setValue(
	const	Buffer &	pValue ) {

	privSetValue( pValue );

}

// ============================================================================

void Dyn::VBuffer::setValueCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr	ev = pEvent;

	switch ( ev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		privSetValue( Buffer( ev->getBool() ) );
		break;

	case Msg::ValueEvent::TypeBuffer :
		privSetValue( ev->getBuffer() );
		break;

	case Msg::ValueEvent::TypeInteger :
		privSetValue( Buffer( ev->getInt64() ) );
		break;

	case Msg::ValueEvent::TypeFloating :
		privSetValue( Buffer( ev->getDouble() ) );
		break;

	}

}

// ============================================================================

void Dyn::VBuffer::syncObservers() const {

	warnObservers( new Msg::ValueEvent( v ) );

}

// ============================================================================

void Dyn::VBuffer::privSetValue(
	const	Buffer &	pValue ) {

	if ( pValue != v ) {
		v = pValue;
		syncObservers();
	}

}

// ============================================================================

