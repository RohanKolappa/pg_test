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
//	VMP.RTP.TSharedClock.h
//
// Description:
//	...
//
// Author(s):
//	Jean-Francois GOBBERS
//
// ============================================================================

#ifndef _VMP_RTP_TSharedClock_H_
#define _VMP_RTP_TSharedClock_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TSharedClockPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.Time.Clock.h"

#include "VMP.RTP.TClockLinePtr.h"
#include "VMP.RTP.TClockPointPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief Provides a mapping between our clock and some peer clock.
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TSharedClock : virtual public BFC::DObject {

public :

	/// \brief Creates a new TSharedClock.

	TSharedClock(
		const	BFC::Uint32	pClckPrio
	);

	BFC::Uint32 getPriority(
	) const {
		return clckPrio;
	}

	/// \brief (Re)Initializes this Clock as a master.

	void configClock(
			TClockPointCPtr	pSyncTime
	);

	/// \brief (Re)Initializes this Clock as a slave, using master clock
	///	\a pMstrClck.

	void followClock(
			TSharedClockPtr	pMstrClck
	);

	/// \brief Adds the couple ( pSelfTime, pPeerTime ) to the clock speed
	///	estimator.
	///
	/// pPeerTime uses a random time base which may vary between
	/// discontinuities.
	///
	/// If the new point is considered too suspicious, a discontinuity is
	/// forced.
	///
	/// If \a pTimeDisc is true, \a pPeerTime might use another time base
	/// than the previous packet.

	BFC::Time::Clock addPacket(
		const	BFC::Time::Clock &
					pSelfTime,
		const	BFC::Time::Clock &
					pPeerTime,
		const	BFC::Bool	pTimeDisc
	);

	TClockPointCPtr getSyncTime(
	) const;

	TClockPointCPtr getSyncTime(
		const	BFC::Time::Clock &
					pSelfTime
	);

	BFC::Time::Delta getFreshness(
		const	BFC::Time::Clock &
					pSelfTime
	) const;

protected :

	BFC::Time::Delta getTargetDelta(
	) const;

	BFC::Time::Delta getTargetDelta(
		const	BFC::Time::Clock &
					pSelfTime
	);

	BFC::Bool hasDiscontinuity(
		const	BFC::Time::Clock &
					pSelfTime,
		const	BFC::Time::Clock &
					pPeerTime
	) const;

private :

	BFC::Uint32		clckPrio;	///< Clock priority.
	TSharedClockPtr		mstrClck;	///< Master TSharedClock.
	TClockPointCPtr		syncTime;	///< Shared by all clocks.
	BFC::Time::Delta	prevLoop;	///< Peer time increase before last discontinuity.
	TClockLinePtr		currLine;
	TClockPointCPtr		prevResp;	///< Previous response.
	BFC::Time::Clock	lastSelf;

	/// \brief Forbidden default constructor.

	TSharedClock(
	);

	/// \brief Forbidden copy constructor.

	TSharedClock(
		const	TSharedClock &
	);

	/// \brief Forbidden copy operator.

	TSharedClock & operator = (
		const	TSharedClock &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TSharedClock_H_

// ============================================================================

