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
//	BFC.Dyn.Type.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_Type_H_
#define _BFC_Dyn_Type_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.TypePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Cloneable.h"
#include "BFC.Base.UUId.h"

#include "BFC.Dyn.VarPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Base class of all Dynamic Types.
///
/// Dynamic Types mimic the standard C/C++ available type constructs. Types
/// belong to one of those groups:
/// -	simple types: TInt, TFloat, TBuffer, TEnum;
/// -	complex types: TArray, TStruct.
///
/// A Type may have a name, which is manipulated via setName() and getName().
///
/// A Type has a "lock" state, which is manipulated via setLocked() and
/// isLocked(). Once locked, a Type cannot be manipulated anymore. A Type
/// cannot be associated with a Dynamic Variable if it's not locked.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL Type : virtual public Cloneable {

public :

	/// \brief Creates a new unnamed and unlocked Type.

	Type(
	);

	/// \brief Creates an unlocked copy of \a pOther.
	///
	/// \param pOther
	///	The Type to copy.

	Type(
		const	Type &		pOther
	);

	/// \brief Copies \a pOther into this object.
	///
	/// \param pOther
	///	The Type to copy.
	///
	/// \throws Locked
	///	If this object is locked.

	Type & operator = (
		const	Type &		pOther
	);

	/// \brief Creates an instance of this Type.
	///
	/// This Type is automatically locked.

	virtual VarPtr create(
	) = 0;

	/// \brief Sets the unique Id identifying this Type.

	void setUUId(
		const	UUId &		pUUId
	);

	/// \brief Returns true iff a unique Id has been set for this Type.

	Bool hasUUId(
	) const;

	/// \brief Returns the unique Id identifying this Type.

	const UUId & getUUId(
	) const;

	/// \brief Sets the name of this Type to \a pName.
	///
	/// \param pName
	///	The new name of this Type.
	///
	/// \throws Locked
	///	If this object is locked.

	virtual void setName(
		const	Buffer &	pName
	);

	/// \brief Returns true iff a unique name has been set for this Type.

	Bool hasName(
	) const;

	/// \brief Returns the name of this Type.

	virtual const Buffer & getName(
	) const;

	/// \brief Locks this Type.
	///
	/// A unique Id must have been set before calling this method!

	virtual void setLocked(
	);

	/// \brief Returns true iff this Type is locked.

	virtual Bool isLocked(
	) const;

protected :

	void crashIfLocked(
	);

private :

	UUId	uuid;
	Bool	lock;	///< Type Locked state.
	Buffer	name;	///< Type name.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_Type_H_

// ============================================================================

