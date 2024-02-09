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
//	VMP.L2.ProxyPushFifo.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_L2_ProxyPushFifo_H_
#define _VMP_L2_ProxyPushFifo_H_

// ============================================================================

#include "VMP.L2.DLL.h"

// ============================================================================

#include "VMP.L2.ProxyPushFifoPtr.h"

// ============================================================================

#include "VMP.L2.ProxyPushEngine.h"
#include "VMP.L2.PumperPtr.h"
#include "VMP.L2.SimpleProxyPushEnginePtr.h"
#include "VMP.L2.SinkPtr.h"

// ============================================================================

namespace VMP {
namespace L2 {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_L2

class VMP_L2_DLL ProxyPushFifo : public ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new ProxyPushFifo.

	ProxyPushFifo(
	);

	/// \brief Destroys this object.

	virtual ~ProxyPushFifo(
	);

	virtual void attachPeerEngine(
		const	BFC::Uint32	pPeerIndx,
			PushEnginePtr	pPeerEngn
	);

	virtual void detachPeerEngine(
		const	BFC::Uint32	pPeerIndx
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

	void privInit(
			PushEnginePtr	pPeerEngn
	);

	void privExit(
	);

private :

	SinkPtr				sinkEngn;
	PumperPtr			pumpEngn;
	SimpleProxyPushEnginePtr	outpEngn;

	/// \brief Forbidden copy constructor.

	ProxyPushFifo(
		const	ProxyPushFifo &
	);

	/// \brief Forbidden copy operator.

	ProxyPushFifo & operator = (
		const	ProxyPushFifo &
	);

};

// ============================================================================

} // namespace L2
} // namespace VMP

// ============================================================================

#endif // _VMP_L2_ProxyPushFifo_H_

// ============================================================================

