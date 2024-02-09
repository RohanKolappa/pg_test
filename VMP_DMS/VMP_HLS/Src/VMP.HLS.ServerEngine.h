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
//	VMP.HLS.ServerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_HLS_ServerEngine_H_
#define _VMP_HLS_ServerEngine_H_

// ============================================================================

#include "VMP.HLS.DLL.h"

// ============================================================================

#include "VMP.HLS.ServerEnginePtr.h"

// ============================================================================

#include "VMP.HLS.PlayListWriterPtr.h"
#include "VMP.HLS.ServerConfigPtr.h"

#include "VMP.L2.FinalPushEngine.h"
#include "VMP.L2.RemuxerEnginePtr.h"

#include "VMP.M2S.TSEncoderEnginePtr.h"

#include "VMP.SegUtils.MuxedSynchronizerPtr.h"
#include "VMP.SegUtils.SegmentWriterPtr.h"

// ============================================================================

namespace VMP {
namespace HLS {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_HLS

class VMP_HLS_DLL ServerEngine : public L2::FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new ServerEngine.

	ServerEngine(
	);

	/// \brief Destroys this object.

	virtual ~ServerEngine(
	);

	void init(
			ServerConfigCPtr
					pCnfgData
	);

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

	void getDiskStats(
			BFC::Uint64 &	pNmbrWrtn,
			BFC::Uint64 &	pNmbrDltd
	);

protected :

private :

	ServerConfigCPtr		cnfgData;

	L2::RemuxerEnginePtr		rmxrEngn;	///< To process the video stream only.
	SegUtils::MuxedSynchronizerPtr	syncEngn;
	M2S::TSEncoderEnginePtr		m2tsEngn;
	SegUtils::SegmentWriterPtr	wrtrEngn;
	PlayListWriterPtr		mngrEngn;

	/// \brief Forbidden copy constructor.

	ServerEngine(
		const	ServerEngine &
	);

	/// \brief Forbidden copy operator.

	ServerEngine & operator = (
		const	ServerEngine &
	);

};

// ============================================================================

} // namespace HLS
} // namespace VMP

// ============================================================================

#endif // _VMP_HLS_ServerEngine_H_

// ============================================================================

