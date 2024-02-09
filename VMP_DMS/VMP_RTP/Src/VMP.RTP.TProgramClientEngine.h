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
//	VMP.RTP.TProgramClientEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TProgramClientEngine_H_
#define _VMP_RTP_TProgramClientEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TProgramClientEnginePtr.h"

// ============================================================================

#include "BFC.Dyn.VBoolPtr.h"

#include "BFC.TL.Array.h"

#include "VMP.L2.ProxyPushEngine.h"
#include "VMP.L2.PushEnginePtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TProgramClientEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TProgramClientEngine.

	TProgramClientEngine(
	);

	/// \brief Destroys this object.

	virtual ~TProgramClientEngine(
	);

	void setAllowMultipleSSRC(
		const	BFC::Bool	pMultSSRC
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

	BFC::Dyn::VBoolPtr		multSSRC;
	BFC::Array< L2::PushEnginePtr >	peerList;

	/// \brief Forbidden copy constructor.

	TProgramClientEngine(
		const	TProgramClientEngine &
	);

	/// \brief Forbidden copy operator.

	TProgramClientEngine & operator = (
		const	TProgramClientEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TProgramClientEngine_H_

// ============================================================================

