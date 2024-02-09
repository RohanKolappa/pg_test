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
//	VMP.HLS.PlayListWriter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_PlayListWriter_H_
#define _VMP_HLS_PlayListWriter_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.PlayListWriterPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.Frac64.h"

#include "BFC.Debug.DObject.h"

#include "BFC.IO.File.h"

#include "BFC.TL.Array.h"

#include "BFC.VFS.FSPath.h"

#include "VMP.HLS.PlayListFilePtr.h"

#include "VMP.L2.FinalPushEngine.h"

#include "VMP.SegUtils.SegmentSessionPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL PlayListWriter : public L2::FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new PlayListWriter.

	PlayListWriter(
	);

	/// \brief Destroys this object.

	virtual ~PlayListWriter(
	);

	void init(
		const	BFC::FSPath &	pOutptDir,
		const	BFC::Buffer &	pM3u8Name,
		const	BFC::Uint32	pPvrLngth,
		const	BFC::Frac64 &	pSegmDrtn,
		const	BFC::Buffer &	pHttpPrfx,
		const	BFC::Bool	pFakeClse,
		const	BFC::Bool	pSyncWrte
	);

	void getDiskStats(
			BFC::Uint64 &	pNmbrWrtn,
			BFC::Uint64 &	pNmbrDltd
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

protected :

private :

	PlayListFilePtr		playList;
	SegUtils::SegmentSessionPtr
				segmSess;

	/// \brief Forbidden copy constructor.

	PlayListWriter(
		const	PlayListWriter &
	);

	/// \brief Forbidden copy operator.

	PlayListWriter & operator = (
		const	PlayListWriter &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_PlayListWriter_H_

// ============================================================================

