// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PushEngineProxy.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_PushEngineProxy_H_
#define _VMP_L2_PushEngineProxy_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PushEngineProxyPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "VMP.L2.PushEnginePtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all engines proxying a PushEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL PushEngineProxy : virtual public BFC::DObject {

public :

	virtual void attachPeerEngine(
		const	BFC::Uint32		slot,
			PushEnginePtr		backend
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32		slot
	);

protected :

	virtual PushEnginePtr getPeerEngine(
		const	BFC::Uint32		slot
	) const;

	virtual void setNbrOutputs(
		const	BFC::Uint32		size
	);

private :

	BFC::Array< PushEnginePtr >	peerList;

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PushEngineProxy_H_

// ============================================================================

