// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IElementFactory.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IElementFactory.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, IElementFactory )

// ============================================================================

UI::IElementFactory::IElementFactory(
	const	Buffer &		pTagName ) :

	tagName( pTagName ) {

	setObjectName( "VMP.UI.IElementFactory" );

}

// ============================================================================

UI::IElementPtr UI::IElementFactory::createNew(
		IElementPtr		parent,
	const	AttrTagValueArray &	props ) {

// FIXME: Check all attributes here...

	IElementPtr res = getNew( parent, props );

	return res;

}

// ============================================================================

void UI::IElementFactory::addSupportedAttribute(
	const	Buffer &		name ) {

	attributes += name;

}

// ============================================================================

Buffer UI::IElementFactory::getAttribute(
	const	AttrTagValueArray &	props,
	const	Buffer &		name ) const {

	for ( Uint32 i = 0 ; i < props.size() ; i++ ) {
		if ( props[i].left() == name ) {
			return props[i].right();
		}
	}

	throw NoSuchAttribute( name );

}

// ============================================================================

