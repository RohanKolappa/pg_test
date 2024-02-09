// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// Filename:
//	VMP.L2.BytesDecoderEngine.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_L2_BytesDecoderEngine_H_
#define _VMP_L2_BytesDecoderEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.BytesDecoderEnginePtr.h"

// ============================================================================

#include "VMP.L2.ProxyPullEngine.h"

#include "VMP.L3.BytesDecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine wrapping an L3::BytesDecoderBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL BytesDecoderEngine : public ProxyPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	BytesDecoderEngine(
			L3::BytesDecoderBackendPtr	backend
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

	void constructPayload(
	);

	BFC::Buffer dumpBytesSession(
	);

private :

	StreamPtr			iStream;
	L3::BytesDecoderBackendPtr	backend;
	BFC::Buffer			payload;

	// others...

	BytesDecoderEngine(
		const	BytesDecoderEngine&
	);

	BytesDecoderEngine& operator = (
		const	BytesDecoderEngine&
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_BytesDecoderEngine_H_

// ============================================================================

