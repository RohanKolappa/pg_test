// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ITitledFrame.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ITitledFrame.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, ITitledFrame )

// ============================================================================

UI::ITitledFrame::ITitledFrame() {

	addProperty( propTitleTag );

}

// ============================================================================

void UI::ITitledFrame::setTitle(
	const	Buffer&		title ) {

	if ( propTitle == title ) {
		return;
	}

	propTitle = title;

	showPropTitle();

}

Buffer UI::ITitledFrame::getTitle() const {

	return propTitle;

}

// ============================================================================

void UI::ITitledFrame::setProperty(
	const	Buffer&		propertyName,
	const	Buffer&		propertyValue ) {

	if ( propertyName == propTitleTag ) {
		setTitle( propertyValue );
	}
	else {
		IElement::setProperty( propertyName, propertyValue );
	}

}

Buffer UI::ITitledFrame::getProperty(
	const	Buffer&		propertyName ) const {

	if ( propertyName == propTitleTag ) {
		return getTitle();
	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void UI::ITitledFrame::sendEvent() const {

}

// ============================================================================

Buffer UI::ITitledFrame::propTitleTag = "title";

// ============================================================================

