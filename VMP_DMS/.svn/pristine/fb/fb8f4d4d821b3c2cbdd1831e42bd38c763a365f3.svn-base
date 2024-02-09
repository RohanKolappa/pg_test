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
//	VMP.AVC.FramerPushEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_FramerPushEngine_H_
#define _VMP_AVC_FramerPushEngine_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.FramerPushEnginePtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.AVC.NALUPtr.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL FramerPushEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new FramerPushEngine.

	FramerPushEngine(
	);

	/// \brief Destroys this object.

	virtual ~FramerPushEngine(
	);

	virtual void putSessionCallback(
			VMP::SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			VMP::FramePtr	pFrame
	);

protected :

private :

	BFC::Bool		syncOnKF;	///< Sync session on keyframe (i.e. drop non-kf at beginning of session).
//	BFC::Bool		useCfgRC;	///< Use config_record to send SPS/PPS ?

	BytesSessionPtr		iBytSess;
	VideoStreamPtr		iVidStrm;

	BFC::Bool		sentSess;
	BFC::Bool		seenKFrm;
	NALUCPtr		spsCache;
	NALUCPtr		ppsCache;

	BytesSessionPtr		oBytSess;
	VideoStreamPtr		oVidStrm;

	/// \brief Forbidden copy constructor.

	FramerPushEngine(
		const	FramerPushEngine &
	);

	/// \brief Forbidden copy operator.

	FramerPushEngine & operator = (
		const	FramerPushEngine &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_FramerPushEngine_H_

// ============================================================================

