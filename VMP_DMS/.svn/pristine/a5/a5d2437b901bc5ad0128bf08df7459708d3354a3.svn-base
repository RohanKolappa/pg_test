// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "VMP::RTP".
// 
// "VMP::RTP" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "VMP::RTP" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "VMP::RTP"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	VMP.RTP.TDTSComputerEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TDTSComputerEngine_H_
#define _VMP_RTP_TDTSComputerEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TDTSComputerEnginePtr.h"

// ============================================================================

#include "BFC.TL.Array.h"
#include "BFC.TL.CompArray.h"
#include "BFC.TL.SortArray.h"

#include "BFC.Time.Clock.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief Recreates DTS based on PTS, and adjusts DTS/PTS to correct NTP
///	jumps, winter/summer time switches, ...
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TDTSComputerEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TDTSComputerEngine.

	TDTSComputerEngine(
	);

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	virtual void flushSessionCallback(
	);

protected :

	void resetState(
	);

	void processFrame(
			FramePtr	pInptFrme
	);

	void flushFrames(
	);

	void sendFrame(
			FramePtr	pInptFrme
	);

private :

	static const BFC::Uint64	Mod_2_31	= ( CU64( 1 ) << 31 );
	static const BFC::Uint64	Mod_2_32	= ( CU64( 1 ) << 32 );

	BFC::Bool				initDone;
	BFC::Uint32				buffSize;
	BFC::Time::Delta			xtsDelta;
	BFC::CompArray< BFC::Time::Clock >	inptTime;	///< Unsorted input PTS.
	BFC::Array< BFC::Array< FramePtr > >	inptFrme;	///< Unsorted input frames.
	BFC::SortArray< BFC::Time::Clock >	sortTime;	///< Sorted PTS --> DTS.
	BFC::Time::Clock			prevTime;	///< Previous output DTS.
	BFC::Time::Delta			currCorr;	///< Current correction.

	/// \brief Forbidden copy constructor.

	TDTSComputerEngine(
		const	TDTSComputerEngine &
	);

	/// \brief Forbidden copy operator.

	TDTSComputerEngine & operator = (
		const	TDTSComputerEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TDTSComputerEngine_H_

// ============================================================================

