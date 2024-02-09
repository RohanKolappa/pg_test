// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IElement.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.Base.BufferParser.h"

#include "VMP.UI.IElement.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IElement )
//BFC_PTR_IMPL_NS( UI, Observer )

// ============================================================================

UI::IElement::IElement(
		IElementPtr	pParent,
	const	Buffer &	pEltName,
	const	Buffer &	pTypeName,
	const	Bool		pTypeData ) {

	typeData = pTypeData;
	eltName = pEltName;

	setObjectName( pTypeName + "[\"" + pEltName + "\"]" );

//	msgDbg( "IElement(): --->" );

	propHidden = true;
	propEnabled = true;
	propCaption = "<no caption>";

	addProperty( propEnabledTag );
	addProperty( propHiddenTag );
	addProperty( propCaptionTag );

	if ( pParent ) {
		if ( ! pParent->isContainer() ) {
			msgWrn( "IElement(): parent is NOT a container!!!" );
		}
		else {
			parent = IElementPtr( pParent, false );
			parent->children += this;
		}
	}

//	msgDbg( "IElement(): <---" );

}

UI::IElement::~IElement() {

//	msgDbg( "~IElement(): --->" );

	children.kill();
//	observer.kill();

	delAllProperties();

//	msgDbg( "~IElement(): <---" );

}

// ============================================================================

UI::IElementPtr UI::IElement::getChildByName(
	const	Buffer &		name ) const {

	BufferArray array = BufferParser::tokenize( name, "." );

	if ( array.isEmpty() ) {
		throw InvalidArgument( "getChildByName(" + name + "): EMPTY!" );
	}

	return getChildByName( array, 0 );

}

UI::IElementPtr UI::IElement::findChildByName(
	const	Buffer &		name ) const {

	BufferArray array = BufferParser::tokenize( name, "." );

	if ( array.isEmpty() ) {
		throw InvalidArgument( "findChildByName(" + name + "): EMPTY!" );
	}

	return findChildByName( array, 0 );

}

void UI::IElement::delAllElements() {

//	msgDbg( "delAllElements(): --->" );

	children.kill();

//	msgDbg( "delAllElements(): <---" );

}

// ============================================================================

void UI::IElement::setCaption(
	const	Buffer &		caption ) {

	if ( propCaption == caption ) {
		return;
	}

	propCaption = caption;

	showPropCaption();

}

Buffer UI::IElement::getCaption() const {

	return propCaption;

}

// ============================================================================

void UI::IElement::setHidden(
	const	Bool		yesno ) {

	if ( propHidden == yesno ) {
		return;
	}

	propHidden = yesno;

	showPropHidden();

}

Bool UI::IElement::getHidden() const {

	return propHidden;

}

// ============================================================================

void UI::IElement::setEnabled(
	const	Bool		yesno ) {

	if ( propEnabled == yesno ) {
		return;
	}

	propEnabled = yesno;

	showPropEnabled();

}

Bool UI::IElement::getEnabled() const {

	return propEnabled;

}

// ============================================================================

void UI::IElement::setProperty(
	const	Buffer &		propertyName,
	const	Buffer &		propertyValue ) {

	if ( propertyName == propEnabledTag ) {
		setEnabled( propertyValue.toBool() );
	}

	else if ( propertyName == propHiddenTag ) {
		setHidden( propertyValue.toBool() );
	}

	else if ( propertyName == propCaptionTag ) {
		setCaption( propertyValue );
	}

	else {
		msgWrn( "No such property: \"" + propertyName + "\"!" );
	}

}

Buffer UI::IElement::getProperty(
	const	Buffer &		propertyName ) const {

	if ( propertyName == propEnabledTag ) {
		return Buffer( propEnabled );
	}

	if ( propertyName == propHiddenTag ) {
		return Buffer( propHidden );
	}

	if ( propertyName == propCaptionTag ) {
		return propCaption;
	}

	throw InvalidArgument( "No such property: \"" + propertyName + "\"!" );

}

// ============================================================================

const Buffer & UI::IElement::getPropertyTag(
	const	Uint32		propertyIndex ) const {

	if ( propertyIndex >= properties.size() ) {
		throw InvalidArgument( "Index out of range!" );
	}

	return properties[ propertyIndex ];

}

Buffer UI::IElement::getPropertyVal(
	const	Uint32		propertyIndex ) const {

	return getProperty( getPropertyTag( propertyIndex ) );

}

Uint32 UI::IElement::getNbrProperties() const {

	return properties.size();

}

// ============================================================================

void UI::IElement::syncObservers() const {

	sendEvent();

}

// ============================================================================

void UI::IElement::addProperty(
	const	Buffer &		name ) {

	for ( Uint32 i = 0 ; i < properties.size() ; i++ ) {
		if ( properties[i] == name ) {
			throw InvalidArgument( "Duplicate property name(\""
				+ name + "\")!" );
		}
	}

	properties += name;

}

void UI::IElement::delAllProperties() {

	properties.kill();

}

// ============================================================================

UI::IElementPtr UI::IElement::getChildByName(
	const	BufferArray &	names,
	const	Uint32		index ) const {

	if ( ! isContainer() ) {
		throw InvalidArgument( "IElement \"" + getElementName()
			+ "\" is NOT a container!" );
	}

	Buffer cache = names[ index ];

	for ( Uint32 i = 0 ; i < children.size() ; i++ ) {
		if ( children[i]->getElementName() == cache ) {
			return ( index == names.size() - 1
				? children[i]
				: children[i]->getChildByName( names, index + 1 ) );
		}
	}

	throw InvalidArgument( "No sub \""
		+ cache
		+ "\" in \""
		+ getElementName()
		+ "\"" );

}

UI::IElementPtr UI::IElement::findChildByName(
	const	BufferArray &	names,
	const	Uint32		index ) const {

	if ( ! isContainer() ) {
		return IElementPtr();
	}

	Buffer cache = names[ index ];
	Uint32 i;

	for ( i = 0 ; i < children.size() ; i++ ) {
		if ( children[i]->getElementName() == cache ) {
			if ( index == names.size() - 1 ) {
				// FOUND!
				return children[i];
			}
			IElementPtr res = children[i]->findChildByName(
					names, index + 1 );
			if ( res ) {
				return res;
			}
		}
	}

	for ( i = 0 ; i < children.size() ; i++ ) {
		IElementPtr res = children[i]->findChildByName( names, index );
		if ( res ) {
			res.setMaster( false );
			return res;
		}
	}

	return IElementPtr();

}

// ============================================================================

//void UI::IElement::emitEvent(
//		Msg::EventPtr	ev) {
//
//	warnObservers( ev );
//
//}

// ============================================================================

UI::IElement::IElement() {

	throw InternalError();

}

// ============================================================================

void UI::IElement::cachePropEnabled(
	const	Bool	yesno ) {

	propEnabled = yesno;

}

void UI::IElement::cachePropHidden(
	const	Bool	yesno ) {

	propHidden = yesno;

}

// ============================================================================

Buffer UI::IElement::propEnabledTag = "enabled";
Buffer UI::IElement::propHiddenTag = "hidden";
Buffer UI::IElement::propCaptionTag = "caption";

// ============================================================================

