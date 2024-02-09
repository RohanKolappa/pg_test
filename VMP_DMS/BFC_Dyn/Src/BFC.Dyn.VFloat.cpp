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
//	BFC.Dyn.VFloat.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "BFC.Dyn.TFloat.h"
#include "BFC.Dyn.VFloat.h"

#include "BFC.IO.ByteInputStream.h"
#include "BFC.IO.ByteOutputStream.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( Dyn, VFloat )

// ============================================================================

Dyn::VFloat::VFloat(
	const	Double		pCurValue,
	const	Double		pMinValue,
	const	Double		pMaxValue ) :

	Var	( TFloat::instance() ),
	curValue( pCurValue ),
	minValue( pMinValue ),
	maxValue( pMaxValue ) {

	addCallback( this, &VFloat::setValueCB );

}

Dyn::VFloat::VFloat(
	const	Buffer &	pVarName,
	const	Double		pCurValue,
	const	Double		pMinValue,
	const	Double		pMaxValue ) :

	Var	( TFloat::instance() ),
	curValue( pCurValue ),
	minValue( pMinValue ),
	maxValue( pMaxValue ) {

	addCallback( this, &VFloat::setValueCB );

	setName( pVarName );

}

Dyn::VFloat::VFloat(
	const	TFloatPtr &	pTFloat ) :

	Var	( pTFloat ),
	curValue( 0.0 ),
	minValue( - DoubleMax ),
	maxValue( DoubleMax ) {

	addCallback( this, &VFloat::setValueCB );

}

Dyn::VFloat::VFloat(
	const	VFloat &	pOther ) :

	SObject		(),
	Cloneable	(),
	Serializable	(),
//	Controllable	(),
	Invokable	(),
	Observable	(),
	Observer	(),
//	IVar		(),
	Var		( pOther ),
	curValue	( pOther.curValue ),
	minValue	( pOther.minValue ),
	maxValue	( pOther.maxValue ) {

	addCallback( this, &VFloat::setValueCB );

}

// ============================================================================

Dyn::VFloat::~VFloat() {

	delCallbacks();

}

// ============================================================================

Dyn::VFloat & Dyn::VFloat::operator = (
	const	VFloat &	pOther ) {

	Var::operator = ( pOther );

	privSetValue( pOther.curValue );

	return *this;

}

// ============================================================================

void Dyn::VFloat::set(
	const	Buffer &	pTag,
	const	Buffer &	pVal ) {

//	msgDbg( "set: " + pTag + " = " + pVal );

	if ( pTag.isEmpty() || pTag == "val" ) {
		setValue( pVal.toDouble() );
	}
	else if ( pTag == "min" ) {
		setMinValue( pVal.toDouble() );
	}
	else if ( pTag == "max" ) {
		setMaxValue( pVal.toDouble() );
	}
	else {
		Var::set( pTag, pVal );
	}

}

Buffer Dyn::VFloat::get(
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

//	msgDbg( "get: " + pTag + " = " + res );

	return res;

}

// ============================================================================

SPtr Dyn::VFloat::clone() const {

	return new VFloat( *this );

}

// ============================================================================

void Dyn::VFloat::doSerialize(
		IO::ByteOutputStreamPtr	pOStream ) const {

	Var::doSerialize( pOStream );

	pOStream->putDouble( curValue );
	pOStream->putDouble( minValue );
	pOStream->putDouble( maxValue );

}

void Dyn::VFloat::unSerialize(
		IO::ByteInputStreamPtr	pIStream ) {

	Var::unSerialize( pIStream );

	curValue = pIStream->getDouble();
	minValue = pIStream->getDouble();
	maxValue = pIStream->getDouble();

}

// ============================================================================

Buffer Dyn::VFloat::toBuffer() const {

	return Buffer( curValue );

}

void Dyn::VFloat::fromBuffer(
	const	Buffer &	pBuffer ) {

	setValue( pBuffer );

}

// ============================================================================

void Dyn::VFloat::setMinValue(
	const	Double		pMinValue ) {

	privSetMinValue( pMinValue );

}

void Dyn::VFloat::setMaxValue(
	const	Double		pMaxValue ) {

	privSetMaxValue( pMaxValue );

}

void Dyn::VFloat::setValue(
	const	Double		pValue ) {

	privSetValue( pValue );

}

void Dyn::VFloat::setValue(
	const	Buffer &	pBuffer ) {

	privSetValue( pBuffer );

}

// ============================================================================

void Dyn::VFloat::syncObservers() const {

	warnObservers( new Msg::BoundedValueEvent( curValue, minValue, maxValue ) );

}

// ============================================================================

void Dyn::VFloat::setValueCB(
		Msg::EventCPtr	pEvent ) {

	if ( pEvent.isA< Msg::BoundedValueEvent >() ) {
		Msg::BoundedValueEventCPtr bev = pEvent;
		if ( bev->getType() == Msg::ValueEvent::TypeInteger ) {
			privSetMinValue( ( Double )bev->getInt64MinValue() );
			privSetMaxValue( ( Double )bev->getInt64MaxValue() );
		}
		else if ( bev->getType() == Msg::ValueEvent::TypeFloating ) {
			privSetMinValue( bev->getDoubleMinValue() );
			privSetMaxValue( bev->getDoubleMaxValue() );
		}
		else {
			throw InternalError();
		}
	}

	Msg::ValueEventCPtr	ev = pEvent;

	switch ( ev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		privSetValue( ev->getBool() ? ( Double )1 : ( Double )0 );
		break;

	case Msg::ValueEvent::TypeBuffer :
		privSetValue( ev->getBuffer() );
		break;

	case Msg::ValueEvent::TypeInteger :
		privSetValue( ( Double )ev->getInt64() );
		break;

	case Msg::ValueEvent::TypeFloating :
		privSetValue( ev->getDouble() );
		break;

	default :
		throw InvalidArgument();

	}

}

// ============================================================================

void Dyn::VFloat::privSetMinValue(
	const	Double		pMinValue ) {

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

void Dyn::VFloat::privSetMaxValue(
	const	Double		pMaxValue ) {

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

void Dyn::VFloat::privSetValue(
	const	Double		pValue ) {

	if ( curValue == pValue ) {
		return;
	}

	if ( pValue < minValue || pValue > maxValue ) {
		throw OutOfRange( "min, val, max: "
			+ Buffer( minValue ) + ", "
			+ Buffer( pValue ) + ", "
			+ Buffer( maxValue ) );
	}

	curValue = pValue;

	syncObservers();

}

void Dyn::VFloat::privSetValue(
	const	Buffer &	pBuffer ) {

	if ( pBuffer.isEmpty() ) {
		throw InvalidArgument( "Dyn::VFloat::setValue(): empty buffer arg!" );
	}

	Uint32	pos = 0;
	Double	res = pBuffer.getDoubleAt( pos );

	if ( pos != pBuffer.getLength() ) {
		throw InvalidArgument( "Dyn::VFloat::fromBuffer(): buffer not an int!" );
	}

	setValue( res );

}

// ============================================================================

