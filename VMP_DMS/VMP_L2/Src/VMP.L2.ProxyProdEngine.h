// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.ProxyProdEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProxyProdEngine_H_
#define _VMP_L2_ProxyProdEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProxyProdEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProdEngine.h"
#include "VMP.L2.PushEngineProxy.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief Base class of all ProdEngine objects proxying a PushEngine.
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProxyProdEngine :

	public ProdEngine,
	public PushEngineProxy {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

protected :

	ProxyProdEngine(
		const	BFC::DL::TypeCPtr &	pType
	) :
		ProdEngine( pType ) {
	}

	virtual void setNbrOutputs(
		const	BFC::Uint32		size
	);

private :

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProxyProdEngine_H_

// ============================================================================

