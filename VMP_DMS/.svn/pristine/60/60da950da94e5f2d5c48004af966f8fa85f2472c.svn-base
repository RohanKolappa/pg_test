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
//	BFC.Base.FastCSLocker.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_FastCSLocker_H_
#define _BFC_Base_FastCSLocker_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.FastCS.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief A FastCS locker.
///
/// \ingroup BFC_Base

class BFC_Light_DLL FastCSLocker {

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

	FastCS &	fastCS;	///< FastCS to enter/leave.

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

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_FastCSLocker_H_

// ============================================================================

