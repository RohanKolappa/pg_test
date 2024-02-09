// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IListView.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IListView.h"

// ============================================================================

using namespace VMP::UI;

// ============================================================================

BFC_PTR_IMPL(IListView)

// ============================================================================

IListView::IListView() {

//	addProperty( textTag );

}

// ============================================================================

void IListView::setProperty(
	const	BFC::Buffer&		propertyName,
	const	BFC::Buffer&		propertyValue ) {

//	if ( propertyName == textTag ) {
//		setText( propertyValue );
//	}
//	else {
		IElement::setProperty( propertyName, propertyValue );
//	}

}

BFC::Buffer IListView::getProperty(
	const	BFC::Buffer&		propertyName ) const {

//	if ( propertyName == textTag ) {
//		return getText();
//	}

	return IElement::getProperty( propertyName );

}

// ============================================================================

void IListView::sendEvent() const {

}

// ============================================================================

//BFC::Buffer IListView::textTag = "text";

// ============================================================================

