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
//	BFC.Dyn.Repository.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_Repository_H_
#define _BFC_Dyn_Repository_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace Dyn {

BFC_PTR_DECL( BFC_Dyn_DLL, Repository )

} // namespace Dyn
} // namespace BFC

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"
#include "BFC.Base.UUId.h"

#include "BFC.Dyn.TypePtr.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL Repository : virtual public SObject {

protected :

	/// \brief Creates a new Repository.

	Repository(
	);

public :

	/// \brief Returns the unique instance of this class.

	static RepositoryPtr instance(
	);

	void add(
			TypePtr		pType
	);

	void del(
			TypePtr		pType
	);

	TypePtr get(
		const	UUId &		pUUId
	) const;

	TypePtr get(
		const	Buffer &	pName
	) const;

protected :

private :

	Map< UUId, TypePtr >	mapUUId;
	Map< Buffer, TypePtr >	mapName;

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_Repository_H_

// ============================================================================

