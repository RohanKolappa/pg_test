// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.FinalPushEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_FinalPushEngine_H_
#define _VMP_L2_FinalPushEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.FinalPushEnginePtr.h"

// ============================================================================

#include "VMP.L2.PushEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all push engines that are not proxying another
///	push engine (hint only).
///
/// \ingroup VMP_L2

class VMP_L2_DLL FinalPushEngine : public PushEngine {

protected :

	FinalPushEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		PushEngine( pType ) {
	}

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_FinalPushEngine_H_

// ============================================================================

