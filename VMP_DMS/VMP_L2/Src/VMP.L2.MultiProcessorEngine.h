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
//	VMP.L2.MultiProcessorEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_MultiProcessorEngine_H_
#define _VMP_L2_MultiProcessorEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.MultiProcessorEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPullEngine.h"

#include "VMP.L3.MultiProcessorBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine wrapping an L3::MultiProcessorBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL MultiProcessorEngine : public ProxyPullEngine {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new MultiProcessorEngine.

	MultiProcessorEngine(
			L3::MultiProcessorBackendPtr	pBackend
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		pIndex
	);

protected :

private :

	L3::MultiProcessorBackendPtr	backend;
	BFC::Uint32			nbrInputs;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_MultiProcessorEngine_H_

// ============================================================================

