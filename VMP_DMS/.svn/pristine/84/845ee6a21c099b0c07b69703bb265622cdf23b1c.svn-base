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
//	VMP.SegUtils.SegmentWriter.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_SegmentWriter_H_
#define _VMP_SegUtils_SegmentWriter_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.SegmentWriterPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.FastCS.h"
#include "BFC.Base.Frac64.h"

#include "BFC.IO.FilePtr.h"

#include "BFC.Time.Clock.h"

#include "BFC.VFS.FSPath.h"

#include "VMP.L2.ProxyPushEngine.h"

#include "VMP.SegUtils.PathComputerPtr.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL SegmentWriter : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new SegmentWriter.

	SegmentWriter(
	);

	/// \brief Destroys this object.

	virtual ~SegmentWriter(
	);

	void init(
			PathComputerCPtr
					pPathComp,
		const	BFC::Frac64 &	pSegmDrtn,
		const	BFC::Uint64	pFrstIndx,
		const	BFC::Bool	pSncWrite
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	BFC::Uint64 getDiskStats(
	);

protected :

	void startFile(
		const	BFC::Time::Clock &
					pTimeStmp
	);

	void closeFile(
		const	BFC::Time::Clock &
					pTimeStmp
	);

	void writeFile(
		const	BFC::Buffer &	pSegmData
	);

private :

	static const BFC::Uint32	CcheMxSz = ( 1 << 20 );

	PathComputerCPtr	pathComp;
	BFC::Frac64		segmDrtn;	///< Theoretical max segment duration (in seconds).
	BFC::Uint64		currIndx;
	BFC::Bool		sncWrite;
	BFC::FSPath		filePath;
	BFC::Buffer		fileName;
	BFC::Buffer		fullName;
	BFC::IO::FilePtr	outpFile;	///< Current output file.
	BFC::Time::Clock	frstTime;	///< Timestamp of first written packet.
	BFC::Time::Clock	lastTime;	///< Timestamp of last written packet.
	BFC::FastCS		critSect;	///< Protects 'nmbrWrtn'.
	BFC::Uint64		nmbrWrtn;
	BFC::Buffer		fileCche;

	/// \brief Forbidden copy constructor.

	SegmentWriter(
		const	SegmentWriter &
	);

	/// \brief Forbidden copy operator.

	SegmentWriter & operator = (
		const	SegmentWriter &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_SegmentWriter_H_

// ============================================================================

