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
//	BFC.Dyn.VStruct.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.Event.h"
#include "BFC.Dyn.VStruct.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.ChildEvent.h"
#include "BFC.Msg.ResizeEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VStruct )

// ============================================================================

Dyn::VStruct::VStruct(
		TStructPtr	pTStruct ) :

	Var	( pTStruct ),
	t	( pTStruct ) {

	addCallback( this, &VStruct::eventCB, EventCBId );
	addCallback( this, &VStruct::childCB, ChildCBId );

	privCreate();

}

Dyn::VStruct::VStruct(
	const	VStruct &	pOther ) :

	SObject		(),
	Cloneable	(),
	Serializable	(),
	Invokable	(),
	Observable	(),
	Observer	(),
	Var		( pOther ),
	t		( pOther.t ) {

	addCallback( this, &VStruct::eventCB, EventCBId );
	addCallback( this, &VStruct::childCB, ChildCBId );

	privCopy( pOther );

}

Dyn::VStruct::~VStruct() {

	privDelete();

	delCallbacks();

}

// ============================================================================

Dyn::VStruct & Dyn::VStruct::operator = (
	const	VStruct &	pOther ) {

	if ( this != &pOther ) {
		Var::operator = ( pOther );
		privCopy( pOther );
		syncObservers();
	}

	return *this;

}

// ============================================================================

Dyn::VarPtr Dyn::VStruct::sub(
	const	Buffer &	pTag ) {

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
		throw InternalError();
	}

	if ( ! v.contains( t1 ) ) {
		throw InvalidArgument( "sub(): No such struct member (\""
			+ pTag
			+ "\")!" );
	}

	Dyn::VarPtr res = v[ t1 ];

	if ( t2.hasContent() ) {
		res = res->sub( t2 );
	}

	return res;

}

void Dyn::VStruct::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

//	msgDbg( "set(): \"" + pTag + "\" = \"" + pVal + "\"" );

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
//		throw NotImplemented( "set(): compact notation not supported!" );
		BufferArray subs;
		splitCompact( pVal, subs );
		if ( subs.getSize() != v.getSize() ) {
			throw InvalidArgument( "set(): compact: size mismatch!" );
		}
		for ( Uint32 i = 0 ; i < subs.getSize() ; i++ ) {
			v.getValue( i )->set( Buffer(), subs[ i ] );
		}
		return;
	}

	if ( v.contains( t1 ) ) {
		v[ t1 ]->set( t2, pVal );
	}
	else {
		Var::set( pTag, pVal );
	}

}

Buffer Dyn::VStruct::get(
	const	Buffer &	pTag ) const {

	Buffer	t1, t2;

	splitTag( pTag, t1, t2 );

	if ( t1.isEmpty() ) {
		throw NotImplemented( "get(): compact notation not supported!" );
	}

	Buffer	res;

	if ( v.contains( t1 ) ) {
		res = v[ t1 ]->get( t2 );
	}
	else {
		res = Var::get( pTag );
	}

	return res;

}

// ============================================================================

SPtr Dyn::VStruct::clone() const {

	return new VStruct( *this );

}

// ============================================================================

void Dyn::VStruct::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		pOStream->putLEUint32( v.getKey( i ).getLength() );
		pOStream->putBytes( v.getKey( i ) );
		v.getValue( i )->doSerialize( pOStream );
	}

}

void Dyn::VStruct::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		Uint32 l = pIStream->getLEUint32();
		Buffer n = pIStream->getBytes( l );
		v[ n ]->unSerialize( pIStream );
	}

}

// ============================================================================

Buffer Dyn::VStruct::toBuffer() const {

	Buffer res = "( ";

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		if ( i ) {
			res += " ; ";
		}
		res += v.getKey( i );
		res += " : ";
		res += v.getValue( i )->toBuffer();
	}

	res += " )";

	return res;

}

void Dyn::VStruct::fromBuffer(
	const	Buffer &	/*pBuffer*/ ) {

	throw NotImplemented();

}

// ============================================================================

void Dyn::VStruct::syncObservers() const {

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v.getValue( i )->syncObservers();
	}

}

// ============================================================================

void Dyn::VStruct::eventCB(
		Msg::EventCPtr	pEvent ) {

	if ( pEvent.isA< Msg::ChildEvent >() ) {
		Msg::ChildEventCPtr e = pEvent;
		v[ e->getChild() ]->getCallback()->call( e->getEvent() );
	}
	else {
		throw InternalError( "Unsupported event!" );
	}

}

void Dyn::VStruct::childCB(
		Msg::EventCPtr	pEvent ) {

	Buffer	pos = v.keyOf( pEvent->getSender() );

	warnObservers(
		new Msg::ChildEvent(
			pos,
			pEvent ) );

}

// ============================================================================

void Dyn::VStruct::privCreate() {

	for ( Uint32 i = 0 ; i < t->getNbrMembers() ; i++ ) {
		v.add( t->getMemberName( i ),
			t->getMemberType( i )->create() );
	}

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v.getValue( i )->addObserver( this, ChildCBId );
	}

}

void Dyn::VStruct::privDelete() {

	for ( Uint32 i = 0 ; i < v.getSize() ; i++ ) {
		v.getValue( i )->delObserver( this );
	}

	v.kill();

}

void Dyn::VStruct::privCopy(
	const	VStruct &	pOther ) {

	for ( Uint32 i = 0 ; i < pOther.v.getSize() ; i++ ) {
		Buffer pos = pOther.v.getKey( i );
		v[ pos ]->delObserver( this );
		v[ pos ].kill();
		v[ pos ] = pOther.v.getValue( i )->clone();
		v[ pos ]->addObserver( this, ChildCBId );
	}

}

// ============================================================================

