// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyPullEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProxyPullEngine_H_
#define _VMP_L2_ProxyPullEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProxyPullEnginePtr.h"

// ============================================================================

#include "VMP.L2.PullEngine.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all pull engines proxying another PullEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProxyPullEngine : public PullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	ProxyPullEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		PullEngine( pType ) {
	}

public :

	virtual void attachPeerEngine(
		const	BFC::Uint32		slot,
			PullEnginePtr		backend
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32		slot
	);

protected :

	PullEnginePtr getPeerEngine(
		const	BFC::Uint32		slot
	) const;

	virtual void setNbrInputs(
		const	BFC::Uint32		size
	);

private :

	BFC::Array< PullEnginePtr >	backends;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProxyPullEngine_H_

// ============================================================================

