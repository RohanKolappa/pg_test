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
//	VMP.HLS.ProgramInfo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_ProgramInfo_H_
#define _VMP_HLS_ProgramInfo_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.ProgramInfoPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "BFC.TL.Array.h"

#include "VMP.HLS.LevelInfoPtr.h"
#include "VMP.HLS.RootManifestPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL ProgramInfo : virtual public BFC::SObject {

public :

	/// \brief Creates a new ProgramInfo.

	ProgramInfo(
	);

	void mergeManifest(
			RootManifestPtr	pRootMfst
	);

	LevelInfoPtr getBestLevel(
	) const;

protected :

private :

	BFC::Buffer			mnfstUrl;
	BFC::Array< LevelInfoPtr >	levelLst;	///< Available levels.

	/// \brief Forbidden copy constructor.

	ProgramInfo(
		const	ProgramInfo &
	);

	/// \brief Forbidden copy operator.

	ProgramInfo & operator = (
		const	ProgramInfo &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_ProgramInfo_H_

// ============================================================================

