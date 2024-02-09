// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::DOM".
// 
// "BFC::DOM" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// ( at your option ) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::DOM"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.DOM.Handler.cpp
// 
// Description:
//	...
// 
// Author( s ):
//	...
// 
// ============================================================================

#include "BFC.XML.Attributes.h"
#include "BFC.XML.SimpleReader.h"

#include "BFC.DOM.Entity.h"
#include "BFC.DOM.Notation.h"

#include "BFC.DOM.DocumentImpl.h"
#include "BFC.DOM.DocumentTypeImpl.h"
#include "BFC.DOM.ElementImpl.h"
#include "BFC.DOM.EntityImpl.h"
#include "BFC.DOM.Handler.h"
#include "BFC.DOM.NotationImpl.h"

// ============================================================================

using namespace BFC;

// ============================================================================

DOM::HandlerImpl::HandlerImpl(
		DocumentImplPtr	pDoc,
		Bool		namespaceProcessing ) :

	doc	( 0, true, true ),
	node	( 0, true, true ) {

	doc	= pDoc;
	node	= pDoc;
	cdata	= false;
	nsProc	= namespaceProcessing;

}

// ============================================================================

void DOM::HandlerImpl::startElement(
	const	Buffer &	nsURI,
	const	Buffer &	/* lName */,
	const	Buffer &	qName,
	const	XML::Attributes &	atts ) {

	NodeImplPtr n;

	if ( nsProc ) {
		n = doc->createElementNS( nsURI, qName );
	}
	else {
		n = doc->createElement( qName );
	}

	node->appendChild( n );
	node = n;

	for ( Uint32 i = 0 ; i < atts.getLength() ; i++ ) {
		if ( nsProc ) {
			( ( ElementImplPtr )node )->setAttributeNS(
				atts.getUri( i ),
				atts.getQualifiedName( i ),
				atts.getValue( i ) );
		}
		else {
			( ( ElementImplPtr )node )->setAttribute(
				atts.getQualifiedName( i ),
				atts.getValue( i ) );
		}
	}

}

void DOM::HandlerImpl::endElement(
	const	Buffer &	/* nsURI */,
	const	Buffer &	/* lName */,
	const	Buffer &	/* qName */ ) {

	if ( node == doc ) {
		throw InvalidArgument( "Invalid document: endElement()" );
	}

	node = node->getParent();

}

void DOM::HandlerImpl::characters(
	const	Buffer &	ch ) {

	if ( node == doc ) {
		throw InvalidArgument( "Invalid document: characters()" );
	}

	NodeImplPtr n;

	if ( cdata ) {
		n = doc->createCDATASection( ch );
	}
	else if ( ! entityName.isEmpty() ) {
		EntityImplPtr e = new EntityImpl(
			doc, 0, entityName,
			Buffer(), Buffer(), Buffer() );
		e->value = ch;
		doc->getDocType()->appendChild( e );
		n = doc->createEntityReference( entityName );
	}
	else {
		n = doc->createTextNode( ch );
	}

	node->appendChild( n );

}

void DOM::HandlerImpl::processingInstruction(
	const	Buffer &	target,
	const	Buffer &	data ) {

	NodeImplPtr n;

	n = doc->createProcessingInstruction( target, data );

	node->appendChild( n );

}

void DOM::HandlerImpl::skippedEntity(
	const	Buffer &	name ) {

	if ( XML::qt_xml_skipped_entity_in_content ) {
		NodeImplPtr n = doc->createEntityReference( name );
		node->appendChild( n );
	}

}

// ============================================================================

void DOM::HandlerImpl::notationDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	NotationImplPtr n = new NotationImpl( doc, 0, name, publicId, systemId );

	doc->getDocType()->appendChild( n );

}

void DOM::HandlerImpl::unparsedEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId,
	const	Buffer &	notationName ) {

	EntityImplPtr e = new EntityImpl( doc, 0, name,
			publicId, systemId, notationName );

	doc->getDocType()->appendChild( e );

}

// ============================================================================

void DOM::HandlerImpl::startDTD(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	doc->getDocType()->name = name;
	doc->getDocType()->publicId = publicId;
	doc->getDocType()->systemId = systemId;

}

void DOM::HandlerImpl::startEntity(
	const	Buffer &	name ) {

	entityName = name;

}

