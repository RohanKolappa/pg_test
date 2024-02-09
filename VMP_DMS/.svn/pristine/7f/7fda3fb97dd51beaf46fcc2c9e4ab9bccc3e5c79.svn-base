// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyPushEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProxyPushEngine_H_
#define _VMP_L2_ProxyPushEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProxyPushEnginePtr.h"

// ============================================================================

#include "VMP.L2.PushEngine.h"
#include "VMP.L2.PushEngineProxy.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all push engines proxying another PushEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProxyPushEngine :

	public PushEngine,
	public PushEngineProxy {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	ProxyPushEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		PushEngine( pType ) {
	}

	virtual void setNbrOutputs(
		const	BFC::Uint32		size
	);

public :

	virtual void flushSessionCallback(
	);

private :

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProxyPushEngine_H_

// ============================================================================

