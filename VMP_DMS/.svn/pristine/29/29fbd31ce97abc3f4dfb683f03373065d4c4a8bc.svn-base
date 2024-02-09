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
//	VMP.RTP.TAVCEncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TAVCEncoderEngine_H_
#define _VMP_RTP_TAVCEncoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TAVCEncoderEnginePtr.h"

// ============================================================================

#include "VMP.RTP.TEStreamEncoderEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TAVCEncoderEngine : public TEStreamEncoderEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TAVCEncoderEngine.

	TAVCEncoderEngine(
	);

	virtual void putSessionCallback(
			SessionPtr	pInptSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pInptFrme
	);

	virtual void flushSessionCallback(
	);

protected :

private :

	static const BFC::Uchar TypeStapA	= 0x18;
	static const BFC::Uchar TypeStapB	= 0x19;
	static const BFC::Uchar TypeMtap16	= 0x1A;
	static const BFC::Uchar TypeMtap24	= 0x1B;
	static const BFC::Uchar TypeFuA		= 0x1C;
	static const BFC::Uchar TypeFuB		= 0x1D;

	BFC::Uint64		frmeOffs;
	BFC::Uint64		unitOffs;

	/// \brief Forbidden copy constructor.

	TAVCEncoderEngine(
		const	TAVCEncoderEngine &
	);

	/// \brief Forbidden copy operator.

	TAVCEncoderEngine & operator = (
		const	TAVCEncoderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TAVCEncoderEngine_H_

// ============================================================================

