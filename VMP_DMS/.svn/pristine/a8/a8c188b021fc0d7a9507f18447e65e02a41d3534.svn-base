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
//	VMP.RTP.THandler.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_THandler_H_
#define _VMP_RTP_THandler_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.THandlerPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "BFC.TL.BufferArray.h"

#include "VMP.RTP.SEStreamPtr.h"
#include "VMP.RTP.TEStreamDecoderEnginePtr.h"
#include "VMP.RTP.TEStreamEncoderEnginePtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL THandler : virtual public BFC::DObject {

public :

	SEStreamCPtr getSEStream(
	) const {
		return sestream;
	}

	void setSEStream(
			SEStreamCPtr	pSEStream
	) {
		sestream = pSEStream;
	}

	virtual void decodeMimeType(
		const	BFC::BufferArray &
					pMimeType
	) = 0;

	virtual void decodeFormat(
		const	BFC::Buffer &	pFmtpItem
	) = 0;

	virtual BFC::Uint32 getTimeScale(
	) const = 0;

	virtual TEStreamDecoderEnginePtr makeTEStreamDecoderEngine(
	) const = 0;

	virtual TEStreamEncoderEnginePtr makeTEStreamEncoderEngine(
	) const = 0;

protected :

private :

	SEStreamCPtr	sestream;

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_THandler_H_

// ============================================================================

