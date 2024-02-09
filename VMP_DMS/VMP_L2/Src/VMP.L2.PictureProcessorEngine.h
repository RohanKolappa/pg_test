// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
//
// Filename:
//	VMP.L2.PictureProcessorEngine.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_L2_PictureProcessorEngine_H_
#define _VMP_L2_PictureProcessorEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PictureProcessorEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPullEngine.h"

#include "VMP.L3.PictureProcessorBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine wrapping an L3::PictureProcessorBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL PictureProcessorEngine : public ProxyPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	PictureProcessorEngine(
			L3::PictureProcessorBackendPtr	backend
	);

	// Implementation of the ProxyPullEngine interface...

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

protected :

private :

	L3::PictureProcessorBackendPtr	backend;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PictureProcessorEngine_H_

// ============================================================================

