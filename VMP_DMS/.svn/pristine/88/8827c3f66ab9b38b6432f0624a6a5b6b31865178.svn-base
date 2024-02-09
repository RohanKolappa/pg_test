// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ILCDNumber.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ILCDNumber.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ILCDNumber )

// ============================================================================

UI::ILCDNumber::ILCDNumber() {

	addProperty( propValueTag );

	propValue = 0;

	addCallback( this, &ILCDNumber::setValueCB );

}

UI::ILCDNumber::~ILCDNumber() {

	delCallbacks();

}

// ============================================================================

void UI::ILCDNumber::setValue(
	const	Int32		value ) {

	if ( propValue == value ) {
		return;
	}

	propValue = value;

	showPropValue();

}

Int32 UI::ILCDNumber::getValue() const {

	return propValue;

}

// ============================================================================

void UI::ILCDNumber::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propValueTag ) {
		setValue( propertyValue.toInt32() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ILCDNumber::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propValueTag ) {
		return Buffer( getValue() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ILCDNumber::setValueCB(
		Msg::EventCPtr	pEvent ) {

	try {
		setValue( Msg::ValueEventCPtr( pEvent )->getInt32() );
	}
	catch ( BadCast & ) {
	}

}

// ============================================================================

void UI::ILCDNumber::sendEvent() const {

	warnObservers( new Msg::ValueEvent( getValue() ) );

}

// ============================================================================

Buffer UI::ILCDNumber::propValueTag = "value";

// ============================================================================

