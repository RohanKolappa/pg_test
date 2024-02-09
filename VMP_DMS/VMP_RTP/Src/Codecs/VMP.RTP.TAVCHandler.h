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
//	VMP.RTP.TAVCHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TAVCHandler_H_
#define _VMP_RTP_TAVCHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TAVCHandlerPtr.h"

// ============================================================================

#include "VMP.AVC.CodecConfigPtr.h"

#include "VMP.RTP.THandler.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TAVCHandler : public THandler {

public :

	/// \brief Creates a new TAVCHandler.

	TAVCHandler(
	);

	virtual void decodeMimeType(
		const	BFC::BufferArray &
					pMimeType
	);

	virtual void decodeFormat(
		const	BFC::Buffer &	pFmtpItem
	);

	virtual BFC::Uint32 getTimeScale(
	) const;

	virtual TEStreamDecoderEnginePtr makeTEStreamDecoderEngine(
	) const;

	virtual TEStreamEncoderEnginePtr makeTEStreamEncoderEngine(
	) const;

protected :

private :

	BFC::Uint32	packMode;	///< "packetization-mode=1"
	BFC::Buffer	profLevl;	///< "profile-level-id=4D4028"
	AVC::CodecConfigCPtr
			cnfgRcrd;	///< "sprop-parameter-sets=J01AKJpigPAET8uAtQEBAUAAAPoAADDUGhgAJmcAAKzNLvLjQwAEzOAAFZml3lwXosA=,KO48gA=="

	/// \brief Forbidden copy constructor.

	TAVCHandler(
		const	TAVCHandler &
	);

	/// \brief Forbidden copy operator.

	TAVCHandler & operator = (
		const	TAVCHandler &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TAVCHandler_H_

// ============================================================================

