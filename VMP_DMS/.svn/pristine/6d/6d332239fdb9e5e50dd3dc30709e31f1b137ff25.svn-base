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
//	VMP.SegUtils.MuxedSynchronizer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_SegUtils_MuxedSynchronizer_H_
#define _VMP_SegUtils_MuxedSynchronizer_H_

// ============================================================================

#include "VMP.SegUtils.DLL.h"

// ============================================================================

#include "VMP.SegUtils.MuxedSynchronizerPtr.h"

// ============================================================================

#include "BFC.Base.Frac64.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Delta.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace SegUtils {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_SegUtils

class VMP_SegUtils_DLL MuxedSynchronizer : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new MuxedSynchronizer.

	MuxedSynchronizer(
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

protected :

private :

	BFC::Time::Delta	segmDrtn;	///< Segment duration.
	BFC::Uint32		compIndx;	///< Component to use as segmentation reference.
	BFC::Bool		frstStrt;	///< Did we see a first segment start ?
	BFC::Array< FramePtr >	currFrme;	///< Records the first slave frames received after the last master frame.

	/// \brief Forbidden copy constructor.

	MuxedSynchronizer(
		const	MuxedSynchronizer &
	);

	/// \brief Forbidden copy operator.

	MuxedSynchronizer & operator = (
		const	MuxedSynchronizer &
	);

};

// ============================================================================

} // namespace SegUtils
} // namespace VMP

// ============================================================================

#endif // _VMP_SegUtils_MuxedSynchronizer_H_

// ============================================================================

