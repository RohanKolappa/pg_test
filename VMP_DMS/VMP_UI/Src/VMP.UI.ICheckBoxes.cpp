// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.ICheckBoxes.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.ICheckBoxes.h"

// ============================================================================

using namespace VMP::UI;

// ============================================================================

BFC_PTR_IMPL(ICheckBoxes)

// ============================================================================

ICheckBoxes::ICheckBoxes() {

	addProperty( propTitleTag );

}

ICheckBoxes::~ICheckBoxes() {



}

// ============================================================================

void ICheckBoxes::setTitle(
	const	BFC::Buffer&		title ) {

	if ( propTitle == title ) {
		return;
	}

	propTitle = title;

	showPropTitle();

}

BFC::Buffer ICheckBoxes::getTitle() const {

	return propTitle;

}

// ============================================================================

void ICheckBoxes::setProperty(
	const	BFC::Buffer&		propertyName,
	const	BFC::Buffer&		propertyValue ) {

	if ( propertyName == propTitleTag ) {
		setTitle( propertyValue );
	}

	else {
		IItemContainer::setProperty( propertyName, propertyValue );
	}


}

BFC::Buffer ICheckBoxes::getProperty(
	const	BFC::Buffer&		propertyName ) const {

	if ( propertyName == propTitleTag ) {
		return getTitle();
	}

	return IItemContainer::getProperty( propertyName );

}

// ============================================================================

void ICheckBoxes::sendEvent() const {

}

// ============================================================================

BFC::Buffer ICheckBoxes::propTitleTag = "title";

// ============================================================================

