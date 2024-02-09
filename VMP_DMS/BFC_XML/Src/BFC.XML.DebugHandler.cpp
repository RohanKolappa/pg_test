// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.XML.DebugHandler.cpp
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#include "BFC.XML.Attributes.h"
#include "BFC.XML.DebugHandler.h"

// ============================================================================

using namespace BFC;

// ============================================================================

BFC_PTR_IMPL_NS( XML, DebugHandler )

// ============================================================================

XML::DebugHandler::DebugHandler() {

	setObjectName( "BFC.XML.DebugHandler" );

}

XML::DebugHandler::~DebugHandler() {



}

// ============================================================================

void XML::DebugHandler::startDocument() {

	msgDbg( "startDocument()" );

}

void XML::DebugHandler::endDocument() {

	msgDbg( "endDocument()" );

}

void XML::DebugHandler::startPrefixMapping(
	const	Buffer &	prefix,
	const	Buffer &	uri ) {

	msgDbg( "startPrefixMapping(): prefix: \""
		+ prefix + "\", uri: \""
		+ uri + "\"" );

}

void XML::DebugHandler::endPrefixMapping(
	const	Buffer &	prefix ) {

	msgDbg( "endPrefixMapping(): prefix: \""
		+ prefix + "\"" );

}

void XML::DebugHandler::startElement(
	const	Buffer &	namespaceURI,
	const	Buffer &	localName,
	const	Buffer &	qName,
	const	Attributes &	atts ) {

	msgDbg( "startElement(): ns: \""
		+ namespaceURI + "\", lname: \""
		+ localName + "\", qname: \""
		+ qName + "\", #attrs: "
		+ Buffer( atts.getCount() ) );

}

void XML::DebugHandler::endElement(
	const	Buffer &	namespaceURI,
	const	Buffer &	localName,
	const	Buffer &	qName ) {

	msgDbg( "endElement(): ns: \""
		+ namespaceURI + "\", lname: \""
		+ localName + "\", qname: \""
		+ qName + "\"" );

}

void XML::DebugHandler::characters(
	const	Buffer &	ch ) {

	msgDbg( "characters(): #chars: "
		+ Buffer( ch.getLength() ) + ", chars: \""
		+ ch + "\"" );

}

void XML::DebugHandler::ignorableWhitespace(
	const	Buffer &	ch ) {

	msgDbg( "ignorableWhitespace(): #chars: "
		+ Buffer( ch.getLength() ) );

}

void XML::DebugHandler::processingInstruction(
	const	Buffer &	target,
	const	Buffer &	data ) {

	msgDbg( "processingInstruction(): target: \""
		+ target + "\", data: \""
		+ data + "\"" );

}

void XML::DebugHandler::skippedEntity(
	const	Buffer &	name ) {

	msgDbg( "skippedEntity(): name: \""
		+ name + "\"" );

}

// ============================================================================

void XML::DebugHandler::warning(
	const	Buffer &	msg ) {

	msgDbg( "WARNG: " + msg );

}

void XML::DebugHandler::error(
	const	Buffer &	msg ) {

	msgDbg( "ERROR: " + msg );

}

void XML::DebugHandler::fatalError(
	const	Buffer &	msg ) {

	msgDbg( "FATAL: " + msg );

}

// ============================================================================

void XML::DebugHandler::notationDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	msgDbg( "notationDecl(): name: \""
		+ name + "\", publicId: \""
		+ publicId + "\", systemId: \""
		+ systemId + "\"" );

}

void XML::DebugHandler::unparsedEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId,
	const	Buffer &	notationName ) {

	msgDbg( "unparsedEntityDecl(): name: \""
		+ name + "\", publicId: \""
		+ publicId + "\", systemId: \""
		+ systemId + "\", notationName: \""
		+ notationName + "\"" );

}

// ============================================================================

void XML::DebugHandler::resolveEntity(
	const	Buffer &	publicId,
	const	Buffer &	systemId,
		InputSourcePtr &	/*ret*/ ) {

	msgDbg( "resolveEntity(): publicId: \""
		+ publicId + "\", systemId: \""
		+ systemId + "\"" );

}

// ============================================================================

void XML::DebugHandler::startDTD(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	msgDbg( "startDTD(): name: \""
		+ name + "\", publicId: \""
		+ publicId + "\", systemId: \""
		+ systemId + "\"" );

}

void XML::DebugHandler::endDTD() {

	msgDbg( "endDTD()" );

}

void XML::DebugHandler::startEntity(
	const	Buffer &	name ) {

	msgDbg( "startEntity(): name: \""
		+ name + "\"" );

}

void XML::DebugHandler::endEntity(
	const	Buffer &	name ) {

	msgDbg( "endEntity(): name: \""
		+ name + "\"" );

}

void XML::DebugHandler::startCDATA() {

	msgDbg( "startCDATA()" );

}

void XML::DebugHandler::endCDATA() {

	msgDbg( "endCDATA()" );

}

void XML::DebugHandler::comment(
	const	Buffer &	ch ) {

	msgDbg( "comment(): text: \""
		+ ch + "\"" );

}

// ============================================================================

void XML::DebugHandler::attributeDecl(
	const	Buffer &	eName,
	const	Buffer &	aName,
	const	Buffer &	type,
	const	Buffer &	defVal,
	const	Buffer &	value ) {

	msgDbg( "attributeDecl(): eName: \""
		+ eName + "\", aName: \""
		+ aName + "\", type: \""
		+ type + "\", defVal: \""
		+ defVal + "\", value: \""
		+ value + "\"" );

}

void XML::DebugHandler::internalEntityDecl(
	const	Buffer &	name,
	const	Buffer &	value ) {

	msgDbg( "itnernalEntityDecl(): name: \""
		+ name + "\", value: \""
		+ value + "\"" );

}

void XML::DebugHandler::externalEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	msgDbg( "externalEntityDecl(): name: \""
		+ name + "\", publicId: \""
		+ publicId + "\", systemId: \""
		+ systemId + "\"" );

}

// ============================================================================

