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
//	VMP.UI.UObjectFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_UObjectFactory_H_
#define _VMP_UI_UObjectFactory_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, UObjectFactory )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.DL.Builder.h"
#include "BFC.DL.Factory.h"

#include "VMP.UI.IToolkit.h"
#include "VMP.UI.UObject.h"
#include "VMP.UI.XMLParser.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================
// VMP::UI::UObjectFactory
// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_UI

class VMP_UI_DLL UObjectFactory : public BFC::DL::Factory {

public :

	/// \brief Creates a new UObjectFactory.

	UObjectFactory(
			BFC::DL::BuilderPtr	pBuilder
	);

	/// \brief Constructs a new UObject, and attaches it to a user inteface
	///	instantiated using toolkit \a pToolkit, and having \a pParent
	///	as parent user interface element.
	///
	/// If \a pToolkit is NULL, no user interface is created.
	///
	/// If \a pParent is NULL, the user interface becomes a "top-level widget".

	UObjectPtr getNewUObject(
			IToolkitPtr	pToolkit = 0,
			IElementPtr	pParent = 0
	);

protected :

private :

	XMLParserPtr			parser;

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_UObjectFactory_H_

// ============================================================================

