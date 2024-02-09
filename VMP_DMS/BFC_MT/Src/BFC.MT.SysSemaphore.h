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
//	BFC.MT.SysSemaphore.h
// 
// Description:
//	...
// 
// Author(s):
//	Jean-Francois GOBBERS
// 
// ============================================================================

#ifndef _BFC_MT_SysSemaphore_H_
#define _BFC_MT_SysSemaphore_H_

// ============================================================================

#include "BFC.MT.DLL.h"

// ============================================================================

#include "BFC.Base.Exception.h"

#include "BFC.MT.SysLockable.h"

// ============================================================================

namespace BFC {
namespace MT {

// ============================================================================

struct SysSemaphoreData;

// ============================================================================

/// \brief OS-dependent semaphore (Linux only).
///
/// Semaphores are counters for resources shared between threads. The basic
/// operations on semaphores are: increment the counter atomically, and wait
/// until the counter is non-null and decrement it atomically.
///
/// \ingroup BFC_MT

class BFC_MT_DLL SysSemaphore : public SysLockable {

public :

	/// \brief Creates a new SysSemaphore.
	///
	/// This constructor creates a new SysSemaphore object, and initializes
	/// it. The count associated with the semaphore is set initially to \a
	/// initialValue.

	SysSemaphore(
		const	Uint32	initialValue = 0
	);

	/// \brief Destructor.
	///
	/// The destructor destroys this object, freeing the resources it might
	/// hold. No threads should be waiting on the semaphore at the time the
	/// destructor is called.

	virtual ~SysSemaphore(
	);

	/// \brief Suspends the calling thread.
	///
	/// This method suspends the calling thread until the semaphore has a
	/// non-zero count. It then atomically decreases the semaphore count.
	///
	/// \throws CantWait

	void wait(
	);

	/// \brief Tries to decrease the count.
	///
	/// This method is a non-blocking variant of wait(). If the semaphore
	/// has a non-zero count, the count is atomically decreased and this
	/// method immediately returns TRUE. If the semaphore count is zero,
	/// this method immediately returns FALSE.

	Bool tryWait(
	);

	/// \brief Atomically increases the count.
	///
	/// This method atomically increases the count of this object.
	///
	/// This method never blocks.
	///
	/// \throws CantPost

	void post(
	);

	virtual void lock(
	) {
		wait();
	}

	virtual void unlock(
	) {
		post();
	}

	// For the exceptions...

	BASE_CLASS_GEN_EXCEPTION("BFC.SysSemaphore");

	CLASS_EXCEPTION(CantPost, "Can't post");
	CLASS_EXCEPTION(CantWait, "Can't wait");

protected :

private :

	// internal state...

	SysSemaphoreData	*data;

	// others...

	SysSemaphore(
		const SysSemaphore&
	);

	SysSemaphore& operator = (
		const SysSemaphore&
	);

};

// ============================================================================

} // namespace MT
} // namespace BFC

// ============================================================================

#endif // _BFC_MT_SysSemaphore_H_

// ============================================================================


