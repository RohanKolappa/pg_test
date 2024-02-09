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
//	BFC.XML.EntityResolver.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_XML_EntityResolver_H_
#define _BFC_XML_EntityResolver_H_

// ============================================================================

#include "BFC.XML.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace XML {

BFC_PTR_DECL( BFC_XML_DLL, EntityResolver )

} // namespace XML
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.XML.InputSource.h"

// ============================================================================

namespace BFC {
namespace XML {

// ============================================================================

/// \brief The EntityResolver class provides an interface to
///	resolve external entities contained in XML data.
/// 
/// If an application needs to implement customized handling for
/// external entities, it must implement this interface, i.e.
/// resolveEntity(), and register it with
/// Reader::setEntityResolver().
/// 
/// \ingroup BFC_XML

class BFC_XML_DLL EntityResolver : virtual public SObject {

public :

	EntityResolver(
	);

	virtual ~EntityResolver(
	);

	/// \brief Called by the reader before it opens any external
	/// entity, except the top-level document entity.
	///
	/// The application may
	/// request the reader to resolve the entity itself ( \a ret is 0 ) or
	/// to use an entirely different input source ( \a ret points to the
	/// input source ).
	/// 
	/// The argument \a publicId is the public identifier of the external
	/// entity, \a systemId is the system identifier of the external
	/// entity and \a ret is the return value of this function. If \a ret
	/// is 0 the reader should resolve the entity itself, if it is
	/// non-zero it must point to an input source which the reader uses
	/// instead.

	virtual void resolveEntity(
		const	Buffer &		publicId,
		const	Buffer &		systemId,
			InputSourcePtr &	ret
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	EntityResolver(
		const	EntityResolver&
	);

	EntityResolver& operator = (
		const	EntityResolver&
	);

};

// ============================================================================

} // namespace XML
} // namespace BFC

// ============================================================================

#endif // _BFC_XML_EntityResolver_H_

// ============================================================================

