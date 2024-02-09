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
//	BFC.MT.EmulCS.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_EmulCS_H_
#define _BFC_MT_EmulCS_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#if defined( PLATFORM_WIN32 )
#include "BFC.MT.SysCS.h"
#else
#include "BFC.MT.SysMutex.h"
#endif

#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief An OS-independent fast critical section.
///
/// See the documentation of the SysCS class for more information.
///
/// \ingroup BFC_MT

class BFC_MT_DLL EmulCS : public SysLockable {

public :

#if defined( PLATFORM_WIN32 )
	EmulCS(
	) {
	}
#elif defined( PLATFORM_LINUX )
	EmulCS(
	) : lockable( SysMutex::Fast ) {
	}
#else
	EmulCS(
	) : lockable( SysMutex::Normal ) {
	}
#endif

	virtual ~EmulCS(
	) {
	}

	virtual void lock(
	) {
		lockable.lock();
	}

	virtual void unlock(
	) {
		lockable.unlock();
	}

protected :

private :

	// internal state...

#if defined( PLATFORM_WIN32 )
	SysCS		lockable;
#else
	SysMutex	lockable;
#endif

	// others...

	EmulCS(
		const	EmulCS&
	);

	EmulCS& operator = (
		const	EmulCS&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_EmulCS_H_

// ============================================================================

