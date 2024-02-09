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
//	VMP.M2S.TSPacketFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSPacketFramer_h_
#define _VMP_M2S_TSPacketFramer_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPacketFramerPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.IO.FinalDevice.h"

#include "BFC.Time.Chrono.h"

#include "VMP.GPC.ConsumerProxy.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPacketFramer :

	public BFC::IO::FinalDevice,
	public GPC::ConsumerProxy {

public :

	TSPacketFramer(
	);

	void setFriendlyMode(
		const	BFC::Bool	pFriendly
	) {
		friendly = pFriendly;
	}

	virtual void open(
	);

	virtual void close(
	);

	virtual void putBytes(
		const	BFC::Buffer &	pDataBuff
	);

	virtual BFC::Uint64 seek(
		const	BFC::Uint64	pByteOffs
	);

	virtual BFC::Uint64 tell(
	) const;

	virtual BFC::Uint64 length(
	) const;

	void dumpStatus(
	);

protected :

private :

	BFC::Bool		friendly;
	BFC::Time::Chrono	strtTime;
	BFC::Uint64		totlSize;
	BFC::Uint64		byteOffs;		///< Offset of dataBuff.
	BFC::Bool		isSynced;
	BFC::Uint32		skipSize;
	BFC::Buffer		dataBuff;

	/// \brief Forbidden copy constructor.

	TSPacketFramer(
		const	TSPacketFramer &
	);

	/// \brief Forbidden copy operator.

	TSPacketFramer & operator = (
		const	TSPacketFramer &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPacketFramer_h_

// ============================================================================

