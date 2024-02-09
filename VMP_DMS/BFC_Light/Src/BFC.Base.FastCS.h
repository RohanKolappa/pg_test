// ============================================================================
// 
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
//	BFC.Base.FastCS.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_FastCS_H_
#define _BFC_Base_FastCS_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief An OS-independent fast critical section.
///
/// \ingroup BFC_Base

class BFC_Light_DLL FastCS {

public :

	/// \brief Creates a new FastCS.

	FastCS(
	);

	/// \brief Destroys this object.

	~FastCS(
	);

	/// \brief Enters the critical section.

	void enter(
	);

	/// \brief Leaves the critical section.

	void leave(
	);

protected :

private :

	class Data;

	// internal state...

	Data *	data;

	// others...

	FastCS(
		const	FastCS&
	);

	FastCS& operator = (
		const	FastCS&
	);

};

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_FastCS_H_

// ============================================================================

