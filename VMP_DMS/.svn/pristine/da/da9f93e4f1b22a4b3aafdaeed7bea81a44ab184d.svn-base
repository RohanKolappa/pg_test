// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.UI.XMLParser.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.DOM.Document.h"
#include "BFC.DOM.Element.h"

#include "VMP.UI.IItemContainer.h"

#include "VMP.UI.XMLParser.h"

// ============================================================================

using namespace BFC;
using namespace VMP;

// ============================================================================

BFC_PTR_IMPL_NS( UI, XMLParser )

// ============================================================================

UI::XMLParser::XMLParser() {

	setObjectName( "VMP.UI.XMLParser" );

}

// ============================================================================

UI::IElementPtr UI::XMLParser::parse(
	const	BFC::Buffer &	pManifest,
		IToolkitPtr	pToolkit ) const {

	IElementPtr parent;

	return parse( pManifest, pToolkit, parent );

}

UI::IElementPtr UI::XMLParser::parse(
	const	Buffer &	pManifest,
		IToolkitPtr	pToolkit,
		IElementPtr	pParent ) const {

//	msgDbg( "parse(string): --->" );

	IElementPtr res;

	Buffer		docContent = pManifest;
	DOM::Document	domDoc;
	Buffer		errorMsg;

	try {
		domDoc.setContent( docContent, false );
	}
	catch ( Exception & e ) {
		msgErr( "parse(): I can't parse your f...ing pManifest!" );
		msgErr( "parse(): " + Buffer( e.what() ) );
		throw CantParseManifest( e, "Malformed XML file!" );
	}

	DOM::Element uiElt = domDoc.getDocumentElement();

	if ( uiElt.tagName() != "UI" ) {
		throw CantParseManifest( "No <UI>...</UI> tags!" );
	}

	uiElt = uiElt.firstChild().toElement();

	if ( uiElt.isEmpty() ) {
		throw CantParseManifest( "No root widget element!" );
	}

//	if ( ! uiElt.nextSibling().isEmpty() ) {
//		throw CantParseManifest( "Multiple root elements!" );
//	}

	res = parse( pParent, uiElt, pToolkit );

//	msgDbg( "parse(string): <---" );

	return res;

}

// ============================================================================

UI::IElementPtr UI::XMLParser::parse(
		IElementPtr	pParent,
	const	DOM::Element &	pElement,
		IToolkitPtr	pToolkit ) const {

//	msgDbg( "parse(xml): --->" );

	// sanity checks...

	Buffer	eltType = pElement.tagName();

	if ( eltType != "Spacer"
	  && ! pElement.hasAttribute( "name" ) ) {
		throw CantParseManifest( "parse(): no \"name\" attribute to element!" );
	}

	// Parse all attributes...

	DOM::NamedNodeMap	attrMap = pElement.getAttributes();
	AttrTagValueArray	attributes;

	for ( Uint32 i = 0 ; i < ( Uint32 )attrMap.count() ; i++ ) {
		DOM::Attr attr = attrMap.getItem( i ).toAttr();
		attributes += AttrTagValue(
			attr.name(),
			attr.value() );
	}

	// Find and dispatch to the appropriate factory...

	IElementFactoryPtr	factory;
	IElementPtr		res;

	try {
		factory = pToolkit->getFactory( eltType );
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't find factory for type \"" + eltType + "\"!" );
		return res;
	}

	try {
		res = factory->createNew( pParent, attributes );
	}
	catch ( Exception& e ) {
		msgExc( e, "Can't use factory on type \"" + eltType + "\"!" );
		return res;
	}

	// Sub elements / properties / items ?

	for ( DOM::Node sub = pElement.firstChild()
	    ; ! sub.isEmpty() ; sub = sub.nextSibling() ) {

		if ( ! sub.isElement() ) {
			msgWrn( "parse(): skipping unknown non-element \""
				+ sub.getNodeName() + "\"!" );
			continue;
		}

		DOM::Element subElt = sub.toElement();

		if ( subElt.tagName() == "Property" ) {
			DOM::NamedNodeMap subAttrs = subElt.getAttributes();
			if ( subAttrs.count() != 1 ) {
				throw CantParseManifest( "parse(): multiple property attributes!" );
			}
			DOM::Attr attr = subAttrs.getItem( 0 ).toAttr();
			res->setProperty(
				attr.name(),
				attr.value() );
		}
		else if ( subElt.tagName() == "Item" ) {
			DOM::NamedNodeMap subAttrs = subElt.getAttributes();
			if ( subAttrs.count() != 1 ) {
				throw CantParseManifest( "parse(): multiple item attributes!" );
			}
			DOM::Attr attr = subAttrs.getItem( 0 ).toAttr();
			if ( attr.name() != "text" ) {
				throw CantParseManifest( "parse(): unknown attribute to item!" );
			}
			IItemContainerPtr ires;
			try {
				ires = res;
			}
			catch ( BadCast& ) {
				throw CantParseManifest( "parse(): item found in no item container!" );
			}
			ires->add( attr.value() );
		}
		else {
			// Must be a subelement!
			try {
				parse( res, subElt, pToolkit );
			}
			catch ( Exception& e ) {
				msgExc( e, "Can't parse sub-element tagged \""
					+ subElt.tagName()
					+ "\"!" );
			}
		}

	}

//	msgDbg( "parse(xml): <---" );

	return res;

}

// ============================================================================

