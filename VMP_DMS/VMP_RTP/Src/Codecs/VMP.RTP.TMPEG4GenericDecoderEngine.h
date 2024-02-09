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
//	VMP.RTP.TMPEG4GenericDecoderEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TMPEG4GenericDecoderEngine_H_
#define _VMP_RTP_TMPEG4GenericDecoderEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TMPEG4GenericDecoderEnginePtr.h"

// ============================================================================

#include "VMP.AAC.AudioSpecificConfigPtr.h"

#include "VMP.RTP.SEStreamPtr.h"
#include "VMP.RTP.TMPEG4GenericContextPtr.h"
#include "VMP.RTP.TEStreamDecoderEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TMPEG4GenericDecoderEngine : public TEStreamDecoderEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TMPEG4GenericDecoderEngine.

	TMPEG4GenericDecoderEngine(
			SEStreamCPtr	pSEStream,
			TMPEG4GenericContextCPtr
					pCodcCtxt
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

	void decodePacket(
	);

	/// \brief Adds ADTS header to AAC-hbr stream.
	///
	/// See ISO/IEC 13818-7, section 6.2.2.

	BFC::Buffer addADTSHeader(
		const	BFC::Buffer &	pData
	) const;

	BFC::Bool decodeADTSHeader(
			BFC::Uint32 &	pBitRate,
			BFC::Float &	pLength,
			BFC::Uint32 &	pFrmeSize
	);

private :

	static const BFC::Uint64 Mod_2_32	= ( CU64( 1 ) << 32 );

	SEStreamCPtr		sestream;
	TMPEG4GenericContextCPtr
				codcCtxt;
	AAC::AudioSpecificConfigPtr
				codcCnfg;
	BFC::Uint64		smplRate;
	BFC::Uint64		frmeOffs;
	BFC::Uint64		unitOffs;

	/// \brief Forbidden default constructor.

	TMPEG4GenericDecoderEngine(
	);

	/// \brief Forbidden copy constructor.

	TMPEG4GenericDecoderEngine(
		const	TMPEG4GenericDecoderEngine &
	);

	/// \brief Forbidden copy operator.

	TMPEG4GenericDecoderEngine & operator = (
		const	TMPEG4GenericDecoderEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TMPEG4GenericDecoderEngine_H_

// ============================================================================

