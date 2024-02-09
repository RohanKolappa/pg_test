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
//	VMP.UI.VStructEncoder.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_UI_VStructEncoder_H_
#define _VMP_UI_VStructEncoder_H_

// ============================================================================

#include "VMP.UI.DLL.h"

// ============================================================================

#include "VMP.UI.VStructEncoderPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.Dyn.VStruct.h"

#include "VMP.UI.IElementPtr.h"

// ============================================================================

namespace VMP {
namespace UI {

// ============================================================================

/// \brief Creates synchronized Dyn::VStruct from existing UI::IElement.
///
/// \ingroup VMP_UI

class VMP_UI_DLL VStructEncoder : virtual public BFC::SObject {

public :

	/// \brief Creates a new VStructEncoder.

	VStructEncoder(
	);

	/// \brief Destroys this object.

	virtual ~VStructEncoder(
	);

	BFC::Dyn::VStructPtr encode(
			IElementPtr	pRoot
	);

protected :

	BFC::Dyn::TypePtr encodeType(
			IElementPtr	pRoot
	);

	void syncVar(
			IElementPtr	pElt,
			BFC::Dyn::VarPtr
					pVar
	);

private :

	/// \brief Forbidden copy constructor.

	VStructEncoder(
		const	VStructEncoder &
	);

	/// \brief Forbidden copy operator.

	VStructEncoder & operator = (
		const	VStructEncoder &
	);

};

// ============================================================================

} // namespace UI
} // namespace VMP

// ============================================================================

#endif // _VMP_UI_VStructEncoder_H_

// ============================================================================

