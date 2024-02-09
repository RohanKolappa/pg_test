// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::RTP".
// 
// "VMP::RTP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::RTP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::RTP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.TDriftRemoverEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TDriftRemoverEngine_H_
#define _VMP_RTP_TDriftRemoverEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TDriftRemoverEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"

#include "VMP.RTP.TClockSharerPtr.h"
#include "VMP.RTP.TSharedClockPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TDriftRemoverEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TDriftRemoverEngine.

	TDriftRemoverEngine(
	);

	void init(
		const	BFC::Uint32	pClckPrio,
			TClockSharerPtr	pClckShrr
	) {
		clckPrio = pClckPrio;
		clckShrr = pClckShrr;
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

	BFC::Uint32		clckPrio;
	TClockSharerPtr		clckShrr;
	TSharedClockPtr		shrdClck;

	/// \brief Forbidden copy constructor.

	TDriftRemoverEngine(
		const	TDriftRemoverEngine &
	);

	/// \brief Forbidden copy operator.

	TDriftRemoverEngine & operator = (
		const	TDriftRemoverEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TDriftRemoverEngine_H_

// ============================================================================

