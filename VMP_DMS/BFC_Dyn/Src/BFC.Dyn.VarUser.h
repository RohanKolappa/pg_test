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
//	BFC.Dyn.VarUser.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_VarUser_H_
#define _BFC_Dyn_VarUser_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.VarUserPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Dyn.ListPtr.h"
#include "BFC.Dyn.VEnumPtr.h"
#include "BFC.Dyn.VFloatPtr.h"
#include "BFC.Dyn.VIntPtr.h"
#include "BFC.Dyn.VarArray.h"
#include "BFC.Dyn.VarPtr.h"

#include "BFC.IO.ByteInputStreamPtr.h"
#include "BFC.IO.ByteOutputStreamPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Base class of all objects that expose a collection of Var objects.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL VarUser : virtual public DObject {

protected :

	/// \brief Creates a new VarUser.

	VarUser(
	);

	/// \brief Creates a copy of \a pOther.

	VarUser(
		const	VarUser &	pOther
	);

	VarUser & operator = (
		const	VarUser &	pOther
	);

public :

	/// \brief Returns the Var named \a pName (RW).

	VarPtr getDynVar(
		const	Buffer &	pName
	);

	/// \brief Returns the Var named \a pName (RO).

	VarCPtr getDynVar(
		const	Buffer &	pName
	) const;

	/// \brief Returns the VEnum named \a pName (RW).

	VEnumPtr getDynVEnum(
		const	Buffer &	pName
	) {
		return getDynVar( pName );
	}

	/// \brief Returns the VEnum named \a pName (RO).

	VEnumCPtr getDynVEnum(
		const	Buffer &	pName
	) const {
		return getDynVar( pName );
	}

	/// \brief Returns the VFloat named \a pName (RW).

	VFloatPtr getDynVFloat(
		const	Buffer &	pName
	) {
		return getDynVar( pName );
	}

	/// \brief Returns the VFloat named \a pName (RO).

	VFloatCPtr getDynVFloat(
		const	Buffer &	pName
	) const {
		return getDynVar( pName );
	}

	/// \brief Returns the VInt named \a pName (RW).

	VIntPtr getDynVInt(
		const	Buffer &	pName
	) {
		return getDynVar( pName );
	}

	/// \brief Returns the VInt named \a pName (RO).

	VIntCPtr getDynVInt(
		const	Buffer &	pName
	) const {
		return getDynVar( pName );
	}

	/// \brief Returns the value of the Var named \a pName.

	Buffer getDynVarValue(
		const	Buffer &	pName
	) const;

	/// \brief Sets the value of the Var named \a pName to \a pValue.

	void setDynVarValue(
		const	Buffer &	pName,
		const	Buffer &	pValue
	);

	/// \brief Returns a list of all registered variables.

	VarCArray getDynVars(
	) const;

	virtual void doSerialize(
			IO::ByteOutputStreamPtr
					pOStream
	) const;

	virtual void unSerialize(
			IO::ByteInputStreamPtr
					pIStream
	);

	/// \brief Creates a new VarUser, acting as a proxy for the collection
	///	of Var objects exposed by \a pOther.

	void attachDynVarList(
			VarUserPtr	pOther
	);

	/// \brief Adds \a pDynVar to the collection of exposed Var objects.

	void addDynVar(
			VarPtr		pDynVar
	);

	/// \brief Removes \a pDynVar from the collection of exposed Var objects.

	void delDynVar(
			VarPtr		pDynVar
	);

	/// \brief Empties the collection of exposed Var objects.

	void delAllDynVars(
	);

private :

	ListPtr		list;	///< List of variables.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_VarUser_H_

// ============================================================================

