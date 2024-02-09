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
//	VMP.M2S.PESPacketFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_PESPacketFramer_h_
#define _VMP_M2S_PESPacketFramer_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PESPacketFramerPtr.h"

// ============================================================================

#include "BFC.Base.BufferConsumer.h"

#include "VMP.GPC.ProxyConsumer.h"

#include "VMP.M2S.PESPacketPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL PESPacketFramer :

	public GPC::ProxyConsumer,
	public BFC::BufferConsumer {

public :

	PESPacketFramer(
		const	BFC::Uchar	pStreamType = 0x00,
		const	BFC::Uchar	pStreamMask = 0x00
	);

	/// \brief Rebuilds PES Packets from TS Packets.

	virtual void putObject(
			BFC::SPtr	pTSPacket
	);

	virtual void putBuffer(
		const	BFC::Buffer &	pBuffer
	);

protected :

private :

	BFC::Uchar		strType;
	BFC::Uchar		strMask;
	BFC::Bool		synced;
	BFC::Buffer		cache;

	PESPacketPtr		curr;

	/// \brief Forbidden copy constructor.

	PESPacketFramer(
		const	PESPacketFramer &
	);

	/// \brief Forbidden copy operator.

	PESPacketFramer & operator = (
		const	PESPacketFramer &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PESPacketFramer_h_

// ============================================================================

