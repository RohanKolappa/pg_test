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
//	VMP.M2S.PSDecoder.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _VMP_M2S_PSDecoder_h_
#define _VMP_M2S_PSDecoder_h_

// ============================================================================

#include "VMP.M2S.DLL.h"

// ============================================================================

#include "VMP.M2S.PSDecoderPtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "BFC.Debug.DObject.h"

#include "VMP.GPC.ConsumerPtr.h"

#include "VMP.M2S.PESPacketPtr.h"

// ============================================================================

namespace VMP {
namespace M2S {

// ============================================================================

/// [OBSOLETE]

class VMP_M2S_DLL PSDecoder : virtual public BFC::DObject {

public :

	PSDecoder(
	);

	void attachVideoConsumer(
			GPC::ConsumerPtr
					consumer
	);

	void attachAudioConsumer(
			GPC::ConsumerPtr
					consumer
	);

	void consume(
		const	BFC::Buffer &	payload
	);

protected :

	void decodePackHeader();
	void decodeSystemHeader();
	void decodeEndCode();
	PESPacketPtr decodePESPacket();

	void reset();

	static const char *getStreamName(
		const	BFC::Uint32		tag
	);

private :

	enum {
		InvalidId = (BFC::Uint32)-1
	};

	BFC::Uint32		vid;
	BFC::Uint32		aid;

	enum Mode {
		NoMode,
		MPEG1Mode,
		MPEG2Mode
	}			mode;

	GPC::ConsumerPtr	vconsumer;
	GPC::ConsumerPtr	aconsumer;

	BFC::Buffer		cache;
	const BFC::Uchar *	ptr;
	BFC::Uint32		len;

	BFC::Bool		announced[ 0x100 ];

	/// \brief Forbidden copy constructor.

	PSDecoder(
		const	PSDecoder &
	);

	/// \brief Forbidden copy operator.

	PSDecoder & operator = (
		const	PSDecoder &
	);

};

// ============================================================================

} // namespace M2S
} // namespace VMP

// ============================================================================

#endif // _VMP_M2S_PSDecoder_h_

// ============================================================================

