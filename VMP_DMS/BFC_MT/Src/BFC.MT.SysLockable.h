// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::MT".
// 
// "BFC::MT" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::MT" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::MT"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.MT.SysLockable.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysLockable_H_
#define _BFC_MT_SysLockable_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Types.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief Interface class, implemented by classes that can be locked.
///
/// The semantics of the locking/unlocking mechanism is specific to each
/// class inheriting this one.
///
/// \sa SysSemaphore, SysMutex, SysCS, ...

class BFC_MT_DLL SysLockable {

public :

	/// \brief Creates a new SysLockable object.

	SysLockable(
	) {
	}

	/// \brief Destroys this object.

	virtual ~SysLockable(
	) {
	}

	/// \brief Tries to lock this object.

	virtual void lock(
	) = 0;

	/// \brief Unlocks this object.

	virtual void unlock(
	) = 0;

protected :

private :

	// Disable the copy constructor and assignment by default so you will get
	// compiler errors instead of unexpected behaviour if you pass objects
	// by value or assign objects.

	SysLockable(
		const	SysLockable&
	);

	SysLockable& operator = (
		const	SysLockable&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SysLockable_H_

// ============================================================================

