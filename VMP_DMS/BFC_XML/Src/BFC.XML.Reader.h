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
//	BFC.XML.Reader.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_Reader_H_
#define _BFC_XML_Reader_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, Reader )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.XML.ContentHandler.h"
#include "BFC.XML.DTDHandler.h"
#include "BFC.XML.DeclHandler.h"
#include "BFC.XML.EntityResolver.h"
#include "BFC.XML.ErrorHandler.h"
#include "BFC.XML.InputSource.h"
#include "BFC.XML.LexicalHandler.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief Provides an interface for XML readers (i.e. parsers).
/// 
/// All readers use the class InputSource to read the input
/// document. Since you are normally interested in particular content
/// in the XML document, the reader reports the content through
/// special handler classes ( DTDHandler, DeclHandler,
/// ContentHandler, EntityResolver, ErrorHandler and
/// LexicalHandler ), which you must subclass, if you want to
/// process the contents.
/// 
/// Features and properties of the reader can be set with setFeature()
/// and setProperty() respectively. You can set the reader to use your
/// own subclasses with setEntityResolver(), setDTDHandler(),
/// setContentHandler(), setErrorHandler(), setLexicalHandler() and
/// setDeclHandler(). The parse itself is started with a call to
/// parse().
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL Reader : virtual public DObject {

public :

	Reader(
	);

	virtual ~Reader(
	);

	/// \brief Returns the value of the feature called \a name.
	///
	/// \throws NoSuchFeature
	///	If this reader doesn't have a feature called \a name.

	virtual Bool getFeature(
		const	Buffer &	name
	) const = 0;

	/// \brief Sets the feature called \a name to the given \a value.
	///
	/// If the reader doesn't have the feature nothing happens.

	virtual void setFeature(
		const	Buffer &	name,
			Bool		value
	) = 0;

	/// \brief Returns true iff the reader has the feature called \a name.

	virtual Bool hasFeature(
		const	Buffer &	name
	) const = 0;

	/// \brief Returns the value of the property called \a name.
	///
	/// \throws NoSuchProperty
	///	If this reader doesn't have a property called \a name.

	virtual void * getProperty(
		const	Buffer &	name
	) const = 0;

	/// \brief Sets the property \a name to \a value. If the reader doesn't
	///	have the property nothing happens.

	virtual void setProperty(
		const	Buffer &	name,
			void *		value
	) = 0;

	/// \brief Returns true iFf the reader has the property \a name.

	virtual Bool hasProperty(
		const	Buffer &	name
	) const = 0;

	/// \brief Sets the entity resolver to \a handler.

	virtual void setEntityResolver(
			EntityResolverPtr	handler
	) = 0;

	/// \brief Returns the entity resolver or 0 if none was set.

	virtual EntityResolverPtr getEntityResolver(
	) const = 0;

	/// \brief Sets the DTD handler to \a handler.

	virtual void setDTDHandler(
			DTDHandlerPtr		handler
	) = 0;

	/// \brief Returns the DTD handler or 0 if none was set.

	virtual DTDHandlerPtr getDTDHandler(
	) const = 0;

	/// \brief Sets the content handler to \a handler.

	virtual void setContentHandler(
			ContentHandlerPtr	handler
	) = 0;

	/// \brief Returns the content handler or 0 if none was set.

	virtual ContentHandlerPtr getContentHandler(
	) const = 0;

	/// \brief Sets the error handler to \a handler.

	virtual void setErrorHandler(
			ErrorHandlerPtr		handler
	) = 0;

	/// \brief Returns the error handler or 0 if none is set.

	virtual ErrorHandlerPtr getErrorHandler(
	) const = 0;

	/// \brief Sets the lexical handler to \a handler.

	virtual void setLexicalHandler(
			LexicalHandlerPtr	handler
	) = 0;

	/// \brief Returns the lexical handler or 0 if none was set.

	virtual LexicalHandlerPtr getLexicalHandler(
	) const = 0;

	/// \brief Sets the declaration handler to \a handler.

	virtual void setDeclHandler(
			DeclHandlerPtr		handler
	) = 0;

	/// \brief Returns the declaration handler or 0 if none was set.

	virtual DeclHandlerPtr getDeclHandler(
	) const = 0;

	/// \brief Reads an XML document from \a input and parses it.

	virtual void parse(
			InputSourcePtr	input
	) = 0;

	BASE_CLASS_GEN_EXCEPTION( "BFC.XML.Reader" );

	CLASS_EXCEPTION( NoSuchFeature, "No such feature" );
	CLASS_EXCEPTION( NoSuchProperty, "No such property" );

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	Reader(
		const	Reader&
	);

	Reader& operator = (
		const	Reader&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_Reader_H_

// ============================================================================

