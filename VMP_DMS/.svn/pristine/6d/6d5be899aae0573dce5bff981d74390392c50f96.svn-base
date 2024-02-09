// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.BytesEncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_BytesEncoderEngine_H_
#define _VMP_L2_BytesEncoderEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.BytesEncoderEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"
#include "VMP.L3.BytesEncoderBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPushEngine wrapping an L3::BytesEncoderBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL BytesEncoderEngine : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	BytesEncoderEngine(
			L3::BytesEncoderBackendPtr	backend
	);

	// Implementation of the ProxyPushEngine interface...

	virtual void putSessionCallback(
			SessionPtr		session
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr		frame
	);

protected :

private :

	L3::BytesEncoderBackendPtr	backend;
	BytesSessionPtr			iSession;
	StreamPtr			oStream;

	// others...

	BytesEncoderEngine(
		const	BytesEncoderEngine&
	);

	BytesEncoderEngine& operator = (
		const	BytesEncoderEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_BytesEncoderEngine_H_

// ============================================================================

