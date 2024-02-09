// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.PictureEncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_PictureEncoderEngine_H_
#define _VMP_L2_PictureEncoderEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PictureEncoderEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"
#include "VMP.L3.PictureEncoderBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPushEngine wrapping an L3::PictureEncoderBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL PictureEncoderEngine : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	PictureEncoderEngine(
			L3::PictureEncoderBackendPtr	backend
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

	L3::PictureEncoderBackendPtr	backend;
	BFC::Bool			isFlipped;
	VideoStreamPtr			ovStream;

	// others...

	PictureEncoderEngine(
		const	PictureEncoderEngine&
	);

	PictureEncoderEngine& operator = (
		const	PictureEncoderEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PictureEncoderEngine_H_

// ============================================================================

