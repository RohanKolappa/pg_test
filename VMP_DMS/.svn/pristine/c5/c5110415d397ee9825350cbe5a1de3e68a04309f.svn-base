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
//	VMP.RTP.WriterBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_WriterBackend_H_
#define _VMP_RTP_WriterBackend_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.Dyn.VBuffer.h"
#include "BFC.Dyn.VSignal.h"

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"

//#include "BFC.Net.IPv4Address.h"
#include "BFC.Net.UDPSocket.h"

#include "VMP.L3.ConsumerBackend.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL WriterBackend : public L3::ConsumerBackend {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new WriterBackend.

	WriterBackend(
	);

	/// \brief Destroys this object.

	virtual ~WriterBackend(
	);

	virtual void initSession(
			SessionPtr	pSession
	);

	virtual void exitSession(
	);

	virtual void putFrame(
			FramePtr	pFrame
	);

protected :

	virtual void initUICallback(
			UI::IElementPtr	pUserInterface
	);

	void resetUI(
	);

	void buttonClickedCallback(
			BFC::Msg::EventCPtr
	);

private :

//	BFC::Net::IPv4Address	addr;
//	BFC::Uint16		port;
	BFC::Net::UDPSocketPtr	socket;
	BFC::Uint16		index;

//	RTPHandlerPtr		rtpHandler;
//	RTCPHandlerPtr		rtcpHandler;
//	RTSPHandlerPtr		rtspHandler;
//
//	BFC::Bool		valid;
	BFC::MT::Mutex		mutex;
	BFC::MT::Condition		cond;
//	BytesSessionPtr		session;

	BFC::Dyn::VBufferPtr	dynRtspBuf;
	BFC::Dyn::VBufferPtr	dynAddrBuf;
	BFC::Dyn::VBufferPtr	dynPortBuf;
//	UI::ILineEditPtr	rtspLineEdit;
//	UI::ILineEditPtr	addrLineEdit;
//	UI::ILineEditPtr	portLineEdit;

	BFC::Dyn::VSignalPtr	dynButtSgn;
//	UI::IPushButtonPtr	connectPushButton;

	BFC::Dyn::VBufferPtr	dynPtBuffr;
	BFC::Dyn::VBufferPtr	dynSsrcBuf;
	BFC::Dyn::VBufferPtr	dynCsrcBuf;
//	UI::ITextLabelPtr	ptTextLabel;
//	UI::ITextLabelPtr	ssrcTextLabel;
//	UI::ITextLabelPtr	csrcTextLabel;

	/// \brief Forbidden copy constructor.

	WriterBackend(
		const	WriterBackend &
	);

	/// \brief Forbidden copy operator.

	WriterBackend & operator = (
		const	WriterBackend &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_WriterBackend_H_

// ============================================================================

