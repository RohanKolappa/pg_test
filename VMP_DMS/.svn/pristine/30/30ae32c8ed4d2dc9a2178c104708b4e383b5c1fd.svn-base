// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Dyn".
// 
// "BFC::Dyn" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Dyn" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Dyn"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Dyn.TArray.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_TArray_H_
#define _BFC_Dyn_TArray_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

BFC_PTR_DECL( BFC_Dyn_DLL, TArray )

} // namespace Dyn
} // namespace BFC

// ============================================================================

#include "BFC.Dyn.Type.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Represents a Dynamic Array.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL TArray : public Type {

public :

	/// \brief Creates a new unlocked TArray.
	///
	/// The type of child elements will have to be specified using
	/// setSubType() before this Type could be used.

	TArray(
	);

	/// \brief Creates a new unlocked TArray, with the type of child
	///	elements set to \a pSubType.
	///
	/// \param pSubType
	///	Type of child elements.

	TArray(
			TypePtr		pSubType
	);

	/// \brief Creates a copy of \a pOther, cloning the type of child
	///	elements (if any).
	///
	/// \param pOther
	///	TArray to copy.

	TArray(
		const	TArray &	pOther
	);

	/// \brief Copies \a pOther, cloning the type of child elements (if
	///	any).
	///
	/// \param pOther
	///	TArray to copy.
	///
	/// \throws Locked
	///	If this object is locked.

	TArray & operator = (
		const	TArray &	pOther
	);

	virtual SPtr clone(
	) const;

	virtual VarPtr create(
	);

	/// \brief Locks this Type and its subtype.

	virtual void setLocked(
	);

	/// \brief Returns the type of child elements.

	TypePtr getSubType(
	) const;

	/// \brief Sets the type of child elements.
	///
	/// \param pSubType
	///	Type of child elements.
	///
	/// \throws Locked
	///	If this object is locked.

	void setSubType(
			TypePtr		pSubType
	);

protected :

private :

	TypePtr		sub;	///< Type of child elements.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_TArray_H_

// ============================================================================

