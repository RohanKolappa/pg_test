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
//	VMP.UI.UObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_UObject_H_
#define _VMP_UI_UObject_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, UObject )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.DL.Object.h"

#include "VMP.UI.IElement.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_UI

class VMP_UI_DLL UObject : public BFC::DL::Object {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new UObject.

	UObject(
	) :
		BFC::DL::Object( getClassType() ) {
	}

	/// \brief Creates a new UObject.

	UObject(
		const	BFC::DL::TypeCPtr &	pType
	) :
		BFC::DL::Object( pType ) {
	}

public :

	/// \brief Attaches this object to its user interface.
	///
	/// This method calls the protected virtual initUICallback() to let
	/// subclasses perform specific initialization.

	virtual void setUserInterface(
			IElementPtr		pUserInterface
	);

	/// \brief Returns true iff a user interface has been attached.

	virtual BFC::Bool hasUserInterface(
	) const;

	/// \brief Returns the currently attached user interface.

	virtual IElementPtr getUserInterface(
	) const;

protected :

	/// \brief Initializes the user interface.
	///
	/// Default is to do nothing.

	virtual void initUICallback(
			IElementPtr		pUserInterface
	);

private :

	IElementPtr		ui;	///< User interface.

	/// \brief Forbidden copy constructor.

	UObject(
		const	UObject		&
	);

	/// \brief Forbidden copy operator.

	UObject & operator = (
		const	UObject		&
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_UObject_H_

// ============================================================================

