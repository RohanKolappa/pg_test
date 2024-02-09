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
//	BFC.XML.DeclHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_DeclHandler_H_
#define _BFC_XML_DeclHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, DeclHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief The DeclHandler class provides an interface to report declaration
/// content of XML data.
/// 
/// You can set the declaration handler with
/// Reader::setDeclHandler().
/// 
/// This interface is based on the SAX2 extension DeclHandler.
/// 
/// The interface provides attributeDecl(), internalEntityDecl() and
/// externalEntityDecl() functions.
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL DeclHandler : virtual public SObject {

public :

	DeclHandler(
	);

	virtual ~DeclHandler(
	);

	/// \brief Called by the reader to report an attribute type
	/// declaration.
	///
	/// Only the effective ( first ) declaration for an
	/// attribute is reported.
	/// 
	/// The reader passes the name of the associated element in \a eName
	/// and the name of the attribute in \a aName. It passes a string that
	/// represents the attribute type in \a type and a string that
	/// represents the attribute default in \a valueDefault. This string
	/// is one of "#IMPLIED", "#REQUIRED", "#FIXED" or an empty string ( if
	/// none of the others applies ). The reader passes the attribute's
	/// default value in \a value. If no default value is specified in the
	/// XML file, \a value is an empty string.

	virtual void attributeDecl(
		const	Buffer &	eName,
		const	Buffer &	aName,
		const	Buffer &	type,
		const	Buffer &	valueDefault,
		const	Buffer &	value
	) = 0;

	/// \brief Called by the reader to report an internal entity
	/// declaration.
	///
	/// Only the effective ( first ) declaration is reported.
	/// 
	/// The reader passes the name of the entity in \a name and the value
	/// of the entity in \a value.

	virtual void internalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	value
	) = 0;

	/// \brief Called by the reader to report a parsed external entity
	/// declaration.
	///
	/// Only the effective ( first ) declaration for each
	/// entity is reported.
	/// 
	/// The reader passes the name of the entity in \a name, the public
	/// identifier in \a publicId and the system identifier in \a
	/// systemId. If there is no public identifier specified, it passes
	/// an empty string in \a publicId.

	virtual void externalEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DeclHandler(
		const	DeclHandler&
	);

	DeclHandler& operator = (
		const	DeclHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_DeclHandler_H_

// ============================================================================

