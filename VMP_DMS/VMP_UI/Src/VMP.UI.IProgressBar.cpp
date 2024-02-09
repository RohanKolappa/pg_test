// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IProgressBar.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "VMP.UI.IProgressBar.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IProgressBar )

// ============================================================================

UI::IProgressBar::IProgressBar() {

	addProperty( propStepsTag );
	addProperty( propProgressTag );

	propSteps	= 100;
	propProgress	=   0;

	addCallback( this, &IProgressBar::setValueCB );

}

UI::IProgressBar::~IProgressBar() {

	delCallbacks();

}

// ============================================================================

void UI::IProgressBar::setSteps(
	const	Uint32		steps ) {

	if ( propSteps == steps ) {
		return;
	}

	propSteps = steps;
	showPropSteps();

	if ( propProgress > propSteps ) {
		propProgress = propSteps;
		showPropProgress();
	}

	sendEvent();

}

Uint32 UI::IProgressBar::getSteps() const {

	return propSteps;

}

// ============================================================================

void UI::IProgressBar::setProgress(
	const	Uint32		progress ) {

	if ( propProgress == progress ) {
		return;
	}

	if ( progress > propSteps ) {
		throw InvalidArgument( "out of range in setProgress()" );
	}

	propProgress = progress;
	showPropProgress();

	sendEvent();

}

Uint32 UI::IProgressBar::getProgress() const {

	return propProgress;

}

// ============================================================================

void UI::IProgressBar::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propStepsTag ) {
		setSteps( propertyValue.toUint32() );
	}

	else if ( propertyName == propProgressTag ) {
		setProgress( propertyValue.toUint32() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::IProgressBar::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propStepsTag ) {
		return getSteps();
	}

	if ( propertyName == propProgressTag ) {
		return getProgress();
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::IProgressBar::setValueCB(
		Msg::EventCPtr	pEvent ) {

// FIXME: check min value too! bounce back if min is negative!
// FIXME: a progress bar should work like a spinbox, with a min and a max...

	Msg::ValueEventCPtr	ve = pEvent;

	try {
		Msg::BoundedValueEventCPtr e = ve;
		if ( e->getType() != Msg::ValueEvent::TypeInteger ) {
			throw InternalError();
		}
		Int64 newSteps = e->getInt64MaxValue();
		if ( newSteps > ( Int64 )( Uint64 )Uint32Max ) {
			newSteps = ( Int64 )( Uint64 )Uint32Max;
		}
		setSteps( ( Int32 )newSteps );
	}
	catch ( BadCast & ) {
	}

	setProgress( ve->getUint32() );

}

// ============================================================================

void UI::IProgressBar::sendEvent() const {

	warnObservers( new Msg::BoundedValueEvent(
		( Int64 )propProgress,
		( Int64 )0,
		( Int64 )propSteps ) );

}

// ============================================================================

Buffer UI::IProgressBar::propStepsTag = "steps";
Buffer UI::IProgressBar::propProgressTag = "progress";

// ============================================================================