void DOM::HandlerImpl::endEntity(
	const	Buffer &	/* name */ ) {

	entityName.kill();

}

void DOM::HandlerImpl::startCDATA() {

	cdata = true;

}

void DOM::HandlerImpl::endCDATA() {

	cdata = false;

}

void DOM::HandlerImpl::comment(
	const	Buffer &	ch ) {

	NodeImplPtr n;

	n = doc->createComment( ch );

	node->appendChild( n );

}

// ============================================================================

void DOM::HandlerImpl::externalEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	unparsedEntityDecl( name, publicId, systemId, Buffer() );

}

// ============================================================================
// BFC::DOM::Handler
// ============================================================================

DOM::Handler::Handler(
		Document	pDoc,
		Bool		namespaceProcessing ) {

	doc	= pDoc;
	node	= pDoc;
	cdata	= false;
	nsProc	= namespaceProcessing;

}

// ============================================================================

void DOM::Handler::startElement(
	const	Buffer &	nsURI,
	const	Buffer &	/* lName */,
	const	Buffer &	qName,
	const	XML::Attributes &	atts ) {

	Element n;

	if ( nsProc ) {
		n = doc.createElementNS( nsURI, qName );
	}
	else {
		n = doc.createElement( qName );
	}

	node.appendChild( n );
	node = n;

	for ( Uint32 i = 0 ; i < atts.getLength() ; i++ ) {
		if ( nsProc ) {
			n.setAttributeNS(
				atts.getUri( i ),
				atts.getQualifiedName( i ),
				atts.getValue( i ) );
		}
		else {
			n.setAttribute(
				atts.getQualifiedName( i ),
				atts.getValue( i ) );
		}
	}

}

void DOM::Handler::endElement(
	const	Buffer &	/* nsURI */,
	const	Buffer &	/* lName */,
	const	Buffer &	/* qName */ ) {

	if ( node == doc ) {
		throw InvalidArgument( "Invalid document: endElement()" );
	}

	node = node.getParent();

}

void DOM::Handler::characters(
	const	Buffer &	ch ) {

	if ( node == doc ) {
		throw InvalidArgument( "Invalid document: characters()" );
	}

	Node n;

	if ( cdata ) {
		n = doc.createCDATASection( ch );
	}
	else if ( ! entityName.isEmpty() ) {
		Entity e = doc.createEntity(
			entityName, Buffer(), Buffer(), Buffer(), ch );
		doc.getDocType().appendChild( e );
		n = doc.createEntityReference( entityName );
	}
	else {
		n = doc.createTextNode( ch );
	}

	node.appendChild( n );

}

void DOM::Handler::processingInstruction(
	const	Buffer &	target,
	const	Buffer &	data ) {

	Node n;

	n = doc.createProcessingInstruction( target, data );

	node.appendChild( n );

}

void DOM::Handler::skippedEntity(
	const	Buffer &	name ) {

	if ( XML::qt_xml_skipped_entity_in_content ) {
		Node n = doc.createEntityReference( name );
		node.appendChild( n );
	}

}

// ============================================================================

void DOM::Handler::notationDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	Notation n = doc.createNotation( name, publicId, systemId );

	doc.getDocType().appendChild( n );

}

void DOM::Handler::unparsedEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId,
	const	Buffer &	notationName ) {

	Entity e = doc.createEntity( name,
			publicId, systemId, notationName, Buffer() );

	doc.getDocType().appendChild( e );

}

// ============================================================================

void DOM::Handler::startDTD(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	doc.getDocType().setName( name );
	doc.getDocType().setPublicId( publicId );
	doc.getDocType().setSystemId( systemId );

}

void DOM::Handler::startEntity(
	const	Buffer &	name ) {

	entityName = name;

}

void DOM::Handler::endEntity(
	const	Buffer &	/* name */ ) {

	entityName.kill();

}

void DOM::Handler::startCDATA() {

	cdata = true;

}

void DOM::Handler::endCDATA() {

	cdata = false;

}

void DOM::Handler::comment(
	const	Buffer &	ch ) {

	Node n;

	n = doc.createComment( ch );

	node.appendChild( n );

}

// ============================================================================

void DOM::Handler::externalEntityDecl(
	const	Buffer &	name,
	const	Buffer &	publicId,
	const	Buffer &	systemId ) {

	unparsedEntityDecl( name, publicId, systemId, Buffer() );

}

// ============================================================================

