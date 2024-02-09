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
//	TBFC.Base.FastCSLocker.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _TBFC_Base_FastCSLocker_H_
#define _TBFC_Base_FastCSLocker_H_

// ============================================================================

#include "TBFC.Light.DLL.h"

// ============================================================================

#include "TBFC.Base.FastCS.h"

// ============================================================================

namespace TBFC {

// ============================================================================

/// \brief A FastCS locker.
///
/// \ingroup TBFC_Base

class TBFC_Light_DLL FastCSLocker {

public :

	/// \brief Creates a new FastCSLocker.
	///
	/// The \a toBeLocked critical section is automatically entered.
	///
	/// \param toBeLocked
	///	The FastCS to automatically enter.

	FastCSLocker(
			FastCS		& toBeLocked
	) : fastCS( toBeLocked ) {
		fastCS.enter();
	}

	/// \brief Destroys this object.
	///
	/// The previously entered critical section is automatically left.

	~FastCSLocker(
	) {
		fastCS.leave();
	}

protected :

private :

	// internal state...

	FastCS		& fastCS;	///< FastCS to enter/leave.

	// others...

	FastCSLocker(
	);

	FastCSLocker(
		const	FastCSLocker&
	);

	FastCSLocker& operator = (
		const	FastCSLocker&
	);

};

// ============================================================================

} // namespace TBFC

// ============================================================================

#endif // _TBFC_Base_FastCSLocker_H_

// ============================================================================

