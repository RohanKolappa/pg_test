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
//	VMP.SegUtils.VideoSegmenterEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_VideoSegmenterEngine_H_
#define _VMP_SegUtils_VideoSegmenterEngine_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.VideoSegmenterEnginePtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL VideoSegmenterEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new VideoSegmenterEngine.

	VideoSegmenterEngine(
	);

	/// \brief Destroys this object.

	virtual ~VideoSegmenterEngine(
	);

	void init(
		const	BFC::Frac64 &	pSegmDrtn
	) {
		segmDrtn = pSegmDrtn;
	}

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

	void computeRates(
			FramePtr	pFrame
	);

private :

	BFC::Bool	forceCut;	///< Force cut, even if not at keyframe ?
	BFC::Frac64	segmDrtn;	///< Segment duration (in seconds).

	// To estimate the framerate...

	BFC::Bool		gotFRate;
	BFC::Bool		gotFirst;
	BFC::Bool		frstAKey;
	BFC::Uint64		frstOffs;
	BFC::Time::Clock	frstTime;
	BFC::Uint64		prevOffs;

	BFC::Frac64		frmeRate;	///< Given or computed frame rate.
	BFC::Uint64		frmPrSeg;	///< Max number of frames per segment.



	BFC::Uint64	segmStrt;	///< Unit offset of first frame of current segment.
	BFC::Uint64	estGopSz;	///< Estimated GOP size.
	BFC::Uint64	lastKFrm;	///< Unit offset of last keyframe.

	/// \brief Forbidden copy constructor.

	VideoSegmenterEngine(
		const	VideoSegmenterEngine &
	);

	/// \brief Forbidden copy operator.

	VideoSegmenterEngine & operator = (
		const	VideoSegmenterEngine &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_VideoSegmenterEngine_H_

// ============================================================================

