// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP".
// 
// "VMP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.EncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Stijn LAST
// 
// ============================================================================

#ifndef _VMP_RTP_EncoderEngine_H_
#define _VMP_RTP_EncoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.L2.FinalPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief FinalPushEngine converting a bytes stream into an RTP stream.
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL EncoderEngine : public L2::FinalPushEngine {

public :

	EncoderEngine(
		const	BFC::Net::IPv4Address &	addr,
		const	BFC::Uint16		port
	);

	virtual ~EncoderEngine(
	);

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	BFC::Net::IPv4Address	addr;
	BFC::Uint16		port;
	BFC::Net::UDPSocketPtr	socket;
	BFC::Uint16		index;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	EncoderEngine(
		const	EncoderEngine&
	);

	EncoderEngine& operator = (
		const	EncoderEngine&
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif

// ============================================================================

