// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IPushButton.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.FastCSLocker.h"

#include "VMP.UI.IPushButton.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IPushButton )

// ============================================================================

UI::IPushButton::IPushButton() {

	addProperty( propTextTag );
	addProperty( propClickedTag );

	propClickedSending = false;

	addCallback( this, &IPushButton::setClickedCB, ServSetClickedId );

}

UI::IPushButton::~IPushButton() {

	delCallbacks();

}

// ============================================================================

void UI::IPushButton::setText(
	const	Buffer&		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	showPropText();

}

Buffer UI::IPushButton::getText() const {

	return propText;

}

void UI::IPushButton::setClicked() {

	sendEvent();

}

// ============================================================================

void UI::IPushButton::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propTextTag ) {
		setText( propertyValue );
	}
	else if ( propertyName == propClickedTag ) {
		setClicked();
	}
	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::IPushButton::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propTextTag ) {
		return getText();
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::IPushButton::syncObservers() const {

}

// ============================================================================

void UI::IPushButton::setClickedCB(
		Msg::EventCPtr	/* pEvent */ ) {

	setClicked();

}

// ============================================================================

void UI::IPushButton::cachePropClicked() {

	sendEvent();

}

void UI::IPushButton::sendEvent() const {

	{
		FastCSLocker locker( propClickedCritSec );
		if ( propClickedSending ) {
			return;
		}
		propClickedSending = true;
	}

	warnObservers( Msg::EventPtr() );

	{
		FastCSLocker locker( propClickedCritSec );
		propClickedSending = false;
	}

}

// ============================================================================

Buffer UI::IPushButton::propTextTag = "text";
Buffer UI::IPushButton::propClickedTag = "clicked";

// ============================================================================

