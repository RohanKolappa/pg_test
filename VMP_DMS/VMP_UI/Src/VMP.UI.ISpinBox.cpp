// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ISpinBox.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "VMP.UI.ISpinBox.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ISpinBox )

// ============================================================================

UI::ISpinBox::ISpinBox() {

	addProperty( propMinValueTag );
	addProperty( propMaxValueTag );
	addProperty( propValueTag );

	propMinValue	=   0;
	propMaxValue	= 100;
	propValue	=   0;

	addCallback( this, &ISpinBox::setValueCB );

}

UI::ISpinBox::~ISpinBox() {

	delCallbacks();

}

// ============================================================================

void UI::ISpinBox::setMinValue(
	const	Int32		minValue ) {

//	msgDbg( "setMinValue(): " + Buffer( minValue ) );

	if ( propMinValue == minValue ) {
		return;
	}

	propMinValue = minValue;
	showPropMinValue();

	if ( propMinValue > propMaxValue ) {
		propMaxValue = propMinValue;
		showPropMaxValue();
	}

	if ( propValue < propMinValue ) {
		propValue = propMinValue;
		showPropValue();
	}

	sendEvent();

}

void UI::ISpinBox::setMaxValue(
	const	Int32		maxValue ) {

//	msgDbg( "setMaxValue(): " + Buffer( maxValue ) );

	if ( propMaxValue == maxValue ) {
		return;
	}

	propMaxValue = maxValue;
	showPropMaxValue();

	if ( propMaxValue < propMinValue ) {
		propMinValue = propMaxValue;
		showPropMinValue();
	}

	if ( propValue > propMaxValue ) {
		propValue = propMaxValue;
		showPropValue();
	}

	sendEvent();

}

void UI::ISpinBox::setValue(
	const	Int32		value ) {

//	msgDbg( "setValue(): " + Buffer( value ) );

	if ( propValue == value ) {
		return;
	}

	if ( value < propMinValue || value > propMaxValue ) {
		throw InvalidArgument( "out of range in setValue( "
			+ Buffer( value )
			+ " ): min,max = "
			+ Buffer( propMinValue )
			+ ","
			+ Buffer( propMaxValue ) );
	}

	propValue = value;
	showPropValue();

	sendEvent();

}

// ============================================================================

Int32 UI::ISpinBox::getMinValue() const {

	return propMinValue;

}

Int32 UI::ISpinBox::getMaxValue() const {

	return propMaxValue;

}

Int32 UI::ISpinBox::getValue() const {

	return propValue;

}

// ============================================================================

void UI::ISpinBox::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propMinValueTag ) {
		setMinValue( propertyValue.toInt32() );
	}

	else if ( propertyName == propMaxValueTag ) {
		setMaxValue( propertyValue.toInt32() );
	}

	else if ( propertyName == propValueTag ) {
		setValue( propertyValue.toInt32() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ISpinBox::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propMinValueTag ) {
		return Buffer( getMinValue() );
	}

	if ( propertyName == propMaxValueTag ) {
		return Buffer( getMaxValue() );
	}

	if ( propertyName == propValueTag ) {
		return Buffer( getValue() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ISpinBox::setValueCB(
		Msg::EventCPtr	pEvent ) {

	try {
		Msg::BoundedValueEventCPtr e = pEvent;
		Int64 newMinValue = e->getInt64MinValue();
		Int64 newMaxValue = e->getInt64MaxValue();
//		msgDbg( "setValueCB(): min,max = "
//			+ Buffer( newMinValue )
//			+ ","
//			+ Buffer( newMaxValue ) );
		if ( newMinValue < ( Int64 )Int32Min ) {
//			msgWrn( "setValueCB(): clipping min value..." );
			newMinValue = ( Int64 )Int32Min;
		}
		if ( newMaxValue > ( Int64 )Int32Max ) {
//			msgWrn( "setValueCB(): clipping max value..." );
			newMaxValue = ( Int64 )Int32Max;
		}
		setMinValue( ( Int32 )newMinValue );
		setMaxValue( ( Int32 )newMaxValue );
	}
	catch ( BadCast & ) {
	}

	setValue( Msg::ValueEventCPtr( pEvent )->getInt32() );

}

// ============================================================================

void UI::ISpinBox::cachePropValue(
	const	Int32		value ) {

	if ( propValue == value ) {
		return;
	}

	propValue = value;

	sendEvent();

}

void UI::ISpinBox::sendEvent() const {

	warnObservers( new Msg::BoundedValueEvent(
		( Int64 )propValue,
		( Int64 )propMinValue,
		( Int64 )propMaxValue ) );

}

// ============================================================================

Buffer UI::ISpinBox::propMinValueTag = "minValue";
Buffer UI::ISpinBox::propMaxValueTag = "maxValue";
Buffer UI::ISpinBox::propValueTag = "value";

// ============================================================================

