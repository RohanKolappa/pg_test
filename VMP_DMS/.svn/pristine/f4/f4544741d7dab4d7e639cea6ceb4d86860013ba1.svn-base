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
//	VMP.M2S.TSPacket.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_TSPacket_h_
#define _VMP_M2S_TSPacket_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.TSPacketPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "VMP.M2S.Packet.h"
#include "VMP.M2S.TimeStamp.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

class VMP_M2S_DLL TSPacket : public Packet {

public :

	TSPacket(
	) :
		packOffs	( 0 ),
		packHder	( 0 )
	{
	}

	void decode(
		const	BFC::Uint64	pOffs,
		const	BFC::Buffer &	pData // points to 188 bytes!
	);

	BFC::Uint64 getPacketOffset(
	) const {
		return packOffs;
	}

	const BFC::Buffer & getRawCopy(
	) const {
		return packCopy;
	}

	BFC::Uint32 getTransportErrorIndicator(
	) const {
		return ( ( packHder & 0x00800000 ) >> 23 );
	}

	BFC::Uint32 getPayloadUnitStartIndicator(
	) const {
		return ( ( packHder & 0x00400000 ) >> 22 );
	}

	BFC::Uint32 getTransportPriority(
	) const {
		return ( ( packHder & 0x00200000 ) >> 21 );
	}

	BFC::Uint32 getPID(
	) const {
		return ( ( packHder & 0x001FFF00 ) >> 8 );
	}

	BFC::Uint32 getTransportScramblingControl(
	) const {
		return ( ( packHder & 0x000000C0 ) >> 6 );
	}

	BFC::Uint32 getAdaptationFieldControl(
	) const {
		return ( ( packHder & 0x00000030 ) >> 4 );
	}

	BFC::Uint32 getContinuityCounter(
	) const {
		return ( ( packHder & 0x0000000F ) );
	}

	BFC::Bool hasAdaptationField(
	) const {
		return ( ( getAdaptationFieldControl() & 0x02 ) != 0 );
	}

	struct VMP_M2S_DLL AdaptationField;

	const AdaptationField & getAdaptationField(
	) const {
		return adaptFld;
	}

	BFC::Bool hasPayload(
	) const {
		return ( ( getAdaptationFieldControl() & 0x01 ) != 0 );
	}

	const BFC::Buffer & getPayload(
	) const {
		return packData;
	}

	BFC::Buffer toBuffer(
	) const;

	struct VMP_M2S_DLL AdaptationField {

		void decode(
			const	BFC::Uchar *&	pPtr,
				BFC::Uint32 &	pLen
		);

		BFC::Buffer toBuffer(
		) const;

		BFC::Uint32	adaptation_field_length;

		BFC::Bool	discontinuity_indicator;
		BFC::Bool	random_access_indicator;
		BFC::Bool	elementary_stream_priority_indicator;
		BFC::Bool	PCR_flag;
		BFC::Bool	OPCR_flag;
		BFC::Bool	splicing_point_flag;
		BFC::Bool	transport_private_data_flag;
		BFC::Bool	adaptation_field_extension_flag;

		TimeStampPtr	PCR;
		TimeStampPtr	OPCR;
		BFC::Int32	splice_countdown;
		BFC::Uint32	transport_private_data_length;
		BFC::Buffer	private_data_byte;
		BFC::Uint32	adaptation_field_extension_length;

	};

protected :

private :

	BFC::Uint64	packOffs;	///< Packet offset in stream (in bytes).
	BFC::Buffer	packCopy;	///< Copy of whole packet.
	BFC::Uint32	packHder;
	AdaptationField	adaptFld;
	BFC::Buffer	packData;

	/// \brief Forbidden copy constructor.

	TSPacket(
		const	TSPacket &
	);

	/// \brief Forbidden copy operator.

	TSPacket & operator = (
		const	TSPacket &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_TSPacket_h_

// ============================================================================

