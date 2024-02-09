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
//	BFC.DL.Object.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_DL_Object_H_
#define _BFC_DL_Object_H_

// ============================================================================

#include "BFC.DL.DLL.h"

// ============================================================================

#include "BFC.DL.ObjectPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.DL.CounterPtr.h"
#include "BFC.DL.TypePtr.h"

// ============================================================================

namespace BFC {
namespace DL {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_DL

class BFC_DL_DLL Object : virtual public DObject {

public :

	/// \brief Returns the unique Type object associated to this class.

	static const TypeCPtr & getClassType(
	);

protected :

	/// \brief Creates a new Object of Type \a pType.

	Object(
		const	TypeCPtr &	pType
	);

	/// \brief Creates a new Object having the same Type as \a pOther.

	Object(
		const	Object &	pOther
	);

public :

	/// \brief Destroys this Object, decrementing any associated Counter.

	virtual ~Object(
	);

	/// \brief Returns the Type object associated to this object.

	const TypeCPtr & getType(
	) const {
		return type;
	}

	/// \brief Returns true iff this object is of a Type that inherits
	///	\a pClassType.

	Bool inherits(
		const	TypeCPtr &	pClassType
	) const;

	/// \brief Associated this object to Counter \a pCounter.
	///
	/// If a Counter was already attached, this Counter is decremented
	/// first.

	void setCounter(
		const	CounterPtr &	pCounter
	);

protected :

	/// \brief Sets the Type object of this object to \a pClassType.

	void setType(
		const	TypeCPtr &	pClassType
	);

private :

	TypeCPtr	type;	///< Real Type of this object.
	CounterPtr	cntr;	///< Counter associated to this object.

	/// \brief Forbidden default constructor.

	Object(
	);

	/// \brief Forbidden copy operator.

	Object & operator = (
		const	Object &
	);

};

// ============================================================================

} // namespace DL
} // namespace BFC

// ============================================================================

#endif // _BFC_DL_Object_H_

// ============================================================================

