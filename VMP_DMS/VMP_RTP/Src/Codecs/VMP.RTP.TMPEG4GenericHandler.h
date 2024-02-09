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
//	VMP.RTP.TMPEG4GenericHandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TMPEG4GenericHandler_H_
#define _VMP_RTP_TMPEG4GenericHandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TMPEG4GenericHandlerPtr.h"

// ============================================================================

#include "VMP.RTP.THandler.h"
#include "VMP.RTP.TMPEG4GenericContextPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TMPEG4GenericHandler : public THandler {

public :

	/// \brief Creates a new TMPEG4GenericHandler.

	TMPEG4GenericHandler(
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

	TMPEG4GenericContextPtr	codcCtxt;

	/// \brief Forbidden copy constructor.

	TMPEG4GenericHandler(
		const	TMPEG4GenericHandler &
	);

	/// \brief Forbidden copy operator.

	TMPEG4GenericHandler & operator = (
		const	TMPEG4GenericHandler &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TMPEG4GenericHandler_H_

// ============================================================================

