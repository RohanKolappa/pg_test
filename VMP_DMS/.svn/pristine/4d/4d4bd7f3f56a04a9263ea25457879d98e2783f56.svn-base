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
//	BFC.MT.EmulSemaphore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_EmulSemaphore_H_
#define _BFC_MT_EmulSemaphore_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.MT.SysLockable.h"

// #define WIN32_REALLY_EMULATE

#if defined( PLATFORM_WIN32 ) && defined( WIN32_REALLY_EMULATE )
#include "BFC.MT.SysSyncEvent.h"
#else
#include "BFC.MT.SysSemaphore.h"
#endif

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

/// \brief An OS-independent semaphore.
///
/// See the documentation of the SysSemaphore class for more information.
///
/// \ingroup BFC_MT

class BFC_MT_DLL EmulSemaphore : public SysLockable {

public :

	EmulSemaphore(
		const	Uint32		initialValue = 0
	);

	virtual ~EmulSemaphore(
	);

	void wait(
	);

	Bool tryWait(
	);

	void post(
	);

	virtual void lock(
	) {
		wait();
	}

	virtual Bool tryLock(
	) {
		return tryWait();
	}

	virtual void unlock(
	) {
		post();
	}

protected :

private :

#if defined( PLATFORM_WIN32 ) && defined( WIN32_REALLY_EMULATE )
	Uint32			count;
	SysSyncEvent		access;
	SysSyncEvent		notNull;
#else
	SysSemaphore		sem;
#endif

	EmulSemaphore(
		const EmulSemaphore&
	);

	EmulSemaphore& operator = (
		const EmulSemaphore&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_EmulSemaphore_H_

// ============================================================================

