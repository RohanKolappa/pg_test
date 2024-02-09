// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Base.SObject.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_SObject_H_
#define _BFC_Base_SObject_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

class SCPtr;
class SPtr;
class SPtrHub;

// ============================================================================

/// \brief Base class of all objects pointed to by smart pointers (see SPtr).
///
/// An 'SObject' ('S' stands for 'Smart') is the base class a SPtr
/// object can point to.
///
/// \ingroup BFC_Base

class BFC_Light_DLL SObject {

	FIX_FOR_WIN32_COMP_PROBLEM( SObject )

public :

	/// \brief Creates a new SObject.
	///
	/// A SPtrHub is automatically created to point to this object.

	SObject(
	);

	/// \brief Creates a copy of \a pOther.
	///
	/// A SPtrHub is automatically created to point to this object.

	SObject(
		const	SObject &	pOther
	);

	/// \brief Destroys this object.
	///
	/// If the unique SPtrHub pointing to this object was not used by
	/// any SPtr object, then it is destroyed too.

	virtual ~SObject(
	);

	/// \brief Returns the number of SPtr objects pointing to this object
	///	(via its unique SPtrHub).

	Uint32 getRefCount(
	) const;

	/// \brief Returns the number of master SPtr objects pointing to this
	///	object (via its unique SPtrHub).

	Uint32 getMstCount(
	) const;

	SPtr getThis(
		const	Bool		pMaster = false,
		const	Bool		pSticky = true
	);

	SCPtr getCThis(
		const	Bool		pMaster = false,
		const	Bool		pSticky = true
	) const;

protected :

private :

	friend class SCPtr;

	/// \brief Returns a pointer to the unique SPtrHub pointing to this
	///	object.

	SPtrHub * getHub(
	) const {
		return sPtrHub;
	}

	SPtrHub	* sPtrHub;	///< Unique SPtrHub pointing to this object.

	/// \brief Forbidden copy operator.

	SObject & operator = (
		const	SObject		&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_SObject_H_

// ============================================================================


