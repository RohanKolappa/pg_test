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
//	VMP.M2S.TSPacketDispatcher.h
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#ifndef _VMP_M2S_TSPacketDispatcher_h_
#define _VMP_M2S_TSPacketDispatcher_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPacketDispatcherPtr.h"

// ============================================================================

#include "VMP.GPC.Consumer.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPacketDispatcher : public GPC::Consumer {

public :

	TSPacketDispatcher(
	);

	void setPeerConsumer(
		const	BFC::Uint32	pEStrmPid,
			GPC::ConsumerPtr
					pConsumer
	);

	void delPeerConsumer(
		const	BFC::Uint32	pEStrmPid
	);

	BFC::Bool hasPeerConsumer(
		const	BFC::Uint32	pEStrmPid
	) const;

	void attachPCRConsumer(
		const	BFC::Uint32	pEStrmPid,
			GPC::ConsumerPtr
					pConsumer
	);

	void detachPCRConsumer(
		const	BFC::Uint32	pEStrmPid
	);

	BFC::Bool hasPCRConsumer(
		const	BFC::Uint32	pEStrmPid
	) const;

	void attachFallback(
			GPC::ConsumerPtr
					pFallback
	);

	void detachFallback(
	);

	BFC::Bool hasFallback(
	) const;

	void attachPassThru(
			GPC::ConsumerPtr
					pPassThru
	);

	void detachPassThru(
	);

	BFC::Bool hasPassThru(
	) const;

	virtual void putObject(
			BFC::SPtr	pTSPacket
	);

	virtual void dumpStatus(
	) const;

protected :

	void checkPid(
		const	BFC::Uint32	pEStrmPid
	) const;

private :

	enum {
		InvalidCC = ( BFC::Uint32 )-1
	};

	BFC::Uint64		trErrCnt;	///< Transport error indicator.
	BFC::Uint64		pcktCntr;	///< Number of received packets.
	BFC::Uint64		dropCntr;	///< Number of dropped packets.

	struct TblEntry {

		TblEntry(
		) :
			currCC	( InvalidCC ),
			received(         0 ),
			consumed(         0 )
		{
		}

		GPC::ConsumerPtr	consumer;
		GPC::ConsumerPtr	pcr;
		BFC::Uint32		currCC;
		BFC::Uint64		received;
		BFC::Uint64		consumed;

	}			tbl[ 0x2000 ];

	GPC::ConsumerPtr	fallback;
	GPC::ConsumerPtr	passthru;

	/// \brief Forbidden copy constructor.

	TSPacketDispatcher(
		const	TSPacketDispatcher &
	);

	/// \brief Forbidden copy operator.

	TSPacketDispatcher & operator = (
		const	TSPacketDispatcher &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPacketDispatcher_h_

// ============================================================================

