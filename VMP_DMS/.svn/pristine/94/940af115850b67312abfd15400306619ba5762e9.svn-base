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
//	VMP.RTP.NEStreamDecoderWorker.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_NEStreamDecoderWorker_H_
#define _VMP_RTP_NEStreamDecoderWorker_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.NEStreamDecoderWorkerPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "VMP.RTP.NFramePtr.h"
#include "VMP.RTP.TFramePtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL NEStreamDecoderWorker : virtual public BFC::DObject {

public :

	/// \brief Creates a new NEStreamDecoderWorker.

	NEStreamDecoderWorker(
	);

	TFramePtr decodeNFrame(
			NFramePtr	pNetFrame
	);

protected :

private :

	/// \brief Forbidden copy constructor.

	NEStreamDecoderWorker(
		const	NEStreamDecoderWorker &
	);

	/// \brief Forbidden copy operator.

	NEStreamDecoderWorker & operator = (
		const	NEStreamDecoderWorker &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_NEStreamDecoderWorker_H_

// ============================================================================

