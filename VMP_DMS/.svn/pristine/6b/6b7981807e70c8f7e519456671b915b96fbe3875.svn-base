// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.EnmuxerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_EnmuxerEngine_H_
#define _VMP_L2_EnmuxerEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.EnmuxerEnginePtr.h"

// ============================================================================

#include "VMP.L2.EnmuxerCore.h"
#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL EnmuxerEngine : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new EnmuxerEngine.

	EnmuxerEngine(
	);

	EnmuxerEngine(
			EnmuxerEnginePtr
					pMuxrEngn
	);

	EnmuxerEngine(
			EnmuxerCorePtr	pMuxrCore
	);

	/// \brief Destroys this object.

	virtual ~EnmuxerEngine(
	);

	virtual void attachPeerEngine(
		const	BFC::Uint32	pPeerIndx,
			PushEnginePtr	pPeerEngn
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32	pPeerIndx
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	EnmuxerCorePtr getEnmuxerCore(
	) {
		return muxrCore;
	}

	EnmuxerCoreCPtr getEnmuxerCore(
	) const {
		return muxrCore;
	}

protected :

	void privInit(
			EnmuxerCorePtr	pMuxrCore
	);

	void privExit(
	);

private :

	PushEnginePtr		peerEngn;
	EnmuxerCorePtr		muxrCore;
	BFC::Uint32		muxrSlot;

	/// \brief Forbidden copy constructor.

	EnmuxerEngine(
		const	EnmuxerEngine &
	);

	/// \brief Forbidden copy operator.

	EnmuxerEngine & operator = (
		const	EnmuxerEngine &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_EnmuxerEngine_H_

// ============================================================================

