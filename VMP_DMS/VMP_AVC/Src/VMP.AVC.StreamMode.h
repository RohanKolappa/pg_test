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
//	VMP.AVC.StreamMode.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_StreamMode_H_
#define _VMP_AVC_StreamMode_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

enum StreamMode {

	/// \brief Use BytesFrames containing 1 or more NALU's prefixed by a
	///	0x00000001 start code.

	ModeAnnexB = 0,

	/// \brief Use BytesFrames containing 1 or more NALU's prefixed by
	///	their length as a BEUint32.

	ModePart15 = 1,

	/// \brief Use Frames.

	ModeNALU = 2

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_StreamMode_H_

// ============================================================================

