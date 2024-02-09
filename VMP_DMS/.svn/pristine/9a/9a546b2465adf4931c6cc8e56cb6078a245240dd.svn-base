// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTSPHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_RTP_RTSPHandler_H_
#define _VMP_RTP_RTSPHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

BFC_PTR_DECL( VMP_RTP_DLL, RTSPHandler )

} // namespace RTP
} // namespace VMP

// ============================================================================

#include "BFC.Base.Exception.h"
#include "BFC.Base.FastCS.h"

#include "BFC.Debug.DObject.h"

#include "BFC.Net.TCPSocket.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.BytesSessionPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

class VMP_RTP_DLL RTSPHandler : virtual public BFC::DObject {

public :

	RTSPHandler(
	);

	virtual ~RTSPHandler(
	);

	BytesSessionPtr open(
		const	BFC::Buffer &			rtspUrl,
			BFC::Net::UDPSocketPtr &	rtpSocket,
			BFC::Net::UDPSocketPtr &	rtcpSocket
	);

	void close(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.RTSPHandler" );

	CLASS_EXCEPTION( InvalidData, "Invalid data" );

protected :

	void parseSetupAnswer(
		const	BFC::Buffer &		buffer,
			BFC::Uint16 &		srvPort0,
			BFC::Uint16 &		srvPort1,
			BFC::Uint16 &		cltPort0,
			BFC::Uint16 &		cltPort1
	);

	BytesSessionPtr parseDescribeAnswer(
		const	BFC::Buffer &		buffer
	);

private :

	static const BFC::Uint16	RTSPDefaultPort;

	BFC::Net::TCPSocketPtr	socket;

	BFC::Buffer		tagSession;

	static BFC::FastCS	cltLock;	///< Protect cltPort.
	static BFC::Uint16	cltPort;	///< Monotically incremented.

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	RTSPHandler(
		const	RTSPHandler&
	);

	RTSPHandler& operator = (
		const	RTSPHandler&
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_RTSPHandler_H_

// ============================================================================

