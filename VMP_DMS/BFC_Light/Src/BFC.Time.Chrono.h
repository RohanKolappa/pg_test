// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::Light".
// 
// "BFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.Time.Chrono.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _BFC_Time_Chrono_H_
#define _BFC_Time_Chrono_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Time.Clock.h"
#include "BFC.Time.Delta.h"

// ============================================================================

namespace BFC {
namespace Time {

// ============================================================================

/// \brief Simple to use chronometer.
///
/// \ingroup BFC_Time

class BFC_Light_DLL Chrono {

public :

	Chrono(
		const	Bool		pAutoStart = false
	);

	void start(
	);

	void stop(
	);

	Bool isStarted(
	) const {
		return startFlag;
	}

	Clock getStartTime(
	) const {
		return startTime;
	}

	Clock getStopTime(
	) const {
		return stopTime;
	}

	Delta getElapsed(
	) const;

	Delta getAverage(
	) const;

	void resetAverage(
	);

protected :

private :

	Clock		startTime;
	Clock		stopTime;
	Bool		startFlag;
	Delta		avrgTotal;
	Uint32		avrgCount;

	// others...

	Chrono(
		const	Chrono&
	);

	Chrono& operator = (
		const	Chrono&
	);

};

// ============================================================================

} // namespace Time
} // namespace BFC

// ============================================================================

#endif // _BFC_Time_Chrono_H_

// ============================================================================

