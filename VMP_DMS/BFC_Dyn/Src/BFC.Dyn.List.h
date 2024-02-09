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
//	BFC.Dyn.List.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Dyn_List_H_
#define _BFC_Dyn_List_H_

// ============================================================================

#include "BFC.Dyn.DLL.h"

// ============================================================================

#include "BFC.Dyn.ListPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Dyn.Var.h"
#include "BFC.Dyn.VarArray.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace BFC {
namespace Dyn {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Dyn

class BFC_Dyn_DLL List : virtual public SObject {

public :

	ListPtr clone(
	) const;

	void add(
			VarPtr		pVar
	) {
		map.add( pVar->getName(), pVar );
	}

	void del(
			VarPtr		pVar
	) {
		map.del( pVar->getName() );
	}

	void kill(
	) {
		map.kill();
	}

	VarPtr get(
		const	Buffer &	pName
	) {
		return map[ pName ];
	}

	VarCArray getAll(
	) const;

	void setAll(
		const	VarArray &	pArray
	);

protected :

private :

	Map< Buffer, VarPtr >		map;	///< Map of variables.

};

// ============================================================================

} // namespace Dyn
} // namespace BFC

// ============================================================================

#endif // _BFC_Dyn_List_H_

// ============================================================================

