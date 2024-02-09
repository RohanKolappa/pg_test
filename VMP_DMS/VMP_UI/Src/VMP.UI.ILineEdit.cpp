// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ILineEdit.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Msg.ValueEvent.h"

#include "VMP.UI.ILineEdit.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ILineEdit )

// ============================================================================

UI::ILineEdit::ILineEdit() :

	outBase( dfltOutBase ),
	outSize( dfltOutSize ),
	outChar( dfltOutChar ) {

	addProperty( propTextTag );
	addProperty( propAlignTag );

	propAlign = AlignLeft;

	addCallback( this, &ILineEdit::setTextCB, ServSetTextId );

}

UI::ILineEdit::~ILineEdit() {

	delCallbacks();

}

// ============================================================================

void UI::ILineEdit::setText(
	const	Buffer&		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	showPropText();

	sendEvent();

}

void UI::ILineEdit::setValue(
	const	Uint64		pValue ) {

	setText( Buffer( pValue, outBase, outSize, outChar ) );

}

Buffer UI::ILineEdit::getText() const {

	return propText;

}

// ============================================================================

void UI::ILineEdit::setAlignment(
	const	Alignment		value ) {

	if ( propAlign == value ) {
		return;
	}

	propAlign = value;

	showPropAlign();

}

UI::ILineEdit::Alignment UI::ILineEdit::getAlignment() const {

	return propAlign;

}

// ============================================================================

void UI::ILineEdit::setProperty(
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

Buffer UI::ILineEdit::getProperty(
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

void UI::ILineEdit::setTextCB(
		Msg::EventCPtr	pEvent ) {

	Msg::ValueEventCPtr sev = pEvent;

	switch ( sev->getType() ) {

	case Msg::ValueEvent::TypeBoolean :
		setText( Buffer( sev->getBool() ) );
		break;

	case Msg::ValueEvent::TypeInteger :
		if ( outBase == dfltOutBase
		  && outSize == dfltOutSize
		  && outChar == dfltOutChar ) {
			setText( Buffer( sev->getInt64() ) );
		}
		else {
			setValue( sev->getUint64() );
		}
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

void UI::ILineEdit::cachePropText(
	const	Buffer		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	sendEvent();

}

void UI::ILineEdit::sendEvent() const {

	warnObservers( new Msg::ValueEvent( propText ) );

}

// ============================================================================

Buffer UI::ILineEdit::propTextTag = "text";
Buffer UI::ILineEdit::propAlignTag = "alignment";

// ============================================================================

Buffer::Base	UI::ILineEdit::dfltOutBase = Buffer::Base10;
Uint32		UI::ILineEdit::dfltOutSize = 1;
Uchar		UI::ILineEdit::dfltOutChar = '0';

// ============================================================================

