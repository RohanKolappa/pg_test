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
//	BFC.XML.DTDHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_DTDHandler_H_
#define _BFC_XML_DTDHandler_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, DTDHandler )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief Interface used to report DTD content of XML data.
/// 
/// If an application needs information about notations and unparsed
/// entities, it can implement this interface and register an instance
/// with Reader::setDTDHandler().
/// 
/// Note that this interface includes only those DTD events that the
/// XML recommendation requires processors to report, i.e. notation
/// and unparsed entity declarations using notationDecl() and
/// unparsedEntityDecl() respectively.
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL DTDHandler : virtual public SObject {

public :

	DTDHandler(
	);

	virtual ~DTDHandler(
	);

	/// \brief Called by the reader when it has parsed a notation
	///	declaration.
	/// 
	/// The argument \a name is the notation name, \a publicId is the
	/// notation's public identifier and \a systemId is the notation's
	/// system identifier.

	virtual void notationDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId
	) = 0;

	/// \brief Called by the reader when it finds an unparsed entity
	///	declaration.
	/// 
	/// The argument \a name is the unparsed entity's name, \a publicId is
	/// the entity's public identifier, \a systemId is the entity's system
	/// identifier and \a notationName is the name of the associated
	/// notation.

	virtual void unparsedEntityDecl(
		const	Buffer &	name,
		const	Buffer &	publicId,
		const	Buffer &	systemId,
		const	Buffer &	notationName
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	DTDHandler(
		const	DTDHandler&
	);

	DTDHandler& operator = (
		const	DTDHandler&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_DTDHandler_H_

// ============================================================================

