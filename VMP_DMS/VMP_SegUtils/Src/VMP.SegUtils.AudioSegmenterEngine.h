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
//	VMP.SegUtils.AudioSegmenterEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_AudioSegmenterEngine_H_
#define _VMP_SegUtils_AudioSegmenterEngine_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.AudioSegmenterEnginePtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL AudioSegmenterEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new AudioSegmenterEngine.

	AudioSegmenterEngine(
	);

	/// \brief Destroys this object.

	virtual ~AudioSegmenterEngine(
	);

	void init(
		const	BFC::Frac64 &	pSegmDrtn
	) {
		segmDrtn = BFC::Time::Delta( ( BFC::Int64 )( pSegmDrtn.toDouble() * 1000000 ) );
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

private :

	BFC::Time::Delta	segmDrtn;	///< Segment duration.
	BFC::Time::Clock	segmTime;	///< DTS of first frame in current segment.
	BFC::Uint64		segmOffs;	///< Unit offset of first frame of current segment.

	/// \brief Forbidden copy constructor.

	AudioSegmenterEngine(
		const	AudioSegmenterEngine &
	);

	/// \brief Forbidden copy operator.

	AudioSegmenterEngine & operator = (
		const	AudioSegmenterEngine &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_AudioSegmenterEngine_H_

// ============================================================================

