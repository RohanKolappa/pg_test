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
//	VMP.RTP.TLATMHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TLATMHandler_H_
#define _VMP_RTP_TLATMHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TLATMHandlerPtr.h"

// ============================================================================

#include "VMP.RTP.THandler.h"
#include "VMP.RTP.TLATMContextPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TLATMHandler : public THandler {

public :

	/// \brief Creates a new TLATMHandler.

	TLATMHandler(
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

	TLATMContextPtr	codcCtxt;

	/// \brief Forbidden copy constructor.

	TLATMHandler(
		const	TLATMHandler &
	);

	/// \brief Forbidden copy operator.

	TLATMHandler & operator = (
		const	TLATMHandler &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TLATMHandler_H_

// ============================================================================

