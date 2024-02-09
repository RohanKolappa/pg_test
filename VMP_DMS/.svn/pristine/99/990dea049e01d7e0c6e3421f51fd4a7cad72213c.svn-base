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
//	VMP.RTP.TEStreamClientEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TEStreamClientEngine_H_
#define _VMP_RTP_TEStreamClientEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TEStreamClientEnginePtr.h"

// ============================================================================

#include "BFC.Dyn.VBoolPtr.h"

#include "VMP.L2.ProxyPushEngine.h"

#include "VMP.RTP.NEStreamDecoderEnginePtr.h"
#include "VMP.RTP.TClockSharerPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TEStreamClientEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TEStreamClientEngine.

	TEStreamClientEngine(
	);

	/// \brief Destroys this object.

	virtual ~TEStreamClientEngine(
	);

	void setAllowMultipleSSRC(
		const	BFC::Bool	pMultSSRC
	);

	void setTClockSharer(
			TClockSharerPtr	pClckKper,
		const	BFC::Uint32	pClckPrio
	) {
		clckShrr = pClckKper;
		clckPrio = pClckPrio;
	}

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

private :

	BFC::Dyn::VBoolPtr		multSSRC;
	TClockSharerPtr			clckShrr;
	BFC::Uint32			clckPrio;
	NEStreamDecoderEnginePtr	nDecoder;

	/// \brief Forbidden copy constructor.

	TEStreamClientEngine(
		const	TEStreamClientEngine &
	);

	/// \brief Forbidden copy operator.

	TEStreamClientEngine & operator = (
		const	TEStreamClientEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TEStreamClientEngine_H_

// ============================================================================

