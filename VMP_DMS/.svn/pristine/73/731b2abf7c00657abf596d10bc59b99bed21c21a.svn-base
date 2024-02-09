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
//	VMP.RTP.TAVCDecoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TAVCDecoderEngine_H_
#define _VMP_RTP_TAVCDecoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TAVCDecoderEnginePtr.h"

// ============================================================================

#include "BFC.Base.Buffer.h"

#include "VMP.AVC.CodecConfigPtr.h"

#include "VMP.RTP.SEStreamPtr.h"
#include "VMP.RTP.TEStreamDecoderEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TAVCDecoderEngine : public TEStreamDecoderEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TAVCDecoderEngine.

	TAVCDecoderEngine(
			SEStreamCPtr	pSEStream,
		const	BFC::Uint32	pPackMode,
		const	BFC::Buffer &	pProfLevl,
			AVC::CodecConfigCPtr
					pCnfgRcrd
	);

	virtual void putSessionCallback(
			SessionPtr	pInptSess
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pInptFrme
	);

	virtual void flushSessionCallback(
	);

protected :

	void sendNalu(
			FrameCPtr	pTmplFrme,
		const	BFC::Buffer &	pNaluData
	);

private :

	static const BFC::Uchar		TypeStapA	= 0x18;
	static const BFC::Uchar		TypeStapB	= 0x19;
	static const BFC::Uchar		TypeMtap16	= 0x1A;
	static const BFC::Uchar		TypeMtap24	= 0x1B;
	static const BFC::Uchar		TypeFuA		= 0x1C;
	static const BFC::Uchar		TypeFuB		= 0x1D;

	static const BFC::Uint64	Mod_2_16	= ( CU64( 1 ) << 16 );

	SEStreamCPtr		sestream;
	BFC::Uint32		packMode;
	BFC::Buffer		profLevl;
	AVC::CodecConfigCPtr	cnfgRcrd;
	BFC::Uint64		frmeOffs;
	BFC::Uint64		unitOffs;
	BFC::Buffer		fuaBuffr;
	BFC::Bool		dataDisc;
	BFC::Bool		timeDisc;

	/// \brief Forbidden default constructor.

	TAVCDecoderEngine(
	);

	/// \brief Forbidden copy constructor.

	TAVCDecoderEngine(
		const	TAVCDecoderEngine &
	);

	/// \brief Forbidden copy operator.

	TAVCDecoderEngine & operator = (
		const	TAVCDecoderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TAVCDecoderEngine_H_

// ============================================================================

