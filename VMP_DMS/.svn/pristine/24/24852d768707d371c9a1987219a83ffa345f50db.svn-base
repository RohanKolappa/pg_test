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
//	VMP.AVC.NALU2AUFramer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_AVC_NALU2AUFramer_H_
#define _VMP_AVC_NALU2AUFramer_H_

// ============================================================================

#include "VMP.AVC.DLL.h"

// ============================================================================

#include "VMP.AVC.NALU2AUFramerPtr.h"

// ============================================================================

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.AVC.NALUPtr.h"
#include "VMP.AVC.SequenceParameterSetPtr.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace AVC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_AVC

class VMP_AVC_DLL NALU2AUFramer : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new NALU2AUFramer.

	NALU2AUFramer(
	);

	/// \brief Destroys this object.

	virtual ~NALU2AUFramer(
	);

	virtual void putSessionCallback(
			VMP::SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			VMP::FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

	void verify(
			BFC::Array< NALUCPtr > &
				pNaluList
	) const;

	BFC::Buffer collect(
		const	BFC::Array< NALUCPtr > &
				pNaluList
	) const;

	void decodeSPS(
			NALUCPtr	pSPS
	);

private :

	BytesSessionPtr		iBytSess;
	VideoStreamPtr		iVidStrm;
	NALUCPtr		audCache;
	NALUCPtr		spsCache;
	SequenceParameterSetPtr seqParSt;
	NALUCPtr		ppsCache;
	BytesSessionPtr		oBytSess;
	VideoStreamPtr		oVidStrm;
	BFC::Uint32		pictWdth;
	BFC::Uint32		pictHght;
	BFC::Uint32		frmeRate;
	BFC::Uint32		frmeScle;

	// Managed by the NALU fetcher and SPS decoder...

	BFC::Uint32		num_units_in_tick;		///< From SPS->VUI.
	BFC::Uint32		time_scale;			///< From SPS->VUI.
	BFC::Uint32		fixed_frame_rate_flag;		///< From SPS->VUI.
	BFC::Bool		separate_colour_plane_flag;	///< From SPS.
	BFC::Uint32		log2_max_frame_num;		///< From SPS.
	BFC::Bool		frame_mbs_only_flag;		///< From SPS.
	BFC::Uint32		pic_order_cnt_type;		///< From SPS.
	BFC::Uint32		log2_max_pic_order_cnt_lsb;	///< From SPS.

	// Managed by NALU --> AU engine...

	NALUCPtr		prevNalu;	///< Previous NALU we handled.
	BFC::Time::Clock	prevsPTS;	///< prevNalu PTS.
	BFC::Time::Clock	prevsDTS;	///< prevNalu DTS.
	BFC::Array< NALUCPtr >	naluList;	///< Buffered NALU's while no complete access unit.
	BFC::Bool		keyFrame;	///< Are we accumulating a keyframe ?
	BFC::Uint64		aUntCntr;	///< Access Unit counter.

	/// \brief Forbidden copy constructor.

	NALU2AUFramer(
		const	NALU2AUFramer &
	);

	/// \brief Forbidden copy operator.

	NALU2AUFramer & operator = (
		const	NALU2AUFramer &
	);

};

// ============================================================================

} // namespace AVC
} // namespace VMP

// ============================================================================

#endif // _VMP_AVC_NALU2AUFramer_H_

// ============================================================================

