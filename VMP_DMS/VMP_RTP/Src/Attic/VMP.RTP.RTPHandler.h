// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.RTPHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_RTP_RTPHandler_H_
#define _VMP_RTP_RTPHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

BFC_PTR_DECL( VMP_RTP_DLL, RTPHandler )

} // namespace RTP
} // namespace VMP

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Net.UDPSocket.h"

#include "VMP.Frame.h"

#include "VMP.RTP.RTCPHandler.h"
#include "VMP.RTP.TPacketPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

class VMP_RTP_DLL RTPHandler : virtual public BFC::DObject {

public :

	RTPHandler(
	);

	virtual ~RTPHandler(
	);

	void useRTCP(
			RTCPHandlerPtr		rtcpHandler
	);

	void open(
		const	BFC::Uint16		port
	);

	void open(
			BFC::Net::UDPSocketPtr	socket
	);

	void open(
		const	BFC::Net::IPv4Address&	addr,
		const	BFC::Uint16		port
	);

	void openMCast(
		const	BFC::Net::IPAddrCPtr &	addr,
		const	BFC::Uint16		port
	);

	void reset(
	);

	void close(
	);

	BytesFramePtr getFrame(
	);

	BFC::Buffer getBytes(
	);

	TPacketCPtr getPacket(
	);

	// Exceptions...

	BASE_CLASS_GEN_EXCEPTION( "VMP.RTPHandler" );

	CLASS_EXCEPTION( BrokenSession, "Broken session" );
	CLASS_EXCEPTION( EndOfStream, "End of stream" );
	CLASS_EXCEPTION( InternalError, "Internal error" );

protected :

private :

	BFC::Net::UDPSocketPtr	socket;

	BFC::Uint16		nextSeqNbr;
	BFC::Uint32		SSRC;
	BFC::Uint32		PT;

	RTCPHandlerPtr		rtcpHandler;

	RTPHandler(
		const	RTPHandler&
	);

	RTPHandler& operator = (
		const	RTPHandler&
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_RTPHandler_H_

// ============================================================================

