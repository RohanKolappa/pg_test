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
//	VMP.HLS.PlayListFile.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_HLS_PlayListFile_H_
#define _VMP_HLS_PlayListFile_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.PlayListFilePtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.TL.BufferArray.h"

#include "BFC.Time.Clock.h"

#include "VMP.SegUtils.SegmentFramePtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL PlayListFile : virtual public BFC::DObject {

protected :

	PlayListFile(
	);

public :

	virtual void create(
	) = 0;

	virtual void update(
			SegUtils::SegmentFrameCPtr
					pSegmFrme
	) = 0;

	virtual void close(
	) = 0;

	virtual void getDiskStats(
			BFC::Uint64 &	pNmbrWrtn,
			BFC::Uint64 &	pNmbrDltd
	) = 0;

protected :

	BFC::BufferArray getHeaderList(
	) const;

private :

	BFC::Time::Clock	strtTime;

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_PlayListFile_H_

// ============================================================================

