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
//	VMP.M2S.PSDecoderBackend.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_M2S_PSDecoderBackend_H_
#define _VMP_M2S_PSDecoderBackend_H_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PSDecoderBackendPtr.h"

// ============================================================================

#include "VMP.MuxedSessionPtr.h"

#include "VMP.L3.MuxedDecoderBackend.h"

#include "VMP.M2S.PESPacketPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_M2S

class VMP_M2S_DLL PSDecoderBackend : public L3::MuxedDecoderBackend {

public :

	/// \brief Returns the identity of this class.

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new PSDecoderBackend.

	PSDecoderBackend(
	);

	/// \brief Destroys this object.

	virtual ~PSDecoderBackend(
	);

	virtual BFC::Bool canDecode(
			BytesSessionPtr	pIBSession
	);

	virtual SessionPtr initSession(
			BytesSessionPtr	pIBSession
	);

	virtual void exitSession(
	);

	virtual FramePtr getFrame(
	);

	virtual void seekFrame(
		const	BFC::Uint64	pIndex
	);

protected :

	void decodePackHeader(
	);

	void decodeSystemHeader(
	);

	void decodeEndCode(
	);

	PESPacketPtr decodePESPacket(
	);

	/// \param pSyncWord
	///	The sync word to find, as it should be found in the input
	///	stream if coded in Big Endian format.

	void sync(
		const	BFC::Uint32		pSyncWord,
		const	BFC::Uint32		pSyncMask
	);

	void reset(
	);

	static const char * getStreamName(
		const	BFC::Uint32		tag
	);

private :

	enum Mode {
		NoMode,
		MPEG1Mode,
		MPEG2Mode
	};

	Mode			mode;
	BFC::Uint64		nextOffset;	///< Next Muxed::Frame offset to output.

	struct StreamData {
		BFC::Bool		announced;	///< Present in stream ?
		BFC::Uint64		byteOffset;	///< BytesFrame offset (in bytes).
		BFC::Uint64		unitOffset;	///< BytesFrame offset (in # frames).
		/// \brief Maps a streamId (ex. 0xC0 for first video) to a session
		///	offset (ex. 0).
		BFC::Uint32		sessionId;
	}			stream[ 0x100 ];

	BFC::Uint32		currRate;	///< Current bitrate (in B/s).
	BFC::Time::Clock	fileTick;	///< Last sample of the PCR.
	BFC::Time::Clock	wallTick;	///< Pos in time of last tick.
	BFC::Uint64		tickOffs;	///< Pos in stream of last tick.
	MuxedSessionPtr		oSession;	///< Current session.

	/// \brief Forbidden copy constructor.

	PSDecoderBackend(
		const	PSDecoderBackend &
	);

	/// \brief Forbidden copy operator.

	PSDecoderBackend & operator = (
		const	PSDecoderBackend &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PSDecoderBackend_H_

// ============================================================================

