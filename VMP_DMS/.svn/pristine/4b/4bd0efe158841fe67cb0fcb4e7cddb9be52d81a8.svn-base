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
//	VMP.RTP.TProgramServerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TProgramServerEngine_H_
#define _VMP_RTP_TProgramServerEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TProgramServerEnginePtr.h"

// ============================================================================

#include "BFC.Net.IPAddrPtr.h"

#include "BFC.TL.Array.h"

#include "VMP.L2.FinalPushEngine.h"

#include "VMP.RTP.TEStreamServerEnginePtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TProgramServerEngine : public L2::FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TProgramServerEngine.

	TProgramServerEngine(
	);

	/// \brief Destroys this object.

	virtual ~TProgramServerEngine(
	);

	void init(
			BFC::Net::IPAddrCPtr
					pOutpAddr,
		const	BFC::Uint16	pOVidPort,
		const	BFC::Uint16	pOAudPort
	) {
		outpAddr = pOutpAddr;
		oVidPort = pOVidPort;
		oAudPort = pOAudPort;
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

	TEStreamServerEnginePtr makeTEStreamServerEngine(
			BFC::Net::IPAddrCPtr
					pOutpAddr,
		const	BFC::Uint16	pOutpPort
	) const;

private :

	BFC::Net::IPAddrCPtr	outpAddr;
	BFC::Uint16		oVidPort;
	BFC::Uint16		oAudPort;
	BFC::Array< TEStreamServerEnginePtr >
				strmSrvr;

	/// \brief Forbidden copy constructor.

	TProgramServerEngine(
		const	TProgramServerEngine &
	);

	/// \brief Forbidden copy operator.

	TProgramServerEngine & operator = (
		const	TProgramServerEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TProgramServerEngine_H_

// ============================================================================

