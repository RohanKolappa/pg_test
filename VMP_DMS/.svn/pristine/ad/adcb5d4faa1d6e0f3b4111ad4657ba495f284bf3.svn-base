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
//	VMP.RTP.TClockSharer.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _VMP_RTP_TClockSharer_H_
#define _VMP_RTP_TClockSharer_H_

// ============================================================================

#include "VMP.RTP.DLL.h"

// ============================================================================

#include "VMP.RTP.TClockSharerPtr.h"

// ============================================================================

#include "BFC.Debug.DObject.h"

#include "BFC.TL.Array.h"

#include "BFC.Time.Clock.h"

#include "VMP.RTP.TSharedClockPtr.h"

// ============================================================================

namespace VMP {
namespace RTP {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup VMP_RTP

class VMP_RTP_DLL TClockSharer : virtual public BFC::DObject {

public :

	/// \brief Creates a new TClockSharer.

	TClockSharer(
	);

	void addClock(
			TSharedClockPtr	pEstrClck,
		const	BFC::Time::Clock &
					pWallTime
	);

	void checkMaster(
		const	BFC::Time::Clock &
					pSelfTime
	);

protected :

private :

	BFC::Uint32			mstrIndx;
	BFC::Array< TSharedClockPtr >	clckList;	///< [ mstrIndx ] is the master.

	/// \brief Forbidden copy constructor.

	TClockSharer(
		const	TClockSharer &
	);

	/// \brief Forbidden copy operator.

	TClockSharer & operator = (
		const	TClockSharer &
	);

};

// ============================================================================

} // namespace RTP
} // namespace VMP

// ============================================================================

#endif // _VMP_RTP_TClockSharer_H_

// ============================================================================

