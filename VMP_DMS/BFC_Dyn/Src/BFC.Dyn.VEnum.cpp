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
//	BFC.Dyn.VEnum.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Event.h"
#include "BFC.Dyn.VEnum.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.ValueEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VEnum )

// ============================================================================

Dyn::VEnum::VEnum() :

	Var	( new TEnum ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	Buffer &	pVarName ) :

	Var	( new TEnum, pVarName ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	char *		pTable[] ) :

	Var	( new TEnum( pTable ) ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	Buffer &	pVarName,
	const	char *		pTable[] ) :

	Var	( new TEnum( pTable ), pVarName ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	TEnumCPtr &	pTEnum ) :

	Var	( pTEnum ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	Buffer &	pVarName,
	const	TEnumCPtr &	pTEnum ) :

	Var	( pTEnum, pVarName ),
	v	( 0 ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::VEnum(
	const	VEnum &		pOther ) :

	SObject		(),
	Cloneable	(),
	Serializable	(),
	Invokable	(),
	Observable	(),
	Observer	(),
	Var		( pOther.t ),
	v		( pOther.v ) {

	t = getType();

	addCallback( this, &VEnum::setValueCB );

}

Dyn::VEnum::~VEnum() {

	delCallbacks();

}

// ============================================================================

Dyn::VEnum & Dyn::VEnum::operator = (
	const	VEnum &		pOther ) {

	Var::operator = ( pOther );

	if ( this != &pOther ) {
		t = pOther.t;
		v = pOther.v;
	}

	return *this;

}

// ============================================================================

SPtr Dyn::VEnum::clone() const {

	return new VEnum( *this );

}

// ============================================================================

void Dyn::VEnum::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putLEUint32( v );

}

void Dyn::VEnum::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	v = pIStream->getLEUint32();

}

// ============================================================================

Buffer Dyn::VEnum::toBuffer() const {

	return Buffer( v );

}

void Dyn::VEnum::fromBuffer(
	const	Buffer &	pBuffer ) {

	setValue( pBuffer.toUint32() );

}

// ============================================================================

void Dyn::VEnum::setValue(
	const	Uint32		pValue ) {

	privSetValue( pValue );

}

void Dyn::VEnum::setValue(
	const	Buffer &	pValue ) {

	privSetValue( pValue );

}

// ============================================================================

void Dyn::VEnum::setValueCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr	ev = pEvent;

	switch ( ev->getType() ) {

	case Msg::ValueEvent::TypeBuffer :
		privSetValue( ev->getBuffer() );
		break;

	case Msg::ValueEvent::TypeInteger :
		privSetValue( ev->getUint32() );
		break;

	default :
		throw InvalidArgument();

	}

}

// ============================================================================

//void Dyn::VEnum::addEntry(
//	const	Buffer &	pBuffer ) {
//
//	crashIfLocked();
//
//	t->addEntry( pBuffer );
//
//	warnObservers( new Dyn::ItemAppended( pBuffer ) );
//
//}
//
//void Dyn::VEnum::delEntries() {
//
//	crashIfLocked();
//
//	t = new TEnum;
//
//	warnObservers( new Dyn::ItemsDeleted );
//
//	v = 0;
//
//	warnObservers( new Msg::ValueEvent( v ) );
//
//}

// ============================================================================

void Dyn::VEnum::syncObservers() const {

	warnObservers( new Dyn::ItemsDeleted );

	for ( Uint32 i = 0 ; i < t->getSize() ; i++ ) {
		warnObservers( new Dyn::ItemAppended( t->getEntry( i ) ) );
	}

	warnObservers( new Msg::ValueEvent( v ) );

}

// ============================================================================

void Dyn::VEnum::privSetValue(
	const	Uint32		pValue ) {

	if ( pValue != v ) {
		v = pValue;
		warnObservers( new Msg::ValueEvent( v ) );
	}

}

void Dyn::VEnum::privSetValue(
	const	Buffer &	pValue ) {

	privSetValue( getEntryIndex( pValue ) );

}

// ============================================================================

