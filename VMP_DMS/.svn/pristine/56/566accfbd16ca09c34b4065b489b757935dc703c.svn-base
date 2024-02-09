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
//	VMP.L2.EngineFactory.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_EngineFactory_H_
#define _VMP_L2_EngineFactory_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.EngineFactoryPtr.h"

// ============================================================================

#include "BFC.DL.Descr.h"

#include "VMP.L2.Engine.h"

#include "VMP.UI.IElement.h"
#include "VMP.UI.IToolkit.h"
#include "VMP.UI.UObjectFactory.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL EngineFactory : public UI::UObjectFactory {

public :

	/// \brief Creates a new EngineFactory.

	EngineFactory(
			BFC::DL::BuilderPtr	pBuilder
	) :
		UI::UObjectFactory( pBuilder ) {
	}

	/// \brief Constructs a new Engine without user interface.

	EnginePtr getNewEngine(
	);

	/// \brief Constructs a new Engine, and attaches it to a user inteface
	///	instantiated using toolkit \a pToolkit.

	EnginePtr getNewEngine(
			UI::IToolkitPtr	pToolkit
	);

	/// \brief Constructs a new Engine, and attaches it to a user inteface
	///	instantiated using toolkit \a pToolkit, and having \a pParent
	///	as parent user interface element.

	EnginePtr getNewEngine(
			UI::IToolkitPtr	pToolkit,
			UI::IElementPtr	pParent
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_EngineFactory_H_

// ============================================================================

