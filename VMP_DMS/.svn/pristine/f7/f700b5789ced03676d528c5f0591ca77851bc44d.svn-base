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
//	BFC.XML.DebugHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_DebugHandler_H_
#define _BFC_XML_DebugHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, DebugHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.XML.ContentHandler.h"
#include "BFC.XML.DTDHandler.h"
#include "BFC.XML.DeclHandler.h"
#include "BFC.XML.EntityResolver.h"
#include "BFC.XML.ErrorHandler.h"
#include "BFC.XML.LexicalHandler.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief Provides a verbose implementation of all the XML handler classes.
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL DebugHandler :
	public ContentHandler,
	public ErrorHandler,
	public DTDHandler,
	public EntityResolver,
	public LexicalHandler,
	public DeclHandler,
	virtual public DObject {

public :

	/// \brief Creates a new DebugHandler.

	DebugHandler(
	);

	/// \brief Destroys this object.

	virtual ~DebugHandler(
	);

	// Reimplemented from ContentHandler...

	virtual void startDocument(
	);

	virtual void endDocument(
	);

	virtual void startPrefixMapping(
		const	Buffer &	prefix,
		const	Buffer &	uri
	);

	virtual void endPrefixMapping(
		const	Buffer &	prefix
	);

	virtual void startElement(
		const	Buffer &	namespaceURI,
		const	Buffer &	localName,
		const	Buffer &	qName,
		const	Attributes	& atts
	);

	virtual void endElement(
		const	Buffer &	namespaceURI,
		const	Buffer &	localName,
		const	Buffer &	qName
	);

	virtual void characters(
		const	Buffer &	ch
	);

	virtual void ignorableWhitespace(
		const	Buffer &	ch
	);

	virtual void processingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	);

	virtual void skippedEntity(
		const	Buffer &	name
	);

	// Reimplemented from ErrorHandler...

	virtual void warning(
		const	Buffer &	exception
	);

	virtual void error(
		const	Buffer &	exception
	);

	virtual void fatalError(
		const	Buffer &	exception
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

	// Reimplemented from EntityResolver...

	virtual void resolveEntity(
		const	Buffer &	publicId,
		const	Buffer &	systemId,
			InputSourcePtr &	ret
	);

	// Reimplemented from LexicalHandler...

	virtual void startDTD(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

	virtual void endDTD(
	);

	virtual void startEntity(
		const	Buffer &	name
	);

	virtual void endEntity(
		const	Buffer &	name
	);

	virtual void startCDATA(
	);

	virtual void endCDATA(
	);

	virtual void comment(
		const	Buffer &	ch
	);

	// Reimplemented from DeclHandler...

	virtual void attributeDecl(
		const	Buffer &	eName,
		const	Buffer &	aName,
		const	Buffer &	type,
		const	Buffer &	valueDefault,
		const	Buffer &	value
	);

	virtual void internalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	value
	);

	virtual void externalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	);

private:

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DebugHandler(
		const	DebugHandler&
	);

	DebugHandler& operator = (
		const	DebugHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_DebugHandler_H_

// ============================================================================

