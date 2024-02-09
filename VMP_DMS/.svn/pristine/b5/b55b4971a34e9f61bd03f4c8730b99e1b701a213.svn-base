// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::M2S".
// 
// "VMP::M2S" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::M2S" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::M2S"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSPAT.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSPAT_h_
#define _VMP_M2S_TSPAT_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPATPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Map.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPAT : virtual public BFC::DObject {

public :

	enum {
		InvalidId	= (BFC::Uint32)-1
	};

	TSPAT(
	);

	BFC::Bool hasVersion(
	) const {
		return ( version != InvalidId );
	}

	BFC::Uint32 getVersion(
	) const {
		return version;
	}

	void setVersion(
		const	BFC::Uint32	pVersion
	) {
		version = pVersion;
	}

	BFC::Bool hasTSId(
	) const {
		return ( ts != InvalidId );
	}

	BFC::Uint32 getTSId(
	) const {
		return ts;
	}

	void setTSId(
		const	BFC::Uint32	pTSId
	) {
		ts = pTSId;
	}

	BFC::Bool hasNetworkPID(
	) const {
		return ( net != InvalidId );
	}

	BFC::Uint32 getNetworkPID(
	) const {
		return net;
	}

	void setNetworkPID(
		const	BFC::Uint32	pNetworkPID
	) {
		net = pNetworkPID;
	}

	BFC::Map< BFC::Uint32, BFC::Uint32 >	map; // PMT PIDs

protected :

private :

	BFC::Uint32	version;
	BFC::Uint32	ts;  // transport_stream_id
	BFC::Uint32	net; // network_PID

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPAT_h_

// ============================================================================

