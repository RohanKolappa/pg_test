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
//	VMP.RTP.TProgramPrinterEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TProgramPrinterEngine_H_
#define _VMP_RTP_TProgramPrinterEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TProgramPrinterEnginePtr.h"

// ============================================================================

#include "BFC.Time.Clock.h"

#include "VMP.L2.FinalPushEngine.h"

#include "VMP.RTP.TFramePtr.h"
#include "VMP.RTP.TSessionPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TProgramPrinterEngine : public L2::FinalPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TProgramPrinterEngine.

	TProgramPrinterEngine(
		const	BFC::Bool	pDumpSess = false,
		const	BFC::Bool	pDumpPckt = true,
		const	BFC::Bool	pPrtHrtBt = false
	);

	virtual void putSessionCallback(
			SessionPtr	pOutpSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pOutpFrme
	);

protected :

	virtual void printTSession(
		const	BFC::Uint32	pStrIndex,
			TSessionCPtr	pRtpTSess
	);

	virtual void printTFrame(
		const	BFC::Uint32	pStrIndex,
			TFrameCPtr	pRtpTFrme
	);

private :

	BFC::Bool		dumpSess;
	BFC::Bool		dumpPckt;
	BFC::Bool		prtHrtBt;
	BFC::Time::Clock	lstHrtBt;

	/// \brief Forbidden copy constructor.

	TProgramPrinterEngine(
		const	TProgramPrinterEngine &
	);

	/// \brief Forbidden copy operator.

	TProgramPrinterEngine & operator = (
		const	TProgramPrinterEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TProgramPrinterEngine_H_

// ============================================================================

