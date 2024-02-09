// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
/// ============================================================================
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
//	VMP.RTP.ReaderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_ReaderEngine_H_
#define _VMP_RTP_ReaderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "BFC.MT.Condition.h"
#include "BFC.MT.Mutex.h"

#include "VMP.L2.FinalPullEngine.h"

#include "VMP.UI.ILineEdit.h"
#include "VMP.UI.IPushButton.h"
#include "VMP.UI.ITextLabel.h"

#include "VMP.RTP.RTCPHandler.h"
#include "VMP.RTP.RTPHandler.h"
#include "VMP.RTP.RTSPHandler.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

class VMP_RTP_DLL ReaderEngine : public L2::FinalPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	ReaderEngine(
	);

	virtual ~ReaderEngine(
	);

	virtual void initUICallback(
			UI::IElementPtr		/* rootUI */
	);

	/// to process data coming from active ports...

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

	virtual DynVar getUserSettings(
	);

	virtual void setUserSettings(
		const	DynVar&			settings
	);

protected :

	void resetUI(
	);

	void buttonClickedCallback(
			BFC::Msg::EventCPtr
	);

private :

	RTPHandlerPtr		rtpHandler;
	RTCPHandlerPtr		rtcpHandler;
	RTSPHandlerPtr		rtspHandler;

	BFC::Bool		valid;
	BFC::MT::Mutex		mutex;
	BFC::MT::Condition	cond;
	BytesSessionPtr		session;

	UI::ILineEditPtr	rtspLineEdit;
	UI::ILineEditPtr	addrLineEdit;
	UI::ILineEditPtr	portLineEdit;
	UI::ILineEditPtr	widthLineEdit;	///< Width hint input.
	UI::ILineEditPtr	heightLineEdit;	///< Height hint input.
	UI::ILineEditPtr	frateLineEdit;	///< Frame rate hint input.
	UI::IPushButtonPtr	connectPushButton;
	UI::ITextLabelPtr	ptTextLabel;
	UI::ITextLabelPtr	ssrcTextLabel;
	UI::ITextLabelPtr	csrcTextLabel;

	/// \brief Forbidden copy constructor.

	ReaderEngine(
		const	ReaderEngine &
	);

	/// \brief Forbidden copy operator.

	ReaderEngine& operator = (
		const	ReaderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_ReaderEngine_H_

// ============================================================================

