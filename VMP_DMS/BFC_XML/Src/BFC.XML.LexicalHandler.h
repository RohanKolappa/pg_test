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
//	BFC.XML.LexicalHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_LexicalHandler_H_
#define _BFC_XML_LexicalHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, LexicalHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief Interface used to report the lexical content of XML data.
/// 
/// The events in the lexical handler apply to the entire document,
/// not just to the document element, and all lexical handler events
/// appear between the content handler's startDocument and endDocument
/// events.
/// 
/// You can set the lexical handler with
/// Reader::setLexicalHandler().
/// 
/// This interface's design is based on the the SAX2 extension
/// LexicalHandler.
/// 
/// The interface provides the startDTD(), endDTD(), startEntity(),
/// endEntity(), startCDATA(), endCDATA() and comment() functions.
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL LexicalHandler : virtual public SObject {

public :

	LexicalHandler(
	);

	virtual ~LexicalHandler(
	);

	/// \brief Called by the reader to report the start of a DTD
	/// declaration, if any.
	///
	/// It reports the name of the document type in
	/// \a name, the public identifier in \a publicId and the system
	/// identifier in \a systemId.
	/// 
	/// If the public identifier is missing, \a publicId is set to
	/// an empty string. If the system identifier is missing, \a systemId is
	/// set to an empty string. Note that it is not valid XML to have a
	/// public identifier but no system identifier; in such cases a parse
	/// error will occur.
	/// 
	/// All declarations reported through DTDHandler or
	/// DeclHandler appear between the startDTD() and endDTD() calls.

	virtual void startDTD(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	) = 0;

	/// \brief Called by the reader to report the end of a DTD
	/// declaration, if any.

	virtual void endDTD(
	) = 0;

	/// \brief Called by the reader to report the start of an entity
	/// called \a name.
	/// 
	/// Note that if the entity is unknown, the reader reports it through
	/// ContentHandler::skippedEntity() and not through this
	/// function.

	virtual void startEntity(
		const	Buffer &	name
	) = 0;

	/// \brief Called by the reader to report the end of an entity
	/// called \a name.
	/// 
	/// For every startEntity() call, there is a corresponding endEntity()
	/// call. The calls to startEntity() and endEntity() are properly
	/// nested.

	virtual void endEntity(
		const	Buffer &	name
	) = 0;

	/// \brief Called by the reader to report the start of a CDATA
	/// section.
	///
	/// The content of the CDATA section is reported through the
	/// ContentHandler::characters() function. This function is
	/// intended only to report the boundary.

	virtual void startCDATA(
	) = 0;

	/// \brief Called by the reader to report the end of a CDATA
	/// section.

	virtual void endCDATA(
	) = 0;

	/// \brief Called by the reader to report an XML comment anywhere
	///	in the document.
	///
	/// The text of the comment is put in \a ch.

	virtual void comment(
		const	Buffer &	ch
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	LexicalHandler(
		const	LexicalHandler&
	);

	LexicalHandler& operator = (
		const	LexicalHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_LexicalHandler_H_

// ============================================================================

