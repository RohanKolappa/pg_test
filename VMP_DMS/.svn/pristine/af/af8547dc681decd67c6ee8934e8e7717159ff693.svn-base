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
//	BFC.Dyn.VArray.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.VArray.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.ChildEvent.h"
#include "BFC.Msg.ResizeEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VArray )

// ============================================================================

Dyn::VArray::VArray(
		TArrayPtr	pTArray ) :

	Var	( pTArray ),
	s	( pTArray->getSubType() ),
	r	( false ) {

	addCallback( this, &VArray::eventCB, EventCBId );
	addCallback( this, &VArray::childCB, ChildCBId );

}

Dyn::VArray::VArray(
	const	VArray &	pOther ) :

	SObject		(),
	Cloneable	(),
	IO::Serializable(),
//	Controllable	(),
	Msg::Invokable	(),
	Msg::Observable	(),
	Msg::Observer	(),
//	IVar		(),
	Var		( pOther ),
	s		( pOther.s ),
	r		( false ) {

	addCallback( this, &VArray::eventCB, EventCBId );
	addCallback( this, &VArray::childCB, ChildCBId );

	privCopy( pOther );

}

Dyn::VArray::~VArray() {

	delCallbacks();

}

// ============================================================================

Dyn::VArray & Dyn::VArray::operator = (
	const	VArray &	pOther ) {

	if ( this != &pOther ) {
		Var::operator = ( pOther );
		privCopy( pOther );
		syncObservers();
	}

	return *this;

}

// ============================================================================

Dyn::VarPtr Dyn::VArray::sub(
	const	Buffer &	pTag ) {

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
		throw InvalidArgument( "sub(): empty selector!" );
	}

	Uint32 i;

	if ( t1 == "new" ) {
		i = getSize();
		setSize( getSize() + 1 );
	}
	else if ( t1 == "last" ) {
		i = getSize() - 1;
	}
	else if ( t1[ 0 ] >= '0' && t1[ 0 ] <= '9' ) {
		i = t1.toUint32();
	}
	else {
		throw InvalidArgument( "sub(): No such array member (\""
			+ pTag
			+ "\")!" );
	}

	Dyn::VarPtr res = v[ i ];

	if ( t2.hasContent() ) {
		res = res->sub( t2 );
	}

	return res;

}

void Dyn::VArray::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
		throw InvalidArgument( "set(): empty selector!" );
	}

	if ( t1 == "size" ) {
		if ( t2.hasContent() ) {
			throw InvalidArgument( "set(): extra selector for size!" );
		}
		setSize( pVal.toUint32() );
	}
	else if ( t1 == "new" ) {
		setSize( getSize() + 1 );
		v[ getSize() - 1 ]->set( t2, pVal );
	}
	else if ( t1 == "last" ) {
		v[ getSize() - 1 ]->set( t2, pVal );
	}
	else if ( t1[ 0 ] >= '0' && t1[ 0 ] <= '9' ) {
		v[ t1.toUint32() ]->set( t2, pVal );
	}
	else {
		Var::set( pTag, pVal );
	}


}

Buffer Dyn::VArray::get(
	const	Buffer &	pTag ) const {

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
		throw NotImplemented( "get(): compact notation not supported!" );
	}

	Buffer res;

	if ( t1 == "size" ) {
		res = Buffer( getSize() );
	}
	else if ( t1 == "last" ) {
		res = v[ getSize() - 1 ]->get( t2 );
	}
	else if ( t1[ 0 ] >= '0' && t1[ 0 ] <= '9' ) {
		res = v[ t1.toUint32() ]->get( t2 );
	}
	else {
		res = Var::get( pTag );
	}

	return res;

}

// ============================================================================

SPtr Dyn::VArray::clone() const {

	return new VArray( *this );

}

// ============================================================================

void Dyn::VArray::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putLEUint32( v.getSize() );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v[ i ]->doSerialize( pOStream );
	}

}

void Dyn::VArray::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	Uint32 l = pIStream->getLEUint32();

	privResize( l );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v[ i ]->unSerialize( pIStream );
	}

	syncObservers();

}

// ============================================================================

Buffer Dyn::VArray::toBuffer() const {

	Buffer res = "( ";

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		if ( i ) {
			res += " ; ";
		}
		res += v[ i ]->toBuffer();
	}

	res += " )";

	return res;

}

void Dyn::VArray::fromBuffer(
	const	Buffer &	/* pBuffer */ ) {

	throw NotImplemented();

}

// ============================================================================

Uint32 Dyn::VArray::getSize() const {

	return v.getSize();

}

void Dyn::VArray::setSize(
	const	Uint32		pSize ) {

	r = true;
	privResize( pSize );
	r = false;

	syncObservers();

}

Dyn::VarPtr Dyn::VArray::getItem(
	const	Uint32		pIndex ) {

	return v[ pIndex ];

}

Dyn::VarCPtr Dyn::VArray::getItem(
	const	Uint32		pIndex ) const {

	return v[ pIndex ];

}

// ============================================================================

void Dyn::VArray::syncObservers() const {

	warnObservers( new Msg::ResizeEvent( v.getSize() ) );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v[ i ]->syncObservers();
	}

}

// ============================================================================

void Dyn::VArray::eventCB(
		Msg::EventCPtr	pEvent ) {

	if ( pEvent.isA< Msg::ResizeEvent >() ) {
		Msg::ResizeEventCPtr e = pEvent;
//		privResize( e->getNewSize() );
		setSize( e->getNewSize() );
	}
	else if ( pEvent.isA< Msg::ChildEvent >() ) {
		if ( ! r ) {
			Msg::ChildEventCPtr e = pEvent;
			v[ e->getChild().toUint32() ]->getCallback()->call( e->getEvent() );
		}
	}
	else {
		throw InternalError( "Unsupported event!" );
	}

}

void Dyn::VArray::childCB(
		Msg::EventCPtr	pEvent ) {

	Uint32 pos;

	if ( ! v.contains( pEvent->getSender(), pos ) ) {
		throw InternalError();
	}

	if ( ! r ) {
		warnObservers(
			new Msg::ChildEvent(
				Buffer( pos ),
				pEvent ) );
	}

}

// ============================================================================

void Dyn::VArray::privCopy(
	const	VArray &	pOther ) {

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v[ i ]->delObserver( this );
		v[ i ].kill();
	}

	v.resize( pOther.v.getSize() );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v[ i ] = pOther.v[ i ]->clone();
		v[ i ]->addObserver( this, ChildCBId );
	}

}

void Dyn::VArray::privResize(
	const	Uint32		pNewSize ) {

	while ( pNewSize < v.getSize() ) {
		v.getLast()->delObserver( this );
		v.popLast();
	}

	while ( pNewSize > v.getSize() ) {
		v.addLast( s->create() );
		v.getLast()->addObserver( this, ChildCBId );
	}

}

// ============================================================================

