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
//	BFC.Base.BusyCS.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_Base_BusyCS_H_
#define _BFC_Base_BusyCS_H_

// ============================================================================

#include "BFC.Light.DLL.h"

// ============================================================================

#if defined( PLATFORM_LINUX ) && !defined( PLATFORM_EMBLINUX)
#include "BFC.Base.Types.h"
#else
#include "BFC.Base.FastCS.h"
#endif

// ============================================================================

namespace BFC {

// ============================================================================

/// \brief [no brief description]
///
/// \ingroup BFC_Base

#if defined( PLATFORM_LINUX ) && !defined( PLATFORM_EMBLINUX)

class BFC_Light_DLL BusyCS {

public :

	/// \brief Creates a new BusyCS.

	BusyCS(
	) : cnt( 1 ) {
	}

	void enter(
	);

	void leave(
	);

protected :

private :

	Uint32	cnt;

	/// \brief Forbidden copy constructor.

	BusyCS(
		const	BusyCS &
	);

	/// \brief Forbidden copy operator.

	BusyCS & operator = (
		const	BusyCS &
	);

};

#else // ----------------------------------------------------------------------

class BFC_Light_DLL BusyCS {

public :

	BusyCS(
	) {
	}

	void enter(
	) {
		cs.enter();
	}

	void leave(
	) {
		cs.leave();
	}

protected :

private :

	FastCS	cs;

	/// \brief Forbidden copy constructor.

	BusyCS(
		const	BusyCS &
	);

	/// \brief Forbidden copy operator.

	BusyCS & operator = (
		const	BusyCS &
	);

};

#endif

// ============================================================================

} // namespace BFC

// ============================================================================

#endif // _BFC_Base_BusyCS_H_

// ============================================================================

