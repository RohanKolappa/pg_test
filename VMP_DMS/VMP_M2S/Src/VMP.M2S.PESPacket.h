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
//	VMP.M2S.PESPacket.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_PESPacket_h_
#define _VMP_M2S_PESPacket_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PESPacketPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"
#include "BFC.Base.SObject.h"

#include "VMP.M2S.Packet.h"
#include "VMP.M2S.TimeStamp.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL PESPacket : public Packet {

public :

	PESPacket(
		const	BFC::Bool	MPEG2Syntax = true
	);

	BFC::Uint32 decode(
		const	BFC::Buffer &	payload
	);

	BFC::Buffer encode(
	);

	void forceReady(
	);

	/// \brief Creates a human-readable representation of this packet.

	BFC::Buffer toBuffer(
	) const;

	BFC::Bool	isMPEG2;
	BFC::Bool	packetReady;

	enum {
		ProgramStreamMap	= 0xBC,
		PrivateStream1		= 0xBD,
		PaddingStream		= 0xBE,
		PrivateStream2		= 0xBF,
		AudioStream0		= 0xC0,
		AudioStream1		= 0xC1,
		AudioStream2		= 0xC2,
		AudioStream3		= 0xC3,
		AudioStream4		= 0xC4,
		AudioStream5		= 0xC5,
		AudioStream6		= 0xC6,
		AudioStream7		= 0xC7,
		AudioStream8		= 0xC8,
		AudioStream9		= 0xC9,
		AudioStream10		= 0xCA,
		AudioStream11		= 0xCB,
		AudioStream12		= 0xCC,
		AudioStream13		= 0xCD,
		AudioStream14		= 0xCE,
		AudioStream15		= 0xCF,
		AudioStream16		= 0xD0,
		AudioStream17		= 0xD1,
		AudioStream18		= 0xD2,
		AudioStream19		= 0xD3,
		AudioStream20		= 0xD4,
		AudioStream21		= 0xD5,
		AudioStream22		= 0xD6,
		AudioStream23		= 0xD7,
		AudioStream24		= 0xD8,
		AudioStream25		= 0xD9,
		AudioStream26		= 0xDA,
		AudioStream27		= 0xDB,
		AudioStream28		= 0xDC,
		AudioStream29		= 0xDD,
		AudioStream30		= 0xDE,
		AudioStream31		= 0xDF,
		VideoStream0		= 0xE0,
		VideoStream1		= 0xE1,
		VideoStream2		= 0xE2,
		VideoStream3		= 0xE3,
		VideoStream4		= 0xE4,
		VideoStream5		= 0xE5,
		VideoStream6		= 0xE6,
		VideoStream7		= 0xE7,
		VideoStream8		= 0xE8,
		VideoStream9		= 0xE9,
		VideoStream10		= 0xEA,
		VideoStream11		= 0xEB,
		VideoStream12		= 0xEC,
		VideoStream13		= 0xED,
		VideoStream14		= 0xEE,
		VideoStream15		= 0xEF,
		ECMStream		= 0xF0,
		EMMStream		= 0xF1,
		DSMCCStream		= 0xF2,
		ISOIES13522Stream	= 0xF3,
		H222TypeA		= 0xF4,
		H222TypeB		= 0xF5,
		H222TypeC		= 0xF6,
		H222TypeD		= 0xF7,
		H222TypeE		= 0xF8,
		AncillaryStream		= 0xF9,
		ProgramStreamDirectory	= 0xFF
	};

	BFC::Buffer	packetDump;	// 0-2:	00.00.01
	BFC::Uint32	streamType;	// 3:	stream_id
	BFC::Uint32	packetLength;	// 4-5:	packet_length

	// MPEG-2 only...

	BFC::Uint32	PES_scrambling_control;
	BFC::Bool	PES_priority;
	BFC::Bool	data_align_indicator;
	BFC::Bool	copyright;
	BFC::Bool	original_or_copy;
	BFC::Bool	ESCR_flag;
	BFC::Bool	ES_rate_flag;
	BFC::Bool	DSM_trick_mode_flag;
	BFC::Bool	additional_copy_info;
	BFC::Bool	PES_CRC_flag;
	BFC::Bool	PES_extension_flag;
	BFC::Uint32	headerLength;

	// MPEG-1 & MPEG-2...

	BFC::Bool	PTS_flag;
	BFC::Bool	DTS_flag;
	TimeStamp	PTS;
	TimeStamp	DTS;

	BFC::Buffer	data_byte;

protected :

	void decodeFields(
	);

private :

	BFC::Bool	startCodeChecked;

	/// \brief Forbidden copy constructor.

	PESPacket(
		const	PESPacket &
	);

	/// \brief Forbidden copy operator.

	PESPacket & operator = (
		const	PESPacket &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PESPacket_h_

// ============================================================================

