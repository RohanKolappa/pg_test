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
//	VMP.UI.UObjectDescr.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_UObjectDescr_H_
#define _VMP_UI_UObjectDescr_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

BFC_PTR_DECL( VMP_UI_DLL, UObjectDescr )

} // namespace UI
} // namespace VMP

// ============================================================================

#include "BFC.DL.Descr.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_UI

class VMP_UI_DLL UObjectDescr : public BFC::DL::Descr {

public :

	UObjectDescr(
	);

	UObjectDescr(
		const	BFC::Buffer &	pName,
		const	BFC::Buffer &	pCtgy,
		const	BFC::Buffer &	pDesc,
		const	BFC::Buffer &	pGUI = BFC::Buffer()
	);

	UObjectDescr(
		const	UObjectDescr &	pOther
	);

	virtual BFC::DL::DescrPtr clone(
	) const;

	const BFC::Buffer & getGUI(
	) const {
		return gui;
	}

private :

	BFC::Buffer	gui;

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_UObjectDescr_H_

// ============================================================================

