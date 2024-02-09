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
//	BFC.MT.Lockable.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_Lockable_H_
#define _BFC_MT_Lockable_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

class BFC_MT_DLL Lockable : public SysLockable {

public :

	Lockable(
	) {
	}

	virtual ~Lockable(
	) {
	}

	virtual void lock(
	) {
		lock( true );
	}

	/// \brief Thread-aware version of the lock() method.

	virtual void lock(
		const	Bool		checkStopped
	) = 0;

protected :

private :

	// others...

	Lockable(
		const	Lockable&
	);

	Lockable& operator = (
		const	Lockable&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_Lockable_H_

// ============================================================================

