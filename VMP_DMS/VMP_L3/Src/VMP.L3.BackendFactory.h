// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L3".
// 
// "VMP::L3" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L3" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L3"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L3.BackendFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L3_BackendFactory_H_
#define _VMP_L3_BackendFactory_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, BackendFactory )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.DL.Descr.h"

#include "VMP.L3.Backend.h"

#include "VMP.UI.IElement.h"
#include "VMP.UI.IToolkit.h"
#include "VMP.UI.UObjectFactory.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L3

class VMP_L3_DLL BackendFactory : public UI::UObjectFactory {

public :

	/// \brief Creates a new BackendFactory.

	BackendFactory(
			BFC::DL::BuilderPtr	pBuilder
	) :
		UI::UObjectFactory( pBuilder ) {
	}

	/// \brief Constructs a new Backend without user interface.

	BackendPtr getNewBackend(
	);

	/// \brief Constructs a new Backend, and attaches it to a user inteface
	///	instantiated using toolkit \a pToolkit.

	BackendPtr getNewBackend(
			UI::IToolkitPtr	pToolkit
	);

	/// \brief Constructs a new Backend, and attaches it to a user inteface
	///	instantiated using toolkit \a pToolkit, and having \a pParent
	///	as parent user interface element.

	BackendPtr getNewBackend(
			UI::IToolkitPtr	pToolkit,
			UI::IElementPtr	pParent
	);

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_BackendFactory_H_

// ============================================================================

