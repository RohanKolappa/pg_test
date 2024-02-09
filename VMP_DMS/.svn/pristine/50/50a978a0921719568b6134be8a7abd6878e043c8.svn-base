// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::L2".
// 
// "VMP::L2" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::L2" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::L2"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.L2.StreamDecoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_StreamDecoderEngine_H_
#define _VMP_L2_StreamDecoderEngine_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.StreamDecoderEnginePtr.h"

// ============================================================================

#include "BFC.IO.ByteInputStream.h"

#include "VMP.L2.FrameStore.h"
#include "VMP.L2.IOPullInputDevice.h"
#include "VMP.L2.ProxyPullEngine.h"

#include "VMP.L3.StreamDecoderBackend.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief ProxyPullEngine wrapping an L3::StreamDecoderBackend.
///
/// \ingroup VMP_L2

class VMP_L2_DLL StreamDecoderEngine : public ProxyPullEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	StreamDecoderEngine(
			L3::StreamDecoderBackendPtr	backend
	);

	// Reimplemented from ProxyPullEngine...

	virtual void attachPeerEngine(
		const	BFC::Uint32		slot,
			PullEnginePtr		backend
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32		slot
	);

	// Implementation of the PullEngine interface...

	virtual SessionPtr getSessionCallback(
	);

	virtual void endSessionCallback(
	);

	virtual FramePtr getFrameCallback(
		const	BFC::Uint64		index
	);

protected :

private :

	IOPullInputDevicePtr		ioDevice;
	BFC::IO::ByteInputStreamPtr	ioStream;
	L3::StreamDecoderBackendPtr	backend;
	FrameStorePtr			frmStore;
	BFC::Uint64			begOffset;	///< Start in input stream.
	BFC::Uint64			nxtOffset;	///< Next in output stream.
	UnitsIndexPtr			untIndex;
	KeyIndexPtr			keyIndex;
	enum {
		NoRandom,
		SelfRandom,
		BackendRandom
	}				randMode;

	StreamDecoderEngine(
	);

	StreamDecoderEngine(
		const	StreamDecoderEngine &
	);

	StreamDecoderEngine & operator = (
		const	StreamDecoderEngine &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_StreamDecoderEngine_H_

// ============================================================================

