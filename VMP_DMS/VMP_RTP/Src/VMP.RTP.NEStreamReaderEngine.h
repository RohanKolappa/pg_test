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
//	VMP.RTP.NEStreamReaderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NEStreamReaderEngine_H_
#define _VMP_RTP_NEStreamReaderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NEStreamReaderEnginePtr.h"

// ============================================================================

#include "BFC.Net.NetIfacePtr.h"
#include "BFC.Net.SockAddrPtr.h"
#include "BFC.Net.UDPSocketPtr.h"

#include "VMP.L2.FinalPullEngine.h"

#include "VMP.RTP.NSessionPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NEStreamReaderEngine : public L2::FinalPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NEStreamReaderEngine.

	NEStreamReaderEngine(
	);

	void init(
			BFC::Net::SockAddrCPtr
					pSockAddr,
			NSessionPtr	pONetSess,
			BFC::Net::NetIfacePtr
					pNetIface = BFC::Net::NetIfacePtr()
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64	index
	);

protected :

private :

	BFC::Net::SockAddrCPtr	sockAddr;
	NSessionPtr		oNetSess;
	BFC::Net::NetIfacePtr	netIface;
	BFC::Net::UDPSocketPtr	inptSock;

	/// \brief Forbidden copy constructor.

	NEStreamReaderEngine(
		const	NEStreamReaderEngine &
	);

	/// \brief Forbidden copy operator.

	NEStreamReaderEngine & operator = (
		const	NEStreamReaderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NEStreamReaderEngine_H_

// ============================================================================

