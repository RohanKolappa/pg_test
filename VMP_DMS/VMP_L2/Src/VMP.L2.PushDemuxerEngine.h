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
//	VMP.L2.PushDemuxerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_PushDemuxerEngine_H_
#define _VMP_L2_PushDemuxerEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PushDemuxerEnginePtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.L2.DemuxerFilterPtr.h"
#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL PushDemuxerEngine : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new PushDemuxerEngine.

	PushDemuxerEngine(
	);

	/// \brief Destroys this object.

	virtual ~PushDemuxerEngine(
	);

	void addOutput(
			DemuxerFilterPtr
					pStrmFltr
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

private :

	BFC::Array< DemuxerFilterPtr >	outpFltr;
	BFC::Array< BFC::Uint32 >	outpMppr;	///< Mux subindex --> peer index.

	/// \brief Forbidden copy constructor.

	PushDemuxerEngine(
		const	PushDemuxerEngine &
	);

	/// \brief Forbidden copy operator.

	PushDemuxerEngine & operator = (
		const	PushDemuxerEngine &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PushDemuxerEngine_H_

// ============================================================================

