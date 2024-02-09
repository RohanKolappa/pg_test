// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.FinalPullEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_FinalPullEngine_H_
#define _VMP_L2_FinalPullEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.FinalPullEnginePtr.h"

// ============================================================================

#include "VMP.L2.PullEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all pull engines that are not proxying another
///	pull engine (hint only).
///
/// \ingroup VMP_L2

class VMP_L2_DLL FinalPullEngine : public PullEngine {

public :

	FinalPullEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		PullEngine( pType ) {
	}

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_FinalPullEngine_H_

// ============================================================================

