// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTCPHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_RTP_RTCPHandler_H_
#define _VMP_RTP_RTCPHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

BFC_PTR_DECL( VMP_RTP_DLL, RTCPHandler )

} // namespace RTP
} // namespace VMP

// ============================================================================

#include "BFC.MT.Thread.h"

#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.UDPSocket.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

class VMP_RTP_DLL RTCPHandler : public BFC::MT::Thread {

public :

	RTCPHandler(
	);

	virtual ~RTCPHandler(
	);

	void open(
		const	BFC::Net::IPv4Address&	serverAddr,
		const	BFC::Uint16		serverPort,
		const	BFC::Uint16		clientPort
	);

	void open(
			BFC::Net::UDPSocketPtr	socket
	);

	void close(
	);

	void setSSRC(
		const	BFC::Uint32		ssrc	// NBO!!!!!!
	);

protected :

	virtual void run(
	);

	BFC::Buffer forgePacket(
	);

private :

	BFC::Net::UDPSocketPtr	socket;
	BFC::Uint32		ssrc;		// NBO!!!
	BFC::Uint32		self;

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	RTCPHandler(
		const	RTCPHandler&
	);

	RTCPHandler& operator = (
		const	RTCPHandler&
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_RTCPHandler_H_

// ============================================================================

