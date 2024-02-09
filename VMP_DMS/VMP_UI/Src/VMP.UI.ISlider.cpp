// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ISlider.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ISlider.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ISlider )

// ============================================================================

UI::ISlider::ISlider() {

	addProperty( propMinValueTag );
	addProperty( propMaxValueTag );
	addProperty( propValueTag );
	addProperty( propLineStepTag );
	addProperty( propPageStepTag );
	addProperty( propTickMarksTag );
	addProperty( propTickIntervalTag );

	propMinValue		= 0;
	propMaxValue		= 100;
	propValue		= 0;
	propLineStep		= 1;
	propPageStep		= 16;
	propTickMarks		= NoMarks;
	propTickInterval	= 16;

	addCallback( this, &ISlider::setMinValueCB, ServSetMinValueId );
	addCallback( this, &ISlider::setMaxValueCB, ServSetMaxValueId );
	addCallback( this, &ISlider::setCurValueCB, ServSetCurValueId );

}

UI::ISlider::~ISlider() {

	delCallbacks();

}

// ============================================================================

void UI::ISlider::setMinValue(
	const	Int32		minValue ) {

	if ( propMinValue == minValue ) {
		return;
	}

	if ( minValue > propMaxValue ) {
		throw InvalidArgument( "min > max in setMinValue()" );
	}

	propMinValue = minValue;

	showPropMinValue();

	if ( propValue < propMinValue ) {
		propValue = propMinValue;
		showPropValue();
		sendEvent();
	}

}

void UI::ISlider::setMaxValue(
	const	Int32		maxValue ) {

	if ( propMaxValue == maxValue ) {
		return;
	}

	if ( maxValue < propMinValue ) {
		throw InvalidArgument( "min > max in setMaxValue()" );
	}

	propMaxValue = maxValue;

	showPropMaxValue();

	if ( propValue > propMaxValue ) {
		propValue = propMaxValue;
		showPropValue();
		sendEvent();
	}

}

void UI::ISlider::setValue(
	const	Int32		value ) {

	if ( propValue == value ) {
		return;
	}

	if ( value < propMinValue || value > propMaxValue ) {
		throw InvalidArgument( "out of range in setValue()" );
	}

	propValue = value;

	showPropValue();

	sendEvent();

}

// ============================================================================

Int32 UI::ISlider::getMinValue() const {

	return propMinValue;

}

Int32 UI::ISlider::getMaxValue() const {

	return propMaxValue;

}

Int32 UI::ISlider::getValue() const {

	return propValue;

}

// ============================================================================

void UI::ISlider::setLineStep(
	const	Uint32		lineStep ) {

	if ( propLineStep == lineStep ) {
		return;
	}

	propLineStep = lineStep;

	showPropLineStep();

}

void UI::ISlider::setPageStep(
	const	Uint32		pageStep ) {

	if ( propPageStep == pageStep ) {
		return;
	}

	propPageStep = pageStep;

	showPropPageStep();

}

// ============================================================================

Uint32 UI::ISlider::getLineStep() const {

	return propLineStep;

}

Uint32 UI::ISlider::getPageStep() const {

	return propPageStep;

}

// ============================================================================

void UI::ISlider::setTickMarks(
	const	TickSetting		setting ) {

	if ( propTickMarks == setting ) {
		return;
	}

	propTickMarks = setting;

	showPropTickMarks();

}

void UI::ISlider::setTickInterval(
	const	Uint32		interval ) {

	if ( propTickInterval == interval ) {
		return;
	}

	propTickInterval = interval;

	showPropTickInterval();

}

// ============================================================================

UI::ISlider::TickSetting UI::ISlider::getTickMarks() const {

	return propTickMarks;

}

Uint32 UI::ISlider::getTickInterval() const {

	return propTickInterval;

}

// ============================================================================

void UI::ISlider::setProperty(
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

	else if ( propertyName == propLineStepTag ) {
		setLineStep( propertyValue.toUint32() );
	}

	else if ( propertyName == propPageStepTag ) {
		setPageStep( propertyValue.toUint32() );
	}

	else if ( propertyName == propTickMarksTag ) {
		setTickMarks( convertSetting( propertyValue ) );
	}

	else if ( propertyName == propTickIntervalTag ) {
		setTickInterval( propertyValue.toUint32() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ISlider::getProperty(
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

	if ( propertyName == propLineStepTag ) {
		return Buffer( getLineStep() );
	}

	if ( propertyName == propPageStepTag ) {
		return Buffer( getPageStep() );
	}

	if ( propertyName == propTickMarksTag ) {
		return Buffer( convertSetting( getTickMarks() ) );
	}

	if ( propertyName == propTickIntervalTag ) {
		return Buffer( getTickInterval() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ISlider::setMinValueCB(
		Msg::EventCPtr	pEvent ) {

	setMinValue( Msg::ValueEventCPtr( pEvent )->getInt32() );

}

void UI::ISlider::setMaxValueCB(
		Msg::EventCPtr	pEvent ) {

	setMaxValue( Msg::ValueEventCPtr( pEvent )->getInt32() );

}

void UI::ISlider::setCurValueCB(
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

static struct {
	UI::ISlider::TickSetting	val;
	const char			*tag;
} convTbl[] = {
	{ UI::ISlider::NoMarks,	"NoMarks" },
	{ UI::ISlider::Both,	"Both" },
	{ UI::ISlider::Above,	"Above" },
	{ UI::ISlider::Below,	"Below" },
	{ UI::ISlider::Left,	"Left" },
	{ UI::ISlider::Right,	"Right" },
	{ UI::ISlider::NoMarks,	0 }
};

// ============================================================================

UI::ISlider::TickSetting UI::ISlider::convertSetting(
	const	Buffer&		buffer ) const {

	for ( Uint32 i = 0 ; convTbl[ i ].tag ; i++ ) {
		if ( buffer == Buffer( convTbl[ i ].tag ) ) {
			return convTbl[ i ].val;
		}
	}

	msgWrn( "Invalid [TickSetting] property: \"" + buffer + "\"!" );

	return NoMarks;

}

Buffer UI::ISlider::convertSetting(
	const	TickSetting		setting ) const {

	return Buffer( convTbl[ setting ].tag  );

}

// ============================================================================

void UI::ISlider::cachePropValue(
	const	Int32		value ) {

	if ( propValue == value ) {
		return;
	}

	propValue = value;

	sendEvent();

}

void UI::ISlider::sendEvent() const {

	warnObservers( new Msg::ValueEvent( propValue ) );

}

// ============================================================================

Buffer UI::ISlider::propMinValueTag = "minValue";
Buffer UI::ISlider::propMaxValueTag = "maxValue";
Buffer UI::ISlider::propValueTag = "value";
Buffer UI::ISlider::propLineStepTag = "lineStep";
Buffer UI::ISlider::propPageStepTag = "pageStep";
Buffer UI::ISlider::propTickMarksTag = "tickmarks";
Buffer UI::ISlider::propTickIntervalTag = "tickInterval";

// ============================================================================

