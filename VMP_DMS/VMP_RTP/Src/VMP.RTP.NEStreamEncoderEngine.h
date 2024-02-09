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
//	VMP.RTP.NEStreamEncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NEStreamEncoderEngine_H_
#define _VMP_RTP_NEStreamEncoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NEStreamEncoderEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"

#include "VMP.RTP.NFramePtr.h"
#include "VMP.RTP.TFramePtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NEStreamEncoderEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NEStreamEncoderEngine.

	NEStreamEncoderEngine(
	);

	virtual void putSessionCallback(
			SessionPtr	pInptSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pInptFrme
	);

	virtual void flushSessionCallback(
	);

protected :

	NFramePtr encodeTFrame(
			TFramePtr	pRtpTFrme
	);

private :

	/// \brief Forbidden copy constructor.

	NEStreamEncoderEngine(
		const	NEStreamEncoderEngine &
	);

	/// \brief Forbidden copy operator.

	NEStreamEncoderEngine & operator = (
		const	NEStreamEncoderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NEStreamEncoderEngine_H_

// ============================================================================

