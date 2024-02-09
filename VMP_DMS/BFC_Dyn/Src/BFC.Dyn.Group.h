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
//	BFC.Dyn.Group.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_Group_H_
#define _BFC_Dyn_Group_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

BFC_PTR_DECL( BFC_Dyn_DLL, Group )

} // namespace Dyn
} // namespace BFC

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Dyn.VarUser.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief Collection of VarUser.
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL Group : virtual public DObject {

public :

	void addUser(
			VarUserPtr	pUser
	) {
		array.add( pUser->getObjectName(), pUser );
	}

	void delUser(
			VarUserPtr	pUser
	) {
		array.del( pUser->getObjectName() );
	}

	void delAllUsers(
	) {
		array.kill();
	}

	VarUserPtr getUser(
		const	Buffer &	pName
	) {
		return array[ pName ];
	}

	VarUserCPtr getUser(
		const	Buffer &	pName
	) const {
		return array[ pName ];
	}

	/// \brief Returns the value of the Var named \a pDynVarName, and exposed by VarUser
	///	\a pUserName.

	Buffer getDynVarValue(
		const	Buffer &	pUserName,
		const	Buffer &	pDynVarName
	) const {
		return array[ pUserName ]->getDynVarValue( pDynVarName );
	}

	/// \brief Sets the value of the Var named \a pDynVarName, and exposed by USer
	///	\a pUserName, to \a pValue.

	void setDynVarValue(
		const	Buffer &	pUserName,
		const	Buffer &	pDynVarName,
		const	Buffer &	pValue
	) {
		array[ pUserName ]->setDynVarValue( pDynVarName, pValue );
	}

protected :

private :

	Map< Buffer, VarUserPtr >	array;

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_Group_H_

// ============================================================================

