// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.PictureDecoderEngine.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_L2_PictureDecoderEngine_H_
#define _VMP_L2_PictureDecoderEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.PictureDecoderEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPullEngine.h"
#include "VMP.L3.PictureDecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine wrapping an L3::PictureDecoderBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL PictureDecoderEngine : public ProxyPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	PictureDecoderEngine(
			L3::PictureDecoderBackendPtr	backend
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

	L3::PictureDecoderBackendPtr	backend;
	BFC::Image::Picture		picture;

	// Private helpers...

	void constructPicture(
	);

	BFC::Buffer dumpBytesSession(
		const	BFC::Uint32		totalLength
	);

	// others...

	PictureDecoderEngine(
		const	PictureDecoderEngine&
	);

	PictureDecoderEngine& operator = (
		const	PictureDecoderEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_PictureDecoderEngine_H_

// ============================================================================

