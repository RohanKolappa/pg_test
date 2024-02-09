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
//	VMP.AVC.Any2NALUFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_Any2NALUFramer_H_
#define _VMP_AVC_Any2NALUFramer_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.Any2NALUFramerPtr.h"

// ============================================================================

#include "VMP.AVC.FramePtr.h"
#include "VMP.AVC.StreamMode.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL Any2NALUFramer : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new Any2NALUFramer.

	Any2NALUFramer(
	);

	/// \brief Destroys this object.

	virtual ~Any2NALUFramer(
	);

	virtual void putSessionCallback(
			VMP::SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			VMP::FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

	void parseAnnexB(
			VMP::FramePtr	pInptFrme,
			BFC::Array< FramePtr > &
					pOutpFrme
	);

	void parsePart15(
			VMP::FramePtr	pInptFrme,
			BFC::Array< FramePtr > &
					pOutpFrme
	);

private :

	StreamMode		mode;

	/// \brief Forbidden copy constructor.

	Any2NALUFramer(
		const	Any2NALUFramer &
	);

	/// \brief Forbidden copy operator.

	Any2NALUFramer & operator = (
		const	Any2NALUFramer &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_Any2NALUFramer_H_

// ============================================================================

