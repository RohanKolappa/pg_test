// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITextLabel.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ITextLabel.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ITextLabel )

// ============================================================================

UI::ITextLabel::ITextLabel() {

	addProperty( propTextTag );
	addProperty( propAlignTag );

	propText = Buffer();
	propAlign = AlignLeft;

	addCallback( this, &ITextLabel::setTextCB, ServSetTextId );

}

UI::ITextLabel::~ITextLabel() {

	delCallbacks();

}

// ============================================================================

void UI::ITextLabel::setText(
	const	Buffer&		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	showPropText();

}

void UI::ITextLabel::setNum(
	const	Uint32		v ) {

	setText( Buffer( v ) );

}

void UI::ITextLabel::setNum(
	const	Int32		v ) {

	setText( Buffer( v ) );

}

void UI::ITextLabel::setNum(
	const	Double		v ) {

	setText( Buffer( v ) );

}

Buffer UI::ITextLabel::getText() const {

	return propText;

}

void UI::ITextLabel::clearText() {

	setText( Buffer() );

}

// ============================================================================

void UI::ITextLabel::setAlignment(
	const	Alignment		value ) {

	if ( propAlign == value ) {
		return;
	}

	propAlign = value;

	showPropAlign();

}

UI::ITextLabel::Alignment UI::ITextLabel::getAlignment() const {

	return propAlign;

}

// ============================================================================

void UI::ITextLabel::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propTextTag ) {
		setText( propertyValue );
	}

	else if ( propertyName == propAlignTag ) {
		setAlignment( alignBufToInt( propertyValue ) );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ITextLabel::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propTextTag ) {
		return getText();
	}

	if ( propertyName == propAlignTag ) {
		return alignIntToBuf( getAlignment() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ITextLabel::setTextCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr sev = pEvent;

	switch ( sev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		setText( Buffer( sev->getBool() ) );
		break;

	case Msg::ValueEvent::TypeInteger :
		setText( Buffer( sev->getInt64() ) );
		break;

	case Msg::ValueEvent::TypeFloating :
		setText( Buffer( sev->getDouble() ) );
		break;

	case Msg::ValueEvent::TypeBuffer :
		setText( sev->getBuffer() );
		break;

	}

}

// ============================================================================

void UI::ITextLabel::sendEvent() const {

}

// ============================================================================

Buffer UI::ITextLabel::propTextTag = "text";
Buffer UI::ITextLabel::propAlignTag = "alignment";

// ============================================================================

