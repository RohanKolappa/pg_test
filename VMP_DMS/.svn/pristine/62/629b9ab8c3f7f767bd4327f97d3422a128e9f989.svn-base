// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IComboBox.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Dyn.Event.h"
#include "BFC.Msg.ValueEvent.h"

#include "VMP.UI.IComboBox.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IComboBox )

// ============================================================================

UI::IComboBox::IComboBox() {

	addProperty( propSelectedTag );

	propSelected = 0;

	addCallback( this, &IComboBox::dynEventCB );

}

UI::IComboBox::~IComboBox() {

	delCallbacks();

}

// ============================================================================

Uint32 UI::IComboBox::size() const {

	return textCache.size();

}

Uint32 UI::IComboBox::add(
	const	Buffer&		text ) {

	textCache += text;

	showItemsAdd( text );

	return textCache.size() - 1;

}

void UI::IComboBox::clear() {

	textCache.kill();

	showItemsClear();

}

Buffer UI::IComboBox::getText(
	const	Uint32		pos ) const {

	return textCache[ pos ];

}

// ============================================================================

void UI::IComboBox::setSelected(
	const	Uint32		pos ) {

	if ( propSelected == pos ) {
		return;
	}

	propSelected = pos;

	showPropSelected();

	sendEvent();

}

void UI::IComboBox::setSelected(
	const	Buffer&		item ) {

	for ( Uint32 i = 0 ; i < textCache.size() ; i++ ) {
		if ( textCache[i] == item ) {
			setSelected( i );
			return;
		}
	}

}

Uint32 UI::IComboBox::getSelected() const {

	return propSelected;

}

Bool UI::IComboBox::isSelected(
	const	Uint32		pos ) const {

	return ( propSelected == pos );

}

// ============================================================================

void UI::IComboBox::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propSelectedTag ) {
		setSelected( propertyValue.toUint32() );
	}

	else {
		IItemContainer::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::IComboBox::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propSelectedTag ) {
		return Buffer( getSelected() );
	}

	return IItemContainer::getProperty( propertyName );

}

// ============================================================================

void UI::IComboBox::dynEventCB(
		Msg::EventCPtr	pEvent ) {

//	msgDbg( "dynEventCB(): event received... (type:"
//		+ Buffer( pEvent.typeName() ) + ")" );

	try {
		Msg::ValueEventCPtr sev = pEvent;
//		msgDbg( "dynEventCB(): IsA ValueChanged :-)" );
		switch ( sev->getType() ) {
		case Msg::ValueEvent::TypeInteger :
			setSelected( sev->getUint32() );
			break;
		case Msg::ValueEvent::TypeBuffer :
			setSelected( sev->getBuffer() );
			break;
		default :
			throw InvalidArgument();
		}
		return;
	}
	catch ( BadCast & ) {
	}

	try {
		Dyn::ItemsDeletedCPtr sev = pEvent;
//		msgDbg( "dynEventCB(): IsA ItemsDeleted :-)" );
		clear();
		return;
	}
	catch ( BadCast & ) {
	}

	try {
		Dyn::ItemAppendedCPtr sev = pEvent;
//		msgDbg( "dynEventCB(): IsA ItemAppended :-)" );
		add( sev->getItemName() );
		return;
	}
	catch ( BadCast & ) {
	}

	throw InvalidArgument();

}

// ============================================================================

void UI::IComboBox::cachePropSelected(
	const	Uint32		id ) {

	if ( propSelected == id ) {
		return;
	}

	propSelected = id;

	sendEvent();

}

// ============================================================================

void UI::IComboBox::sendEvent() const {

	if ( textCache.getSize() ) {
		warnObservers( new Msg::ValueEvent( propSelected ) );
	}

}

// ============================================================================

Buffer UI::IComboBox::propSelectedTag = "selected";

// ============================================================================

