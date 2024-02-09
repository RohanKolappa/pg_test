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
//	VMP.SegUtils.PathComputer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_PathComputer_H_
#define _VMP_SegUtils_PathComputer_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.PathComputerPtr.h"

// ============================================================================

#include "BFC.Base.SObject.h"

#include "BFC.VFS.FSPath.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL PathComputer : virtual public BFC::SObject {

public :

	virtual BFC::FSPath getFilePath(
		const	BFC::Uint64	pCurrIndx
	) const = 0;

protected :

private :

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_PathComputer_H_

// ============================================================================

