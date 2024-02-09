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
//	VMP.RTP.NProgramDecoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NProgramDecoderEngine_H_
#define _VMP_RTP_NProgramDecoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NProgramDecoderEnginePtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "VMP.L2.EnmuxerCorePtr.h"
#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief Decodes a mux of NSession into a mux of TSession.
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NProgramDecoderEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NProgramDecoderEngine.

	NProgramDecoderEngine(
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

	L2::EnmuxerCorePtr			muxrCore;
	BFC::Uint32				nbrComps;
	BFC::Array< L2::ProxyPushEnginePtr >	dcdrList;	///< Actual decoders.
	BFC::Array< L2::ProxyPushEnginePtr >	muxrList;	///< Actual enmuxers.
		
	/// \brief Forbidden copy constructor.

	NProgramDecoderEngine(
		const	NProgramDecoderEngine &
	);

	/// \brief Forbidden copy operator.

	NProgramDecoderEngine & operator = (
		const	NProgramDecoderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NProgramDecoderEngine_H_

// ============================================================================

