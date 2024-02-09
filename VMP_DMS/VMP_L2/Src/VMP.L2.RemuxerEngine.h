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
//	VMP.L2.RemuxerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_RemuxerEngine_H_
#define _VMP_L2_RemuxerEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.RemuxerEnginePtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.L2.DemuxerFilterPtr.h"
#include "VMP.L2.EnmuxerCorePtr.h"
#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL RemuxerEngine : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new RemuxerEngine.

	RemuxerEngine(
	);

	EnmuxerCorePtr getEnmuxerCore(
	) {
		return muxrCore;
	}

	void addProcessor(
			DemuxerFilterPtr
					pStrmFltr,
			L2::ProxyPushEnginePtr
					pPeerEngn
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

protected :

	void createGraph(
			SessionPtr	pSession
	);

	void deleteGraph(
	);

private :

	EnmuxerCorePtr				muxrCore;
	BFC::Array< DemuxerFilterPtr >		fltrList;	///< Filters.
	BFC::Array< L2::ProxyPushEnginePtr >	engnList;	///< Potential processors (same size as fltrList).
	BFC::Array< BFC::Array< L2::ProxyPushEnginePtr > >
						procList;	///< Actual processors.
		
	/// \brief Forbidden copy constructor.

	RemuxerEngine(
		const	RemuxerEngine &
	);

	/// \brief Forbidden copy operator.

	RemuxerEngine & operator = (
		const	RemuxerEngine &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_RemuxerEngine_H_

// ============================================================================

