// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "TBFC::Light".
// 
// "TBFC::Light" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "TBFC::Light" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "TBFC::Light"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	TBFC.Time.Chrono.h
// 
// Description:
//	...
// 
// Author(s):
//	...
// 
// ============================================================================

#ifndef _TBFC_Time_Chrono_H_
#define _TBFC_Time_Chrono_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Time.Clock.h"
#include "TBFC.Time.Delta.h"

// ============================================================================

namespace TBFC {
namespace Time {

// ============================================================================

/// \brief Simple to use chronometer.
///
/// \ingroup TBFC_Time

class TBFC_Light_DLL Chrono {

public :

	Chrono(
	);

	~Chrono(
	);

	void start(
	);

	void stop(
	);

	Bool isStarted(
	) const;

	Clock getStartTime() const;
	Clock getStopTime() const;
	Delta getElapsed() const;

protected :

private :

	// internal state...

	Clock		startTime;
	Clock		stopTime;

	Bool		startFlag;

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
} // namespace TBFC

// ============================================================================

#endif // _TBFC_Time_Chrono_H_

// ============================================================================

