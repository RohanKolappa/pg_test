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
// (at your option) any later version.
// 
// "BFC::DOM" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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
//	BFC.DOM.Handler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_Handler_H_
#define _BFC_DOM_Handler_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.XML.DefaultHandler.h"

#include "BFC.DOM.Document.h"
#include "BFC.DOM.Node.h"
#include "BFC.DOM.DocumentImplPtr.h"
#include "BFC.DOM.NodeImplPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

class HandlerImpl : public XML::DefaultHandler {

public :

	HandlerImpl(
			DocumentImplPtr	d,
			Bool		namespaceProcessing
	);

	// Reimplemented from ContentHandler...

	virtual void startElement(
		const	Buffer &	nsURI,
		const	Buffer &	localName,
		const	Buffer &	qName,
		const	XML::Attributes &	atts
	);

	virtual void endElement(
		const	Buffer &	nsURI,
		const	Buffer &	localName,
		const	Buffer &	qName
	);

	virtual void characters(
		const	Buffer &	ch
	);

	virtual void processingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	);

	virtual void skippedEntity(
		const	Buffer &	name
	);

	// Reimplemented from DTDHandler...

	virtual void notationDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	virtual void unparsedEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId,
		const	Buffer &	notationName
	);

	// Reimplemented from LexicalHandler...

	virtual void startDTD(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	virtual void startEntity(
		const	Buffer &
	);

	virtual void endEntity(
		const	Buffer &
	);

	virtual void startCDATA(
	);

	virtual void endCDATA(
	);

	virtual void comment(
		const	Buffer &	ch
	);

	// Reimplemented from DeclHandler...

	virtual void externalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	BASE_CLASS_GEN_EXCEPTION( "BFC.XML.DOM.HandlerImpl" );

	CLASS_EXCEPTION( InvalidDocument, "Invalid document" );

private :

	DocumentImplPtr	doc;
	NodeImplPtr	node;
	Buffer		entityName;
	Bool		cdata;
	Bool		nsProc;

};

// ============================================================================

class Handler : public XML::DefaultHandler {

public :

	Handler(
			Document	d,
			Bool		namespaceProcessing
	);

	// Reimplemented from ContentHandler...

	virtual void startElement(
		const	Buffer &	nsURI,
		const	Buffer &	localName,
		const	Buffer &	qName,
		const	XML::Attributes &	atts
	);

	virtual void endElement(
		const	Buffer &	nsURI,
		const	Buffer &	localName,
		const	Buffer &	qName
	);

	virtual void characters(
		const	Buffer &	ch
	);

	virtual void processingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	);

	virtual void skippedEntity(
		const	Buffer &	name
	);

	// Reimplemented from DTDHandler...

	virtual void notationDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	virtual void unparsedEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId,
		const	Buffer &	notationName
	);

	// Reimplemented from LexicalHandler...

	virtual void startDTD(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	virtual void startEntity(
		const	Buffer &
	);

	virtual void endEntity(
		const	Buffer &
	);

	virtual void startCDATA(
	);

	virtual void endCDATA(
	);

	virtual void comment(
		const	Buffer &	ch
	);

	// Reimplemented from DeclHandler...

	virtual void externalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

private :

	Document	doc;
	Node		node;
	Buffer		entityName;
	Bool		cdata;
	Bool		nsProc;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_Handler_H_

// ============================================================================

