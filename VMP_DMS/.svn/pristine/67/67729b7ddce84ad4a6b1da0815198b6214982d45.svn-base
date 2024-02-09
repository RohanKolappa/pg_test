// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.IToolkit.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "VMP.UI.IToolkit.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

UI::IToolkit::IToolkit() {

}

// ============================================================================

UI::IElementFactoryPtr UI::IToolkit::getFactory(
	const	Buffer &		tagName ) const {

//	msgDbg( "getFactory(): finding factory for \""
//		+ tagName + "\"..." );

	IElementFactoryPtr res = factories[ tagName ];

//	msgDbg( "getFactory(): found it!" );

	return res;

}

UI::IElementPtr UI::IToolkit::getElement(
	const	Buffer &		tagName,
		IElementPtr		parent,
	const	Buffer &		eltName ) const {

	AttrTagValueArray props;

	props += AttrTagValue( "name", eltName );

	return getFactory( tagName )->createNew( parent, props );

}

UI::IElementPtr UI::IToolkit::getElement(
	const	Buffer &		tagName,
		IElementPtr		parent,
	const	AttrTagValueArray &	props ) const {

	return getFactory( tagName )->createNew( parent, props );

}

// ============================================================================

void UI::IToolkit::addFactory(
		IElementFactoryPtr	factory ) {

//	msgDbg( "addFactory(): factory for \"" + tagName
//		+ "\" registering..." );

	factory.setMaster( false );

	factories.add( factory->getTagName(), factory );

}

void UI::IToolkit::delFactory(
		IElementFactoryPtr	factory ) {

//	msgDbg( "delFactory(): factory for \"" + tagName
//		+ "\" unregistering..." );

	factory.setMaster( false );

	factories.del( factory->getTagName() );

}

// ============================================================================

