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
// FastFilename:
//	VMP.HLS.PlayListFastFile.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_HLS_PlayListFastFile_H_
#define _VMP_HLS_PlayListFastFile_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.PlayListFastFilePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.Frac64.h"

#include "BFC.IO.DevicePtr.h"

#include "BFC.TL.Array.h"

#include "BFC.VFS.FSPath.h"

#include "VMP.HLS.PlayListFile.h"

#include "VMP.SegUtils.SegmentFramePtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL PlayListFastFile : public PlayListFile {

public :

	/// \brief Creates a new PlayListFastFile.

	PlayListFastFile(
		const	BFC::FSPath &	pOutptDir,
		const	BFC::Buffer &	pM3u8Name,
		const	BFC::Frac64 &	pTrgtDrtn,
		const	BFC::Buffer &	pHttpPrfx,
		const	BFC::Bool	pFakeClse,
		const	BFC::Bool	pSyncWrte
	);

	/// \brief Destroys this object.

	virtual ~PlayListFastFile(
	);

	virtual void create(
	);

	virtual void update(
			SegUtils::SegmentFrameCPtr
					pSegmFrme
	);

	virtual void close(
	);

	virtual void getDiskStats(
			BFC::Uint64 &	pNmbrWrtn,
			BFC::Uint64 &	pNmbrDltd
	);

protected :

private :

	BFC::FSPath		outptDir;	///< Output directory (for both playlist and segments).
	BFC::Buffer		m3u8Name;	///< Playlist output file name.
	BFC::FSPath		m3u8Path;	///< Playlist output file path.
	BFC::Frac64		trgtDrtn;
	BFC::Buffer		httpPrfx;
	BFC::Bool		fakeClse;
	BFC::Uint64		eventPos;	///< Where we wrote "EVENT" if ! fakeClse.
	BFC::Bool		syncWrte;
	BFC::FastCS		critSect;
	BFC::Uint64		nmbrWrtn;
	BFC::Uint64		nmbrDltd;
	BFC::IO::DevicePtr	m3u8File;

	/// \brief Forbidden default constructor.

	PlayListFastFile(
	);

	/// \brief Forbidden copy constructor.

	PlayListFastFile(
		const	PlayListFastFile &
	);

	/// \brief Forbidden copy operator.

	PlayListFastFile & operator = (
		const	PlayListFastFile &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_PlayListFastFile_H_

// ============================================================================

