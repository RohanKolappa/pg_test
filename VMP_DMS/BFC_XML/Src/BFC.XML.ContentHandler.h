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
//	BFC.XML.ContentHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_ContentHandler_H_
#define _BFC_XML_ContentHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, ContentHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

class Attributes;

// ============================================================================

/// \brief Interface used to report the logical content of XML data.
/// 
/// If the application needs to be informed of basic parsing events,
/// it can implement this interface and activate it using
/// Reader::setContentHandler(). The reader can then report basic
/// document-related events like the start and end of elements and
/// character data through this interface.
/// 
/// The order of events in this interface is very important, and
/// mirrors the order of information in the document itself. For
/// example, all of an element's content ( character data, processing
/// instructions, and sub-elements ) appears, in order, between the
/// startElement() event and the corresponding endElement() event.
/// 
/// The class DefaultHandler provides a default implementation for
/// this interface; subclassing from the DefaultHandler class is
/// very convenient if you only want to be informed of some parsing
/// events.
/// 
/// The startDocument() function is called at the start of the
/// document, and endDocument() is called at the end. For each element
/// startElement() is called, with endElement() being called at the
/// end of each element. The characters() function is called with
/// chunks of character data; ignorableWhitespace() is called with
/// chunks of whitespace and processingInstruction() is called with
/// processing instructions. If an entity is skipped skippedEntity()
/// is called. At the beginning of prefix-URI scopes
/// startPrefixMapping() is called.
///
/// \ingroup BFC_XML

class BFC_XML_DLL ContentHandler : virtual public SObject {

public :

	ContentHandler(
	);

	virtual ~ContentHandler(
	);

	/// \brief The reader calls this function when it starts parsing the
	/// document.
	///
	/// The reader calls this function just once, before any other functions in this
	/// class or in the DTDHandler class are called.

	virtual void startDocument(
	) = 0;

	/// \brief The reader calls this function after it has finished parsing.
	///
	/// It
	/// is called just once, and is the last handler function called. It
	/// is called after the reader has read all input or has abandoned
	/// parsing because of a fatal error.

	virtual void endDocument(
	) = 0;

	/// \brief The reader calls this function to signal the begin of a prefix-URI
	/// namespace mapping scope.
	///
	/// This information is not necessary for
	/// normal namespace processing since the reader automatically
	/// replaces prefixes for element and attribute names.
	/// 
	/// Note that startPrefixMapping() and endPrefixMapping() calls are
	/// not guaranteed to be properly nested relative to each other: all
	/// startPrefixMapping() events occur before the corresponding
	/// startElement() event, and all endPrefixMapping() events occur
	/// after the corresponding endElement() event, but their order is not
	/// otherwise guaranteed.
	/// 
	/// The argument \a prefix is the namespace prefix being declared and
	/// the argument \a uri is the namespace URI the prefix is mapped to.

	virtual void startPrefixMapping(
		const	Buffer &	prefix,
		const	Buffer &	uri
	) = 0;

	/// \brief The reader calls this function to signal the end of a prefix
	/// mapping for the prefix \a prefix.

	virtual void endPrefixMapping(
		const	Buffer &	prefix
	) = 0;

	/// \brief The reader calls this function when it has parsed a start element
	/// tag.
	/// 
	/// There is a corresponding endElement() call when the corresponding
	/// end element tag is read. The startElement() and endElement() calls
	/// are always nested correctly. Empty element tags
	/// cause a startElement() call to be immediately followed by an
	/// endElement() call.
	/// 
	/// The attribute list provided only contains attributes with explicit
	/// values. The attribute list contains attributes used for namespace
	/// declaration ( i.e. attributes starting with xmlns ) only if the
	/// namespace-prefix property of the reader is true.
	/// 
	/// The argument \a namespaceURI is the namespace URI, or
	/// an empty string if the element has no namespace URI or if no
	/// namespace processing is done. \a localName is the local name
	/// ( without prefix ), or an empty string if no namespace processing is
	/// done, \a qName is the qualified name ( with prefix ) and \a atts are
	/// the attributes attached to the element. If there are no
	/// attributes, \a atts is an empty attributes object.

	virtual void startElement(
		const	Buffer &	namespaceURI,
		const	Buffer &	localName,
		const	Buffer &	qName,
		const	Attributes &	atts
	) = 0;

	/// \brief The reader calls this function when it has parsed an end element
	/// tag with the qualified name \a qName, the local name \a localName
	/// and the namespace URI \a namespaceURI.

	virtual void endElement(
		const	Buffer &	namespaceURI,
		const	Buffer &	localName,
		const	Buffer &	qName
	) = 0;

	/// \brief The reader calls this function when it has parsed a chunk of
	/// character data ( either normal character data or character data
	/// inside a CDATA section; if you need to distinguish between those
	/// two types you must use LexicalHandler::startCDATA() and
	/// LexicalHandler::endCDATA() ).
	///
	/// The character data is reported in \a ch.
	/// 
	/// Some readers report whitespace in element content using the
	/// ignorableWhitespace() function rather than using this one.
	/// 
	/// A reader may report the character data of an element in more than
	/// one chunk; e.g. a reader might want to report "a\<b" in three
	/// characters() events ( "a ", "\<" and " b" ).

	virtual void characters(
		const	Buffer &	ch
	) = 0;

	/// \brief Some readers may use this function to report each chunk of
	/// whitespace in element content.
	///
	/// The whitespace is reported in \a ch.

	virtual void ignorableWhitespace(
		const	Buffer &	ch
	) = 0;

	/// \brief The reader calls this function when it has parsed a processing
	/// instruction.
	/// 
	/// \a target is the target name of the processing instruction and \a
	/// data is the data in the processing instruction.

	virtual void processingInstruction(
		const	Buffer &	target,
		const	Buffer &	data
	) = 0;

	/// \brief Some readers may skip entities if they have not seen the
	/// declarations ( e.g. because they are in an external DTD ). If they
	/// do so they report that they skipped the entity called \a name by
	/// calling this function.

	virtual void skippedEntity(
		const	Buffer &	name
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	ContentHandler(
		const	ContentHandler&
	);

	ContentHandler& operator = (
		const	ContentHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_ContentHandler_H_

// ============================================================================

