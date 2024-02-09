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
//	VMP.RTP.TTSConverterEngine.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TTSConverterEngine_H_
#define _VMP_RTP_TTSConverterEngine_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TTSConverterEnginePtr.h"

// ============================================================================

#include "BFC.Time.Clock.h"

#include "VMP.L2.ProxyPushEngine.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief Converts TFrame::TS into Frame::PTS and Frame::DTS.
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TTSConverterEngine : public L2::ProxyPushEngine {

public :

	static const BFC::DL::TypeCPtr & getClassType(
	);

	/// \brief Creates a new TTSConverterEngine.

	TTSConverterEngine(
	);

	void init(
		const	BFC::Uint32	pTimeScle
	) {
		timeScle = pTimeScle;
	}

	virtual void putSessionCallback(
			SessionPtr	pSession
	);

	virtual void endSessionCallback(
	);

	virtual void putFrameCallback(
			FramePtr	pFrame
	);

	static const BFC::Uint64	Mod_2_31	= ( CU64( 1 ) << 31 );
	static const BFC::Uint64	Mod_2_32	= ( CU64( 1 ) << 32 );
	static const BFC::Uint64	Mask_63_32	= ~( Mod_2_32 - 1 );

protected :

	BFC::Time::Clock convert(
		const	BFC::Uint32	pTimeStmp,
		const	BFC::Bool	pTimeDisc
	);

private :

	BFC::Uint64		timeScle;	///< E.g. 48000, 90000, ...
	BFC::Bool		initDone;	///< Got a first timestamp ?
	BFC::Uint64		frstStmp;	///< First timestamp for this run.
	BFC::Uint64		lastStmp;	///< Last timestamp for this run.
	BFC::Time::Clock	timeBase;	///< Return value for first timestamp.

	/// \brief Forbidden copy constructor.

	TTSConverterEngine(
		const	TTSConverterEngine &
	);

	/// \brief Forbidden copy operator.

	TTSConverterEngine & operator = (
		const	TTSConverterEngine &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TTSConverterEngine_H_

// ============================================================================

