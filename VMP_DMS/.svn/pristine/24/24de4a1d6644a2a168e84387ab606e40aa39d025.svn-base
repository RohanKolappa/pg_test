// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ICheckBox.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "VMP.UI.ICheckBox.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ICheckBox )

// ============================================================================

UI::ICheckBox::ICheckBox() {

	addProperty( propTextTag );
	addProperty( propCheckedTag );

	propChecked = false;

	addCallback( this, &ICheckBox::setCheckedCB );

}

UI::ICheckBox::~ICheckBox() {

	delCallbacks();

}

// ============================================================================

void UI::ICheckBox::setText(
	const	Buffer &		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	showPropText();

}

Buffer UI::ICheckBox::getText() const {

	return propText;

}

// ============================================================================

void UI::ICheckBox::setChecked(
	const	Bool		yesno ) {

	if ( propChecked == yesno ) {
		return;
	}

	propChecked = yesno;

	showPropChecked();

	sendEvent();

}

Bool UI::ICheckBox::isChecked() const {

	return propChecked;

}

// ============================================================================

void UI::ICheckBox::setProperty(
	const	Buffer &		propertyName,
	const	Buffer &		propertyValue ) {

	if ( propertyName == propTextTag ) {
		setText( propertyValue );
	}

	else if ( propertyName == propCheckedTag ) {
		setChecked( propertyValue.toBool() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ICheckBox::getProperty(
	const	Buffer &		propertyName ) const {

	if ( propertyName == propTextTag ) {
		return getText();
	}

	if ( propertyName == propCheckedTag ) {
		return Buffer( isChecked() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ICheckBox::setCheckedCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr sev = pEvent;

	switch ( sev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		setChecked( sev->getBool() );
		break;

	case Msg::ValueEvent::TypeBuffer :
		setText( sev->getBuffer().toBool() );
		break;

	default :
		throw InvalidArgument();

	}

}

// ============================================================================

void UI::ICheckBox::cachePropChecked(
	const	Bool		value ) {

	if ( propChecked == value ) {
		return;
	}

	propChecked = value;

	sendEvent();

}

void UI::ICheckBox::sendEvent() const {

	warnObservers( new Msg::ValueEvent( propChecked ) );

}

// ============================================================================

Buffer UI::ICheckBox::propTextTag = "text";
Buffer UI::ICheckBox::propCheckedTag = "checked";

// ============================================================================

