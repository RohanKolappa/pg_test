// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IRadioButtons.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IRadioButtons.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IRadioButtons )

// ============================================================================

UI::IRadioButtons::IRadioButtons() {

	addProperty( propTitleTag );
	addProperty( propCheckedTag );

	propChecked = 0;

	addCallback( this, &IRadioButtons::setCheckedCB );

}

UI::IRadioButtons::~IRadioButtons() {

	delCallbacks();

}

// ============================================================================

void UI::IRadioButtons::setTitle(
	const	Buffer&		title ) {

	if ( propTitle == title ) {
		return;
	}

	propTitle = title;

	showPropTitle();

}

Buffer UI::IRadioButtons::getTitle() const {

	return propTitle;

}

// ============================================================================

void UI::IRadioButtons::setChecked(
	const	Uint32		pos ) {

	if ( propChecked == pos ) {
		return;
	}

	propChecked = pos;

	showPropChecked();

	sendEvent();

}

Uint32 UI::IRadioButtons::getChecked() const {

	return propChecked;

}

Bool UI::IRadioButtons::isChecked(
	const	Uint32		pos ) const {

	return ( propChecked == pos );

}

// ============================================================================

void UI::IRadioButtons::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propTitleTag ) {
		setTitle( propertyValue );
	}

	else if ( propertyName == propCheckedTag ) {
		setChecked( propertyValue.toUint32() );
	}

	else {
		IItemContainer::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::IRadioButtons::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propTitleTag ) {
		return getTitle();
	}

	if ( propertyName == propCheckedTag ) {
		return Buffer( getChecked() );
	}

	return IItemContainer::getProperty( propertyName );

}

// ============================================================================

void UI::IRadioButtons::setCheckedCB(
		Msg::EventCPtr	pEvent ) {

	try {
		setChecked( Msg::ValueEventCPtr( pEvent )->getUint32() );
		return;
	}
	catch ( BadCast & ) {
	}

}

// ============================================================================

void UI::IRadioButtons::cachePropChecked(
	const	Uint32		id ) {

	if ( propChecked == id ) {
		return;
	}

	propChecked = id;

	sendEvent();

}

void UI::IRadioButtons::sendEvent() const {

	warnObservers( new Msg::ValueEvent( propChecked ) );

}

// ============================================================================

Buffer UI::IRadioButtons::propTitleTag = "title";
Buffer UI::IRadioButtons::propCheckedTag = "checked";

// ============================================================================

