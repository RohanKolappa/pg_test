// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC".
// 
// "BFC" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.M2S.TSEncoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_TSEncoderEngine_H_
#define _VMP_M2S_TSEncoderEngine_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSEncoderEnginePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Time.Clock.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL TSEncoderEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TSEncoderEngine.

	TSEncoderEngine(
	);

	/// \brief Destroys this object.

	virtual ~TSEncoderEngine(
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

protected :

	BFC::Buffer addADTSHeader(
		const	BFC::Buffer &	pData,
			ConfigInfoCPtr	pCnfg
	) const;

	void generateTSPackets(
		const	BFC::Uint32	pPID,
		const	BFC::Buffer &	pBuf,
		const	BFC::Time::Clock &
					pPTS,
		const	BFC::Time::Clock &
					pDTS,
		const	BFC::Bool	pPCR
	);

	void sendTSPacket(
		const	BFC::Buffer &	pTSPacket,
		const	BFC::Time::Clock &
					pFramePts,
		const	BFC::Time::Clock &
					pFrameDts
	);

	BFC::Buffer getPATPacket(
	);

	BFC::Buffer getPMTPacket(
	);

	static BFC::Uint32 getCRC32(
		const	BFC::Uchar *	pPtr,
			BFC::Uint32	pLen
	);

private :

	class SCR {
	public :
		BFC::Uint8	highBits;
		BFC::Uint32	rmngBits;
		BFC::Uint16	extsBits;
	};

	static const BFC::Uint32	cTSPckSz = 188;		///< Transport Packet size.
	static const BFC::Uint32	cMaxSPid = 0x2000;	///< Maximum PID value.
	static const BFC::Uint16	csPatPid = 0x0000;	///< PAT PID.
	static const BFC::Uint16	progNmbr = 0x0001;	///< Fixed program number.
	static const BFC::Uint16	csPmtPid = 0x0100;	///< PMT PID.

	struct Entry {
		BFC::Uint16	estrmPid;	///< EStream PID (e.g. 0x0101 for first estream).
		BFC::Uchar	streamId;	///< Packetized stream id (e.g. 0xC0 for first audio, ...).
		BFC::Uint8	strmType;	///< EStream type (e.g. 0x1B for H.264, ...).
		ConfigInfoCPtr	cnfgInfo;	///< ConfigInfo needed for ADTS header addition.
	};

	BFC::Array< Entry >		strmList;		///< Our exported estreams.
	BFC::Uint16			crPcrPid;		///< Current PCR PID.
	BFC::Bool			chnkStrt;		///< Should we start a new Segment now ?
	BFC::Uint32			contCntr[ cMaxSPid ];	///< Continuity counters.
	BFC::Buffer			patPackt;		///< Cached PAT.
	BFC::Buffer			pmtPackt;		///< Cached PMT.
	BFC::Uint64			byteOffs;
	BFC::Uint64			unitOffs;

	static const BFC::Uint32	CRC32[ 256 ];

	/// \brief Forbidden copy constructor.

	TSEncoderEngine(
		const	TSEncoderEngine &
	);

	/// \brief Forbidden copy operator.

	TSEncoderEngine & operator = (
		const	TSEncoderEngine &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSEncoderEngine_H_

// ============================================================================

