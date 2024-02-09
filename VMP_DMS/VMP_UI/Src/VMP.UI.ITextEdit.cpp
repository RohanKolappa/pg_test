// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITextEdit.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ITextEdit.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ITextEdit )

// ============================================================================

UI::ITextEdit::ITextEdit() {

	addProperty( propTextTag );
	addProperty( propReadOnlyTag );

	propReadOnly = false;

}

// ============================================================================

void UI::ITextEdit::setText(
	const	Buffer&		text ) {

	if ( propText == text ) {
		return;
	}

	propText = text;

	showPropText();

}

void UI::ITextEdit::addText(
	const	Buffer&		text ) {

	if ( text.isEmpty() ) {
		return;
	}

	propText += text;

	showPropText();

}

Buffer UI::ITextEdit::getText() const {

	return propText;

}

void UI::ITextEdit::clearText() {

	setText( Buffer() );

}

// ============================================================================

void UI::ITextEdit::setReadOnly(
	const	Bool		yesno ) {

	if ( propReadOnly == yesno ) {
		return;
	}

	propReadOnly = yesno;

	showPropReadOnly();

}

Bool UI::ITextEdit::isReadOnly() const {

	return propReadOnly;

}

// ============================================================================

void UI::ITextEdit::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propTextTag ) {
		setText( propertyValue );
	}

	else if ( propertyName == propReadOnlyTag ) {
		setReadOnly( propertyValue.toBool() );
	}

	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ITextEdit::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propTextTag ) {
		return getText();
	}

	if ( propertyName == propReadOnlyTag ) {
		return Buffer( isReadOnly() );
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ITextEdit::sendEvent() const {

	warnObservers( new Msg::ValueEvent( propText ) );

}

// ============================================================================

Buffer UI::ITextEdit::propTextTag = "text";
Buffer UI::ITextEdit::propReadOnlyTag = "readOnly";

// ============================================================================

