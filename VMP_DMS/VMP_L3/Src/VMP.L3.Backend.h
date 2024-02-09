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
//	VMP.L3.Backend.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_L3_Backend_H_
#define _VMP_L3_Backend_H_

// ============================================================================

#include "VMP.L3.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace L3 {

BFC_PTR_DECL( VMP_L3_DLL, Backend )

} // namespace L3
} // namespace VMP

// ============================================================================

#include "BFC.Dyn.VarUser.h"

#include "BFC.Msg.Observer.h"

#include "BFC.DL.Object.h"
#include "BFC.DL.Type.h"

#include "VMP.DynVar.h"

#include "VMP.UI.IElement.h"
#include "VMP.UI.UObject.h"

// ============================================================================

namespace VMP {
namespace L3 {

// ============================================================================

/// \brief Base class of all backends.
///
/// \ingroup VMP_L3

class VMP_L3_DLL Backend :

	public UI::UObject,
	public BFC::Msg::Observer,
	public BFC::Dyn::VarUser {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new Backend.

	Backend(
	) :
		UI::UObject( getClassType() ) {
	}

	/// \brief Creates a new Backend.

	Backend(
		const	BFC::DL::TypeCPtr &	pType
	) :
		UI::UObject( pType ) {
	}

public :

	virtual DynVar getUserSettings(
	);

	virtual void setUserSettings(
		const	DynVar &		settings
	);

	virtual void startBackend(
	);

	virtual void stopBackend(
	);

	BASE_CLASS_GEN_EXCEPTION( "VMP.L3.Backend" );

	CLASS_EXCEPTION( AlreadyInitialized, "Already initialized" );
	CLASS_EXCEPTION( BrokenPipe, "Broken pipe" );
	CLASS_EXCEPTION( EndOfStream, "End of stream" );
	CLASS_EXCEPTION( NotInitialized, "Not initialized" );
	CLASS_EXCEPTION( NotSuitable, "Not suitable" );

protected :

private :

	/// \brief Forbidden copy constructor.

	Backend(
		const	Backend&
	);

	/// \brief Forbidden copy operator.

	Backend& operator = (
		const	Backend&
	);

};

// ============================================================================

} // namespace L3
} // namespace VMP

// ============================================================================

#endif // _VMP_L3_Backend_H_

// ============================================================================

