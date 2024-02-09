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
//	BFC.Dyn.VInt.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Dyn.TInt.h"
#include "BFC.Dyn.VInt.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

#include "BFC.Msg.ValueEvent.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VInt )

// ============================================================================

Dyn::VInt::VInt(
	const	Int64		pCurValue,
	const	Int64		pMinValue,
	const	Int64		pMaxValue ) :

	Var	( TInt::instance() ),
	curValue( pCurValue ),
	minValue( pMinValue ),
	maxValue( pMaxValue ),
	convBase( Buffer::Base10 ),
	convSize( 1 ),
	convChar( '0' ) {

	addCallback( this, &VInt::setValueCB );

}

Dyn::VInt::VInt(
	const	Buffer &	pVarName,
	const	Int64		pCurValue,
	const	Int64		pMinValue,
	const	Int64		pMaxValue ) :

	Var	( TInt::instance() ),
	curValue( pCurValue ),
	minValue( pMinValue ),
	maxValue( pMaxValue ),
	convBase( Buffer::Base10 ),
	convSize( 1 ),
	convChar( '0' ) {

	addCallback( this, &VInt::setValueCB );

	setName( pVarName );

}

Dyn::VInt::VInt(
	const	TIntPtr &	pTInt ) :

	SObject	(),
	Var	( pTInt ),
	minValue( Int64Min ),
	maxValue( Int64Max ),
	convBase( Buffer::Base10 ),
	convSize( 1 ),
	convChar( '0' ) {

	curValue = ( 0 < minValue ? minValue : maxValue < 0 ? maxValue : 0 );

	addCallback( this, &VInt::setValueCB );

}

Dyn::VInt::VInt(
	const	VInt &		pOther ) :

	SObject	(),
	Cloneable(),
	Serializable(),
//	Controllable(),
	Invokable(),
	Observable(),
	Observer(),
//	IVar(),
	Var	(         pOther ),
	curValue( pOther.curValue ),
	minValue( pOther.minValue ),
	maxValue( pOther.maxValue ),
	convBase(  Buffer::Base10 ),
	convSize(               1 ),
	convChar(             '0' ) {

	addCallback( this, &VInt::setValueCB );

}

// ============================================================================

Dyn::VInt::~VInt() {

	delCallbacks();

}

// ============================================================================

void Dyn::VInt::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

	if ( pTag.isEmpty() || pTag == "val" ) {
		setValue( pVal.toInt64() );
	}
	else if ( pTag == "min" ) {
		setMinValue( pVal.toInt64() );
	}
	else if ( pTag == "max" ) {
		setMaxValue( pVal.toInt64() );
	}
	else {
		Var::set( pTag, pVal );
	}

}

Buffer Dyn::VInt::get(
	const	Buffer &	pTag ) const {

	Buffer res;

	if ( pTag.isEmpty() || pTag == "val" ) {
		res = Buffer( getValue() );
	}
	else if ( pTag == "min" ) {
		res = Buffer( getMinValue() );
	}
	else if ( pTag == "max" ) {
		res = Buffer( getMaxValue() );
	}
	else {
		res = Var::get( pTag );
	}

	return res;

}

// ============================================================================

SPtr Dyn::VInt::clone() const {

	return new VInt( *this );

}

// ============================================================================

void Dyn::VInt::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putLEInt64( curValue );
	pOStream->putLEInt64( minValue );
	pOStream->putLEInt64( maxValue );
	pOStream->putUchar( ( Uchar )convBase );
	pOStream->putLEUint16( convSize );
	pOStream->putUchar( convChar );

}

void Dyn::VInt::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	curValue = pIStream->getLEInt64();
	minValue = pIStream->getLEInt64();
	maxValue = pIStream->getLEInt64();
	convBase = ( Buffer::Base )pIStream->getUchar();
	convSize = pIStream->getLEUint16();
	convChar = pIStream->getUchar();

}

// ============================================================================

Buffer Dyn::VInt::toBuffer() const {

	return	( curValue >= 0
		? Buffer( ( Uint64 ) curValue, convBase, convSize, convChar )
		: "-" + Buffer( ( Uint64 )-curValue, convBase, convSize, convChar ) );

}

void Dyn::VInt::fromBuffer(
	const	Buffer &	pBuffer ) {

	setValue( pBuffer );

}

// ============================================================================

void Dyn::VInt::setMinValue(
	const	Int64		pMinValue ) {

	privSetMinValue( pMinValue );

}

void Dyn::VInt::setMaxValue(
	const	Int64		pMaxValue ) {

	privSetMaxValue( pMaxValue );

}

void Dyn::VInt::setValue(
	const	Int64		pValue ) {

	privSetValue( pValue );

}

void Dyn::VInt::setValue(
	const	Buffer &	pBuffer ) {

	privSetValue( pBuffer );

}

void Dyn::VInt::decValue() {

	privSetValue( curValue - 1 );

}

void Dyn::VInt::incValue() {

	privSetValue( curValue + 1 );

}

// ============================================================================

void Dyn::VInt::syncObservers() const {

//	msgDbg( "syncObservers()" );

	warnObservers( new Msg::BoundedValueEvent( curValue, minValue, maxValue ) );

}

// ============================================================================

void Dyn::VInt::setValueCB(
		Msg::EventCPtr	pEvent ) {

	try {
		Msg::BoundedValueEventCPtr bev = pEvent;
		if ( bev->getType() == Msg::ValueEvent::TypeInteger ) {
			privSetMinValue( bev->getInt64MinValue() );
			privSetMaxValue( bev->getInt64MaxValue() );
		}
		else {
			throw InternalError();
		}
	}
	catch ( BadCast & ) {
	}

	Msg::ValueEventCPtr	ev = pEvent;

	switch ( ev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		privSetValue( ev->getBool() ? ( Int64 )1 : ( Int64 )0 );
		break;

	case Msg::ValueEvent::TypeBuffer :
		privSetValue( ev->getBuffer() );
		break;

	case Msg::ValueEvent::TypeInteger :
		privSetValue( ev->getInt64() );
		break;

	default :
		throw InvalidArgument();

	}

}

// ============================================================================

void Dyn::VInt::privSetMinValue(
	const	Int64		pMinValue ) {

	if ( pMinValue == minValue ) {
		return;
	}

	minValue = pMinValue;

	if ( minValue > maxValue ) {
		maxValue = minValue;
	}

	if ( curValue < minValue ) {
		curValue = minValue;
	}

	syncObservers();

}

void Dyn::VInt::privSetMaxValue(
	const	Int64		pMaxValue ) {

	if ( pMaxValue == maxValue ) {
		return;
	}

	maxValue = pMaxValue;

	if ( maxValue < minValue ) {
		minValue = maxValue;
	}

	if ( curValue > maxValue ) {
		curValue = maxValue;
	}

	syncObservers();

}

void Dyn::VInt::privSetValue(
	const	Int64		pValue ) {

	if ( curValue == pValue ) {
		return;
	}

	if ( pValue < minValue || pValue > maxValue ) {
		throw OutOfRange();
	}

	curValue = pValue;

	syncObservers();

}

void Dyn::VInt::privSetValue(
	const	Buffer &	pBuffer ) {

	if ( pBuffer.isEmpty() ) {
		throw InvalidArgument( "Dyn::VInt::setValue(): empty buffer arg!" );
	}

	Uint32	pos = 0;
	Int64	res = pBuffer.getIntAt( pos );

	if ( pos != pBuffer.getLength() ) {
		throw InvalidArgument( "Dyn::VInt::fromBuffer(): buffer not an int!" );
	}

	setValue( res );

}

// ============================================================================

