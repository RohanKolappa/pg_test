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
//	VMP.L2.ProducerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProducerEngine_H_
#define _VMP_L2_ProducerEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProducerEnginePtr.h"

// ============================================================================

#include "VMP.L2.FinalPullEngine.h"

#include "VMP.L3.ProducerBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief FinalPullEngine wrapping an L3::ProducerBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProducerEngine : public FinalPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new ProducerEngine.

	ProducerEngine(
			L3::ProducerBackendPtr	pBackend
	);

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64	index
	);

private :

	L3::ProducerBackendPtr backend;
		// to avoid having to downcast getBackend()

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProducerEngine_H_

// ============================================================================

