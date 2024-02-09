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
//	BFC.DOM.ImplementationImpl.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_DOM_ImplementationImpl_H_
#define _BFC_DOM_ImplementationImpl_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

BFC_PTR_DECL( BFC_DOM_DLL, ImplementationImpl )

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.DOM.Implementation.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Implements Implementation.
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ImplementationImpl : public Implementation {

public :

	/// \brief Creates a new ImplementationImpl.

	ImplementationImpl(
	);

	virtual ImplementationPtr clone(
	) const;

	virtual Bool hasFeature(
		const	Buffer &	pFeature,
		const	Buffer &	pVersion
	) const;

	virtual DocumentType createDocumentType(
		const	Buffer		& qName,
		const	Buffer		& publicId,
		const	Buffer		& systemId
	) const;

	virtual Document createDocument(
		const	Buffer		& nsURI,
		const	Buffer		& qName,
		const	DocumentType	& doctype
	) const;

	virtual SCPtr getFeature(
		const	Buffer &	pFeature,
		const	Buffer &	pVersion
	) const;

protected :

private :

	/// \brief Forbidden copy constructor.

	ImplementationImpl(
		const	ImplementationImpl &
	);

	/// \brief Forbidden copy operator.

	ImplementationImpl & operator = (
		const	ImplementationImpl &
	);

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ImplementationImpl_H_

// ============================================================================

