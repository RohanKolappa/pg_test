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
//	BFC.Dyn.VBool.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TBool.h"
#include "BFC.Dyn.VBool.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.ValueEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VBool )

// ============================================================================

Dyn::VBool::VBool() :

	Var	( TBool::instance() ),
	v	( false ) {

	addCallback( this, &VBool::setValueCB );

}

Dyn::VBool::VBool(
	const	Bool		pBool ) :

	Var	( TBool::instance() ),
	v	( pBool ) {

	addCallback( this, &VBool::setValueCB );

}

Dyn::VBool::VBool(
	const	Buffer &	pVarName,
	const	Bool		pBool ) :

	Var	( TBool::instance(), pVarName ),
	v	( pBool ) {

	addCallback( this, &VBool::setValueCB );

}

Dyn::VBool::VBool(
	const	VBool &		pOther ) :

	SObject 	(),
	Cloneable	(),
	IO::Serializable(),
//	Controllable	(),
	Msg::Invokable	(),
	Msg::Observable	(),
	Msg::Observer	(),
//	IVar		(),
	Var		( pOther ),
	v		( pOther.v ) {

	addCallback( this, &VBool::setValueCB );

}

// ============================================================================

Dyn::VBool::~VBool() {

	delCallbacks();

}

// ============================================================================

Dyn::VBool & Dyn::VBool::operator = (
	const	VBool &		pOther ) {

	Var::operator = ( pOther );

	privSetValue( pOther.v );

	return *this;

}

// ============================================================================

SPtr Dyn::VBool::clone() const {

	return new VBool( *this );

}

// ============================================================================

void Dyn::VBool::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putUchar( v ? 0x01 : 0x00 );

}

void Dyn::VBool::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	v = ( pIStream->getUchar() == 0x01 );

}

// ============================================================================

Buffer Dyn::VBool::toBuffer() const {

	return Buffer( v );

}

void Dyn::VBool::fromBuffer(
	const	Buffer &	pBuffer ) {

	setValue( pBuffer );

}

// ============================================================================

void Dyn::VBool::setValue(
	const	Bool		pValue ) {

	privSetValue( pValue );

}

void Dyn::VBool::setValue(
	const	Buffer &	pBuffer ) {

	privSetValue( pBuffer );

}

// ============================================================================

void Dyn::VBool::setValueCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr	ev = pEvent;

	switch ( ev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		privSetValue( ev->getBool() );
		break;

	case Msg::ValueEvent::TypeBuffer :
		privSetValue( ev->getBuffer() );
		break;

	case Msg::ValueEvent::TypeInteger :
		privSetValue( ev->getUint32() != 0 );
		break;

	default :
		throw InvalidArgument();

	}

}

// ============================================================================

void Dyn::VBool::syncObservers() const {

	warnObservers( new Msg::ValueEvent( v ) );

}

// ============================================================================

void Dyn::VBool::privSetValue(
	const	Bool		pValue ) {

	if ( v != pValue ) {
		v = pValue;
		syncObservers();
	}

}

void Dyn::VBool::privSetValue(
	const	Buffer &	pBuffer ) {

	privSetValue( pBuffer.toBool() );

}

// ============================================================================

