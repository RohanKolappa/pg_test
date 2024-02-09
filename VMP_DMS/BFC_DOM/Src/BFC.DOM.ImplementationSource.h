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
//	BFC.DOM.ImplementationSource.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DOM_ImplementationSource_H_
#define _BFC_DOM_ImplementationSource_H_

// ============================================================================

#include "BFC.DOM.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

BFC_PTR_DECL( BFC_DOM_DLL, ImplementationSource )

} // namespace DOM
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.DOM.ImplementationPtr.h"
#include "BFC.DOM.ImplementationListPtr.h"

// ============================================================================

namespace BFC {
namespace DOM {

// ============================================================================

/// \brief Supplies one or more Implementations, based upon requested features
///	and versions.
///
/// This interface permits a DOM implementer to supply one or more
/// implementations, based upon requested features and versions, as specified
/// in DOM Features. Each implemented ImplementationSource object is listed in
/// the binding-specific list of available sources so that its Implementation
/// objects are made available.
///
/// \since DOM Level 3
///
/// \ingroup BFC_DOM

class BFC_DOM_DLL ImplementationSource : virtual public SObject {

public :

	virtual ImplementationCPtr getImplementation(
		const	Buffer &	features
	) const = 0;

	virtual ImplementationListCPtr getImplementationList(
		const	Buffer &	features
	) const = 0;

};

// ============================================================================

} // namespace DOM
} // namespace BFC

// ============================================================================

#endif // _BFC_DOM_ImplementationSource_H_

// ============================================================================

